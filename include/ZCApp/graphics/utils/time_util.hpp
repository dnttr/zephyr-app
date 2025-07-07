//
// Created by Damian Netter on 29/06/2025.
//

#pragma once
#include <chrono>

namespace zc_app
{
    class time_util
    {
        // Use high_resolution_clock::time_point consistently
        inline static std::chrono::high_resolution_clock::time_point last_time =
            std::chrono::high_resolution_clock::now();

    public:
        static float get_delta_time()
        {
            const auto current_time = get_current_time();
            // Call get_elapsed_time with high_resolution_clock::time_point
            const float delta_time = get_elapsed_time(last_time);
            last_time = current_time;
            return delta_time;
        }

        // Return type changed to be consistent with high_resolution_clock
        static std::chrono::high_resolution_clock::time_point get_current_time()
        {
            return std::chrono::high_resolution_clock::now();
        }

        // Parameter type changed to be consistent
        static float get_elapsed_time(const std::chrono::high_resolution_clock::time_point start_time)
        {
            // Explicitly cast to seconds<float> to be absolutely sure of the unit
            const std::chrono::duration<float> elapsed =
                std::chrono::duration_cast<std::chrono::duration<float>>(get_current_time() - start_time);
            return elapsed.count();
        }
    };
}