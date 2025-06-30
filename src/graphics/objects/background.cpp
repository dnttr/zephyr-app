//
// Created by Damian Netter on 30/06/2025.
//

#include "ZCApp/graphics/objects/background.hpp""

#include <iostream>
#include <random>

#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCApp/graphics/utils/time_util.hpp"
#include "ZCGKit/external.hpp"

namespace zc_app
{
    constexpr int NUM_BALLS = 100;

    glm::vec2 ballTargetPositions[NUM_BALLS];
    glm::vec2 ballPositions[NUM_BALLS];
    glm::vec2 ballTargetVelocity[NUM_BALLS];

    void background::fetch_uniforms()
    {
        u_point_projection = glGetUniformLocation(point_program, "projection_matrix");
        u_point_ball_radius = glGetUniformLocation(point_program, "ball_radius");
        u_point_color = glGetUniformLocation(point_program, "ball_color");
    }

    void background::setup()
    {
        point_program = shaders::create_program("effect_vert", "effect_frag");

        fetch_uniforms();

        glGenBuffers(1, &point_vbo);
        glGenBuffers(1, &point_instanced_vbo);
        glGenBuffers(1, &point_ebo);

        glGenVertexArrays(1, &point_vao);
        glBindVertexArray(point_vao);

        const float vertices[]
        {
            0.0F, 0.0F,
            1.0F, 0.0F,
            1.0F, 1.0F,
            0.0F, 1.0F
        };

        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        std::pair<float, float> effective_virtual_dimensions = perspective_util::get_effective_virtual_dimensions();
        std::random_device rd;
        std::mt19937 random(rd());
        std::uniform_real_distribution<float> distX(0, effective_virtual_dimensions.first / 2);
        std::uniform_real_distribution<float> distY(0, effective_virtual_dimensions.second / 2);

        for (auto &ballPosition : ballPositions)
        {
            ballPosition = glm::vec2(
                distX(random),
                distY(random)
            );
        }

        for (auto &ball_target_position : ballTargetPositions)
        {
            ball_target_position = glm::vec2(
                distX(random),
                distY(random)
            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, point_instanced_vbo);
        glBufferData(GL_ARRAY_BUFFER, NUM_BALLS * sizeof(glm::vec2), &ballPositions[0], GL_DYNAMIC_DRAW);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);

        glVertexAttribDivisor(1, 1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    float velocity = 100.0f;
    std::pair<float, float> evd;
    std::uniform_real_distribution<float> distX, distY;

    std::random_device rd;
    std::mt19937 random(rd());

    std::vector<std::vector<int>> grid;

    int cellSize = 30;
    constexpr float searchRadius = 10.0f;
    constexpr float searchRadiusSquared = searchRadius * searchRadius;

    int cells_count_x = 0;
    int cells_count_y = 0;

    void background::render()
    {
        glUseProgram(point_program);

        const float time = time_util::get_delta_time();

        if (evd.first == 0 || evd.second == 0)
        {
            evd = perspective_util::get_effective_virtual_dimensions();

            distX = std::uniform_real_distribution<float>(0, evd.first);
            distY = std::uniform_real_distribution<float>(0, evd.second);

            cells_count_x = evd.first / cellSize + 1;
            cells_count_y = evd.second / cellSize + 1;

            for (auto &velocity : ballTargetVelocity)
            {
                float angle = std::uniform_real_distribution<float>(0.0f, 2.0f * M_PI)(random);
                velocity = glm::vec2(cos(angle), sin(angle)) * 50.0f;
            }

            grid.assign(cells_count_x * cells_count_y, std::vector<int>());
        }
        else
        {
            for (auto &cell : grid)
            {
                cell.clear();
            }
        }

        for (int i = 0; i < NUM_BALLS; ++i)
        {
            const int cell_x = std::clamp(static_cast<int>(ballPositions[i].x) / cellSize, 0, cells_count_x - 1);
            const int cell_y = std::clamp(static_cast<int>(ballPositions[i].y) / cellSize, 0, cells_count_y - 1);

            grid[cell_y * cells_count_x + cell_x].push_back(i);
        }

        for (int i = 0; i < NUM_BALLS; ++i)
        {
            int cell_x = static_cast<int>(ballPositions[i].x) / cellSize;
            int cell_y = static_cast<int>(ballPositions[i].y) / cellSize;

            cell_x = std::clamp(cell_x, 0, cells_count_x - 1);
            cell_y = std::clamp(cell_y, 0, cells_count_y - 1);

            for (int _x = -1; _x <= 1; ++_x)
            {
                for (int _y = -1; _y <= 1; ++_y)
                {
                    const int neighbour_x = cell_x + _x;
                    const int neighbour_y = cell_y + _y;

                    if (neighbour_x < 0 || neighbour_x >= cells_count_x ||
                        neighbour_y < 0 || neighbour_y >= cells_count_y)
                    {
                        continue;
                    }

                    const auto &balls = grid[neighbour_y * cells_count_x + neighbour_x];

                    for (int j : balls)
                    {
                        if (i == j)
                        {
                            continue;
                        }

                        glm::vec2 delta = ballPositions[i] - ballPositions[j];
                        float distance = glm::length(delta);
                        if (distance < searchRadius && distance > 0.0f)
                        {
                            glm::vec2 normal = delta / distance;
                            float overlap = searchRadius - distance;

                            ballPositions[i] += normal * overlap * 0.5f;
                            ballPositions[j] -= normal * overlap * 0.5f;

                            float v1 = glm::dot(ballTargetVelocity[i], normal);
                            float v2 = glm::dot(ballTargetVelocity[j], normal);

                            float m1 = 1.0F;
                            float m2 = 1.0F;

                            float p = (2.0F * (v1 - v2)) / (m1 + m2);
                            ballTargetVelocity[i] = ballTargetVelocity[i] - p * m2 * normal;
                            ballTargetVelocity[j] = ballTargetVelocity[j] + p * m1 * normal;
                        }
                    }
                }
            }

            auto &ball_position = ballPositions[i];
            auto &target_position = ballTargetPositions[i];
            glm::vec2 distance = target_position - ball_position;

            std::cout << "Ball " << i << " Position: (" << ball_position.x << ", " << ball_position.y << ") "
                      << "Target: (" << target_position.x << ", " << target_position.y << ") "
                      << "Distance: (" << distance.x << ", " << distance.y << ")" << std::endl;

            if (glm::length(distance) < velocity * time)
            {
                ball_position = target_position;
                target_position = glm::vec2(distX(random), distY(random));
                glm::vec2 direction = glm::normalize(target_position - ball_position);
                ballTargetVelocity[i] = direction * velocity;
            }
            else
            {
                auto vec = ballTargetVelocity[i] * time;


                ball_position += vec;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, point_instanced_vbo);

        if (auto *ptr = static_cast<glm::vec2 *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)))
        {
            memcpy(ptr, ballPositions, NUM_BALLS * sizeof(glm::vec2));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (need_update)
        {
            glUniform1f(u_point_ball_radius, 5.0f);
            glUniform3f(u_point_color, 0.0f, 0.0f, 1.0f);
            glUniformMatrix4fv(u_point_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());

            need_update = false;
        }

        glBindVertexArray(point_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 100);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void background::reshape() const
    {
        glUniformMatrix4fv(u_point_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());
    }

    background::~background()
    {
        if (zcg_kit::external::is_context_valid())
        {
            glDeleteProgram(point_program);
            point_program = 0;

            glDeleteProgram(line_program);
            line_program = 0;

            glDeleteBuffers(1, &point_vbo);
            point_vbo = 0;

            glDeleteBuffers(1, &point_instanced_vbo);
            point_instanced_vbo = 0;

            glDeleteBuffers(1, &point_ebo);
            point_ebo = 0;

            glDeleteVertexArrays(1, &point_vao);
            point_vao = 0;

            glDeleteBuffers(1, &line_vbo);
            line_vbo = 0;

            glDeleteBuffers(1, &line_ebo);
            line_ebo = 0;

            glDeleteVertexArrays(1, &line_vao);
            line_vao = 0;
        }
    }
}
