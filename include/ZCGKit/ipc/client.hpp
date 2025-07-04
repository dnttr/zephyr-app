//
// Created by Damian Netter on 04/07/2025.
//

#pragma once

#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define MAX 10
// In mb

namespace zcg_kit
{
    class client
    {
        int sockfd;
        bool connected;

        pid_t java_pid;

    public:
        client()
        {
            connected = false;
            sockfd = -1;
            java_pid = -1;
        }

        ~client()
        {
            disconnect();
        }

        bool start(const std::string &path)
        {
            int sv[2];

            if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
            {
                perror("[ERROR] socketpair failed");

                return false;
            }

            java_pid = fork();

            if (java_pid == 0)
            {
                close(sv[0]);

                std::cerr << "[CHILD] Child process (PID: " << getpid() << ") forked. Redirecting FDs." << std::endl;

                dup2(sv[1], STDIN_FILENO);
                dup2(sv[1], STDOUT_FILENO);

                close(sv[1]);

                const int devNullFd = open("/dev/null", O_WRONLY);

                if (devNullFd == -1)
                {
                    perror("[CHILD ERROR] Failed to open /dev/null");
                    exit(1);
                }

                if (dup2(devNullFd, STDERR_FILENO) == -1)
                {
                    perror("[CHILD ERROR] Failed to dup2 stderr to /dev/null");
                    close(devNullFd);
                    exit(1);
                }
                close(devNullFd);

                std::cerr << "[CHILD] Executing Java JAR: /usr/bin/java -jar " << path << std::endl;
                execl("/usr/bin/java", "java", "-Djava.awt.headless=true", "-jar", path.c_str(), (char *)nullptr);

                perror("[CHILD ERROR] execl failed");
                exit(1);
            }

            if (java_pid > 0)
            {
                close(sv[1]);
                sockfd = sv[0];
                connected = true;

                std::cerr << "[PARENT] Forked Java PID: " << java_pid << ". Connected to socket FD: " << sockfd <<
                    std::endl;

                return true;
            }

            perror("[ERROR] fork failed");

            return false;
        }

        void send_message(const std::string &message)
        {
            if (!connected)
            {
                return;
            }

            const uint32_t length = message.length();
            const uint32_t network_length = htonl(length);

            ssize_t bytes_sent = send(sockfd, &network_length, sizeof(network_length), 0);
            if (bytes_sent < 0)
            {
                perror("[ERROR] send length failed");
                disconnect();

                return;
            }

            if (bytes_sent != sizeof(network_length))
            {
                disconnect();

                return;
            }

            bytes_sent = send(sockfd, message.c_str(), length, 0);

            if (bytes_sent < 0)
            {
                perror("[ERROR] send data failed");
                disconnect();
            }
            else if (bytes_sent != length)
            {
                std::cerr << "[ERROR] send data sent " << bytes_sent << " instead of " << length << " bytes." <<
                    std::endl;

                disconnect();
            }
            else
            {
                std::cerr << "[SEND] Successfully sent " << bytes_sent << " bytes of message data." << std::endl;
            }
        }

        [[nodiscard]] std::string read()
        {
            if (!connected)
            {
                return "";
            }

            uint32_t network_length;
            ssize_t bytes_received = 0;
            size_t total_read = 0;

            while (total_read < sizeof(network_length))
            {
                bytes_received = recv(sockfd, reinterpret_cast<char *>(&network_length) + total_read,
                                      sizeof(network_length) - total_read, 0);

                if (bytes_received <= 0)
                {
                    if (bytes_received != 0)
                    {
                        perror("[C++ Client READ ERROR] recv length failed");
                    }

                    disconnect();

                    return "";
                }

                total_read += bytes_received;
            }

            const uint32_t message_length = ntohl(network_length);

            if (message_length == 0)
            {
                return "";
            }

            if (message_length > 1024 * 1024 * MAX)
            {
                std::cerr << "[C++ Client READ ERROR] Received excessively large message length: " << message_length <<
                    ". Disconnecting." << std::endl;

                disconnect();

                return "";
            }

            std::vector<char> buffer(message_length);
            total_read = 0;

            while (total_read < message_length)
            {
                bytes_received = recv(sockfd, buffer.data() + total_read, message_length - total_read, 0);

                if (bytes_received <= 0)
                {
                    if (bytes_received != 0)
                    {
                        perror("[C++ Client READ ERROR] recv data failed");
                    }
                    disconnect();
                    return "";
                }

                total_read += bytes_received;
            }

            std::string received_message(buffer.begin(), buffer.end());

            return received_message;
        }


        void disconnect()
        {
            if (connected)
            {
                if (sockfd >= 0)
                {
                    close(sockfd);
                    sockfd = -1;
                }

                if (java_pid > 0)
                {
                    kill(java_pid, SIGTERM);

                    int status;

                    if (waitpid(java_pid, &status, 0) == -1)
                    {
                        perror("[ERROR] waitpid failed");
                    }
                    else
                    {
                        if (WIFEXITED(status))
                        {
                            std::cerr << "[C++ Client] Java process exited with status " << WEXITSTATUS(status) <<
                                std::endl;
                        }
                        else if (WIFSIGNALED(status))
                        {
                            std::cerr << "[C++ Client] Java process terminated by signal " << WTERMSIG(status) <<
                                std::endl;
                        }
                    }

                    java_pid = -1;
                }

                connected = false;
            }
            else
            {
                std::cerr << "[C++ Client] Already disconnected." << std::endl;
            }
        }

        [[nodiscard]] bool is_connected() const
        {
            return connected;
        }
    };
}
