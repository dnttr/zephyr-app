//
// Created by Damian Netter on 30/06/2025.
//

#pragma once

#include <random>
#include <glm/glm.hpp>

namespace zc_app
{
    class math_util
    {
    public:
        static glm::vec2 get_random(std::mt19937 &random, std::uniform_real_distribution<float> &dist_x,
                                    std::uniform_real_distribution<float> &dist_y,
                                    const std::vector<glm::vec2> &positions, float min_distance)
        {
            glm::vec2 position;
            bool valid;

            do
            {
                position = glm::vec2(dist_x(random), dist_y(random));
                valid = true;

                for (auto vec : positions)
                {
                    if (glm::distance(position, vec) < min_distance)
                    {
                        valid = false;
                        break;
                    }
                }
            }
            while (!valid);

            return position;
        }

        // math is beyond me, so I just copied this from the internet
        static float ease_out(float t)
        {
            t -= 1.0F;
            return t * t * t + 1.0F;
        }
    };
}
