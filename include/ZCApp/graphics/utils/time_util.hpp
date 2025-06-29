//
// Created by Damian Netter on 29/06/2025.
//

#pragma once
#include <chrono>

namespace zc_app
{
    class time_util
    {
    public:
        static std::chrono::steady_clock::time_point get_current_time()
        {
            return std::chrono::high_resolution_clock::now();
        }

        static float get_elapsed_time(const std::chrono::steady_clock::time_point start_time)
        {
            const std::chrono::duration<float> elapsed = get_current_time() - start_time;
            return elapsed.count();
        }
    };
}
