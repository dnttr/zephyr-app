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
    };

    glm::vec2 generate_random(
        std::mt19937 &random,
        std::uniform_real_distribution<float> &distX,
        std::uniform_real_distribution<float> &distY,
        float cellSize,
        int cells_count_x, int cells_count_y,
        const std::vector<std::vector<int>> &grid,
        const std::vector<glm::vec2> &positions,
        float minDistance)
    {
        const int maxAttempts = 20;

        for (int attempt = 0; attempt < maxAttempts; ++attempt)
        {
            glm::vec2 candidate(distX(random), distY(random));

            int cell_x = static_cast<int>(candidate.x / cellSize);
            int cell_y = static_cast<int>(candidate.y / cellSize);

            cell_x = std::clamp(cell_x, 0, cells_count_x - 1);
            cell_y = std::clamp(cell_y, 0, cells_count_y - 1);

            bool tooClose = false;

            for (int _x = -1; _x <= 1; ++_x)
            {
                for (int _y = -1; _y <= 1; ++_y)
                {
                    int neighbour_x = cell_x + _x;
                    int neighbour_y = cell_y + _y;

                    if (neighbour_x < 0 || neighbour_x >= cells_count_x ||
                        neighbour_y < 0 || neighbour_y >= cells_count_y)
                        continue;

                    const auto &balls = grid[neighbour_y * cells_count_x + neighbour_x];
                    for (int j : balls)
                    {
                        if (glm::distance(candidate, positions[j]) < minDistance)
                        {
                            tooClose = true;
                            break;
                        }
                    }
                    if (tooClose) break;
                }
                if (tooClose) break;
            }

            if (!tooClose)
            {
                return candidate;
            }
        }

        return glm::vec2(distX(random), distY(random));
    }
}
