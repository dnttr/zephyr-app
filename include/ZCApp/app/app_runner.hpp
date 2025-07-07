//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <condition_variable>
#include <string>

namespace zc_kit
{
    class app_runner
    {
        friend class bridge;

        static bool ready;

        static std::mutex mtx;
        static std::condition_variable condition;

    public:
        static void run_application();
        static void run_test(const std::string &role);
    };
}
