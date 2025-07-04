//
// Created by Damian Netter on 04/07/2025.
//

#pragma once

#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
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
            sockfd = -1;
            java_pid = -1;
        }

        ~client()
        {
            disconnect();
        }

        bool start()
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

                execl("/usr/bin/java", "java", "-jar", "runnable.jar", nullptr);
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

        void disconnect()
        {
        }
    };
}
