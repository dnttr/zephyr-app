//
// Created by Damian Netter on 30/06/2025.
//

#include "ZCApp/graphics/objects/background.hpp"

#include <iostream>
#include <random>

#include "ZCApp/graphics/framebuffer.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/math_util.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCApp/graphics/utils/time_util.hpp"
#include "ZCGKit/external.hpp"

namespace zc_app
{
    constexpr int NUM_BALLS = 100;

    std::uniform_real_distribution<float> background::alpha = std::uniform_real_distribution(0.1F, 1.0F);

    glm::vec2 ball_target_positions[NUM_BALLS];
    glm::vec2 ball_positions[NUM_BALLS];
    glm::vec2 ball_velocities[NUM_BALLS];

    std::vector<glm::vec2> line_start_points;
    std::vector<glm::vec2> line_end_points;
    std::vector<float> line_alphas;

    framebuffer fbo_1, fbo_2;

    void background::fetch_uniforms()
    {
        u_point_projection = glGetUniformLocation(point_program, "projection_matrix");
        u_point_ball_radius = glGetUniformLocation(point_program, "ball_radius");
        u_point_color = glGetUniformLocation(point_program, "ball_color");

        u_line_projection = glGetUniformLocation(line_program, "projection_matrix");
        u_line_ball_radius = glGetUniformLocation(line_program, "ball_radius");
        u_line_width = glGetUniformLocation(line_program, "line_width_pixels");
        u_line_color = glGetUniformLocation(line_program, "line_color");

        u_blur_projection = glGetUniformLocation(blur_program, "projection_matrix");

        u_blur_texture = glGetUniformLocation(blur_program, "tex");
        u_blur_radius = glGetUniformLocation(blur_program, "blur_radius");
        u_blur_size = glGetUniformLocation(blur_program, "tex_size");
        u_blur_tint_color = glGetUniformLocation(blur_program, "tint_color");
        u_blur_tint_strength = glGetUniformLocation(blur_program, "tint_strength");
        u_blur_quality = glGetUniformLocation(blur_program, "sample_count");

        u_passthrough_screen_texture = glGetUniformLocation(passthrough_program, "screenTexture");
    }

    void background::setup()
    {
        point_program = shaders::create_program("effect_vert", "effect_frag");
        line_program = shaders::create_program("line_vert", "line_frag");
        blur_program = shaders::create_program("fs_blur_vert", "fs_blur_frag");
        passthrough_program = shaders::create_program("fs_blur_vert", "passthrough_frag");

        fetch_uniforms();

        glGenBuffers(1, &point_vbo);
        glGenBuffers(1, &point_instanced_vbo);
        glGenBuffers(1, &point_ebo);

        glGenBuffers(1, &line_vbo);
        glGenBuffers(1, &line_ebo);
        glGenBuffers(1, &line_start_vbo);
        glGenBuffers(1, &line_end_vbo);
        glGenBuffers(1, &line_alpha_vbo);

        glGenBuffers(1, &blur_vbo);
        glGenBuffers(1, &blur_ebo);

        glGenVertexArrays(1, &blur_vao);
        glGenVertexArrays(1, &line_vao);
        glGenVertexArrays(1, &point_vao);

        constexpr float vertices[]
        {
            0.0F, 0.0F,
            1.0F, 0.0F,
            1.0F, 1.0F,
            0.0F, 1.0F
        };

        constexpr float vertices_blur[] {
            0.0F, 0.0F, 0.0F, 1.0F,
            1.0F, 0.0F, 1.0F, 1.0F,
            1.0F, 1.0F, 1.0F, 0.0F,
            0.0F, 1.0F, 0.0F, 0.0F
        };

        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        auto [width, height] = perspective_util::get_effective_virtual_dimensions();
        std::random_device rd;
        std::mt19937 random(rd());
        std::uniform_real_distribution<float> distX(0, width / 2);
        std::uniform_real_distribution<float> distY(0, height / 2);

        std::vector<glm::vec2> existingPositions{};

        for (auto &ballPosition : ball_positions)
        {
            ballPosition = math_util::get_random(random, distX, distY, existingPositions, 15.0f);

            existingPositions.push_back(ballPosition);
        }

        for (auto &ball_target_position : ball_target_positions)
        {
            ball_target_position = math_util::get_random(random, distX, distY, existingPositions, 15.0f);
        }

        glBindVertexArray(blur_vao);

        glBindBuffer(GL_ARRAY_BUFFER, blur_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_blur), vertices_blur, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blur_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(point_vao);

        glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, point_instanced_vbo);
        glBufferData(GL_ARRAY_BUFFER, NUM_BALLS * sizeof(glm::vec2), &ball_positions[0], GL_DYNAMIC_DRAW);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);

        glVertexAttribDivisor(1, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(line_vao);
        glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, line_start_vbo);
        glBufferData(GL_ARRAY_BUFFER, NUM_BALLS * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        glBindBuffer(GL_ARRAY_BUFFER, line_end_vbo);
        glBufferData(GL_ARRAY_BUFFER, NUM_BALLS * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);

        glBindBuffer(GL_ARRAY_BUFFER, line_alpha_vbo);
        glBufferData(GL_ARRAY_BUFFER, NUM_BALLS * NUM_BALLS * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void background::update()
    {
        line_start_points.clear();
        line_end_points.clear();
        line_alphas.clear();

        const float time = time_util::get_delta_time();

        if (evd.first == 0 || evd.second == 0)
        {
            auto [width, height] = perspective_util::get_effective_virtual_dimensions();
            evd = {width, height};

            dist_x = std::uniform_real_distribution<float>(0, width);
            dist_y = std::uniform_real_distribution<float>(0, height);

            cells_count_x = static_cast<int>(width) / cellSize + 1;
            cells_count_y = static_cast<int>(height) / cellSize + 1;

            for (auto &velocity : ball_velocities)
            {
                const float angle = std::uniform_real_distribution<float>(0.0f, 2.0f * M_PI)(random);
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
            const int cell_x = std::clamp(static_cast<int>(ball_positions[i].x) / cellSize, 0, cells_count_x - 1);
            const int cell_y = std::clamp(static_cast<int>(ball_positions[i].y) / cellSize, 0, cells_count_y - 1);

            grid[cell_y * cells_count_x + cell_x].push_back(i);
        }

        for (int i = 0; i < NUM_BALLS; ++i)
        {
            int cell_x = static_cast<int>(ball_positions[i].x) / cellSize;
            int cell_y = static_cast<int>(ball_positions[i].y) / cellSize;

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

                    for (int j : grid[neighbour_y * cells_count_x + neighbour_x])
                    {
                        if (i == j)
                        {
                            continue;
                        }

                        glm::vec2 delta = ball_positions[i] - ball_positions[j];
                        const float distance = glm::length(delta);

                        if (i < j)
                        {
                            if (distance > 0.0f && distance < searchRadius * 5)
                            {
                                line_start_points.push_back(ball_positions[i]);
                                line_end_points.push_back(ball_positions[j]);

                                float normalized_distance = distance / (searchRadius * 5);
                                float distance_alpha = 1.0f - normalized_distance;
                                distance_alpha = std::clamp(distance_alpha * 0.9f + 0.1f, 0.1f, 1.0f);

                                line_alphas.push_back(distance_alpha);
                            }
                        }

                        if (distance > 0.0f && distance < searchRadius)
                        {
                            glm::vec2 normal = delta / distance;
                            const float overlap = searchRadius - distance;

                            ball_positions[i] += normal * overlap * 0.5f;
                            ball_positions[j] -= normal * overlap * 0.5f;

                            const float v1 = glm::dot(ball_velocities[i], normal);
                            const float v2 = glm::dot(ball_velocities[j], normal);

                            constexpr float m1 = 1.0F;
                            constexpr float m2 = 1.0F;

                            const float p = 2.0F * (v1 - v2) / (m1 + m2);

                            ball_velocities[i] = ball_velocities[i] - p * m2 * normal;
                            ball_velocities[j] = ball_velocities[j] + p * m1 * normal;

                            constexpr float collisionSpeedBoost = 2.0f;

                            ball_velocities[i] += normal * collisionSpeedBoost;
                            ball_velocities[j] -= normal * collisionSpeedBoost;

                            constexpr float new_target_distance = 100.0f;

                            constexpr float x = 0.0f;
                            constexpr float y = 0.0f;

                            const float width = evd.first;
                            const float height = evd.second;

                            glm::vec2 dir_i = glm::normalize(ball_target_positions[i] - ball_positions[i]);
                            ball_target_positions[i] = ball_positions[i] - dir_i * new_target_distance;

                            glm::vec2 dir_j = glm::normalize(ball_target_positions[j] - ball_positions[j]);
                            ball_target_positions[j] = ball_positions[j] - dir_j * new_target_distance;

                            ball_target_positions[i].x = glm::clamp(ball_target_positions[i].x, x, width);
                            ball_target_positions[i].y = glm::clamp(ball_target_positions[i].y, y, height);
                        }
                    }
                }
            }


            glm::vec2 left_to_target = ball_target_positions[i] - ball_positions[i];
            const float distance_to_target = glm::length(left_to_target);

            if (distance_to_target > 0.001f)
            {
                glm::vec2 desired_velocity = glm::normalize(left_to_target) * speed;

                constexpr float acceleration = 0.33f;
                const float max_speed = speed * 1.0f;

                ball_velocities[i] += (desired_velocity - ball_velocities[i]) * acceleration;

                if (glm::length(ball_velocities[i]) > max_speed)
                {
                    ball_velocities[i] = glm::normalize(ball_velocities[i]) * max_speed;
                }
            }

            ball_positions[i] += ball_velocities[i] * time;

            if (distance_to_target <= speed * time)
            {
                ball_positions[i] = ball_target_positions[i];
                ball_target_positions[i] = glm::vec2(dist_x(random), dist_y(random));
            }
        }
    }

    void background::draw_quad()
    {
        glBindVertexArray(blur_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void background::draw(int width, int height)
    {
        GLint original_fbo;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &original_fbo);

        fbo_1.setup(width, height);
        fbo_2.setup(width, height);

        fbo_1.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();
        fbo_1.unbind();

        glUseProgram(blur_program);

        if (blur_need_update)
        {

            glUniform1i(u_blur_texture, 0);
            glUniform1f(u_blur_radius, 20.0f);
            glUniform2f(u_blur_size, width, height);
            glUniform3f(u_blur_tint_color, 1.0f, 0.2f, 0.2f);
            glUniform1f(u_blur_tint_strength, 0.05f);
            glUniform1i(u_blur_quality, 28);
        }

        fbo_2.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_1.get_texture());
        draw_quad();
        fbo_2.unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, original_fbo);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(passthrough_program);
        glUniform1i(u_passthrough_screen_texture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_2.get_texture());
        draw_quad();
        glUseProgram(0);

        blur_need_update = false;
    }

    void background::render()
    {
        glUseProgram(point_program);

        update();

        glBindBuffer(GL_ARRAY_BUFFER, point_instanced_vbo);

        if (auto *ptr = static_cast<glm::vec2 *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)))
        {
            memcpy(ptr, ball_positions, NUM_BALLS * sizeof(glm::vec2));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (effect_need_update)
        {
            glUniform1f(u_point_ball_radius, 5.0f);
            glUniform3f(u_point_color, 1.0f, 1.0f, 1.0f);
            glUniformMatrix4fv(u_point_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());
        }

        glBindVertexArray(point_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 100);

        glBindVertexArray(0);
        glUseProgram(line_program);

        glBindBuffer(GL_ARRAY_BUFFER, line_start_vbo);
        if (auto *ptr = static_cast<glm::vec2 *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)))
        {
            memcpy(ptr, line_start_points.data(), line_start_points.size() * sizeof(glm::vec2));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        glBindBuffer(GL_ARRAY_BUFFER, line_end_vbo);
        if (auto *ptr = static_cast<glm::vec2 *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)))
        {
            memcpy(ptr, line_end_points.data(), line_end_points.size() * sizeof(glm::vec2));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        glBindBuffer(GL_ARRAY_BUFFER, line_alpha_vbo);
        if (auto *ptr = static_cast<float *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)))
        {
            memcpy(ptr, line_alphas.data(), line_alphas.size() * sizeof(float));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (effect_need_update)
        {
            glUniformMatrix4fv(u_line_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());
            glUniform1f(u_line_ball_radius, 5.0f);
            glUniform3f(u_line_color, 1.0f, 1.0f, 1.0f);
            glUniform1f(u_line_width, 2.0f);
        }

        glBindVertexArray(line_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, line_start_points.size());
        glBindVertexArray(0);
        glUseProgram(0);

        effect_need_update = false;
    }

    void background::reshape(int width, int height) const
    {
        glUseProgram(point_program);
        glUniformMatrix4fv(u_point_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());
        glUseProgram(0);

        fbo_1.resize(width, height);
        fbo_2.resize(width, height);
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

            fbo_1.destroy();
            fbo_2.destroy();
        }
    }
}
