//
// Created by Damian Netter on 04/07/2025.
//


#pragma once

#include <atomic>
#include <csignal>
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

namespace zcg_kit
{
    class client
    {
        std::atomic<bool> connected;
        std::string read_buffer;

        int ipc_socket_fd;

        pid_t java_pid;

        static std::string read_line_from_fd(int fd)
        {
            std::string line;
            char c;

            while (::read(fd, &c, 1) > 0)
            {
                if (c == '\n')
                {
                    break;
                }

                line += c;
            }

            return line;
        }

    public:
        client() : connected(false), ipc_socket_fd(-1), java_pid(-1)
        {
        }

        ~client()
        {
            disconnect();
        }

        bool start(const std::string &path)
        {
            int child_stdout_pipe[2];

            if (pipe(child_stdout_pipe) < 0)
            {
                perror("[C++ Client ERROR] stdout pipe failed");

                return false;
            }

            java_pid = fork();

            if (java_pid == 0)
            {
                close(child_stdout_pipe[0]);
                dup2(child_stdout_pipe[1], STDOUT_FILENO);
                close(child_stdout_pipe[1]);

                execl("/usr/bin/java", "java", "-Djava.awt.headless=true", "-jar", path.c_str(), nullptr);
                perror("[C++ Client CHILD ERROR] execl failed");

                _exit(1);
            }
            if (java_pid > 0)
            {
                close(child_stdout_pipe[1]);

                const std::string line = read_line_from_fd(child_stdout_pipe[0]);

                close(child_stdout_pipe[0]);

                int port = 0;
                const std::string prefix = "IPC_PORT:";

                if (line.rfind(prefix, 0) == 0)
                {
                    try
                    {
                        port = std::stoi(line.substr(prefix.length()));
                    }

                    catch (...)
                    {
                        std::cerr << "[C++ Client ERROR] Failed to parse port from Java: " << line << std::endl;
                        return false;
                    }
                }
                else
                {
                    std::cerr << "[C++ Client ERROR] Expected IPC_PORT:, got: " << line << std::endl;
                    return false;
                }

                if (port == 0)
                {
                    return false;
                }

                ipc_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

                if (ipc_socket_fd < 0)
                {
                    perror("[C++ Client ERROR] IPC socket creation failed");
                    return false;
                }

                sockaddr_in serv_addr{};

                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port);

                inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

                if (connect(ipc_socket_fd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0)
                {
                    perror("[C++ Client ERROR] IPC socket connection failed");
                    return false;
                }

                connected = true;

                std::cout << "[C++ Client] IPC connection established. Forked Java PID: " << java_pid << std::endl;

                return true;
            }

            perror("[C++ Client ERROR] fork failed");
            return false;
        }

        void send_message(const std::string &message)
        {
            if (!connected)
            {
                return;
            }

            const auto message_with_newline = message + "\n";

            if (send(ipc_socket_fd, message_with_newline.c_str(), message_with_newline.length(), 0) < 0)
            {
                perror("[C++ Client ERROR] Failed to send message");

                disconnect();
            }
        }

        [[nodiscard]] std::string read()
        {
            if (!connected)
            {
                return "";
            }

            while (true)
            {
                const auto newline_pos = read_buffer.find('\n');

                if (newline_pos != std::string::npos)
                {
                    std::string message = read_buffer.substr(0, newline_pos);
                    read_buffer.erase(0, newline_pos + 1);
                    return message;
                }

                char buffer[8192];

                const auto rx = recv(ipc_socket_fd, buffer, sizeof(buffer), 0);

                if (rx > 0)
                {
                    read_buffer.append(buffer, rx);
                }
                else
                {
                    disconnect();

                    return "";
                }
            }
        }

        void disconnect()
        {
            if (!connected.exchange(false))
            {
                return;
            }

            if (ipc_socket_fd >= 0)
            {
                close(ipc_socket_fd);

                ipc_socket_fd = -1;
            }

            if (java_pid > 0)
            {
                kill(java_pid, SIGTERM);

                int status;
                waitpid(java_pid, &status, 0);

                java_pid = -1;
            }
        }

        [[nodiscard]] bool is_connected() const { return connected; }
    };
}
