//
// Created by Damian Netter on 04/07/2025.
//

#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <csignal>
#include <string>
#include <sys/wait.h>

namespace zcg_kit
{
    class client
    {
        int sockfd;
        pid_t java_pid;

        bool connected;

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

        bool start(std::string path)
        {
            int sv[2];

            if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
            {
                perror("socketpair");
                return false;
            }

            java_pid = fork();

            if (java_pid == 0)
            {
                close(sv[0]);
                dup2(sv[1], STDIN_FILENO);
                dup2(sv[1], STDOUT_FILENO);
                close(sv[1]);

                //that may require a bit of attention, if embedded
                execl("/usr/bin/java", "java", "-jar", path.c_str(), nullptr);
                perror("execl");
                exit(1);
            }

            if (java_pid > 0)
            {
                close(sv[1]);
                sockfd = sv[0];
                connected = true;
                return true;
            }

            perror("fork");
            return false;
        }

        void send_message(const std::string &message)
        {
            if (!connected)
            {
                return;
            }

            const std::string buffer = message + "\n";

            if (send(sockfd, buffer.c_str(), buffer.size(), 0) < 0)
            {
                perror("send");
                disconnect();
            }
        }

        [[nodiscard]] std::string read() const
        {
            if (!connected)
            {
                return "";
            }

            char buffer[4096];

            if (const ssize_t bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0); bytes > 0)
            {
                buffer[bytes] = '\0';
                return {buffer};
            }

            return "";
        }

        void disconnect()
        {
            if (!connected)
            {
                if (sockfd >= 0)
                {
                    close(sockfd);
                    sockfd = -1;
                }

                if (java_pid > 0)
                {
                    kill(java_pid, SIGTERM);
                    waitpid(java_pid, nullptr, 0);
                }

                connected = false;
            }
        }

        [[nodiscard]] bool is_connected() const
        {
            return connected;
        }
    };
}
