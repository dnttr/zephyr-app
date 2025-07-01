//
// Created by Damian Netter on 30/06/2025.
//

#include "ZCApp/graphics/objects/background.hpp"

#include <fstream>
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

    framebuffer fbo;

    std::uniform_real_distribution<float> background::alpha = std::uniform_real_distribution(0.1F, 1.0F);

    fullscreen_blur background::blur;

    void background::fetch_uniforms()
    {
        u_point_projection = glGetUniformLocation(point_program, "projection_matrix");
        u_point_ball_radius = glGetUniformLocation(point_program, "ball_radius");
        u_point_color = glGetUniformLocation(point_program, "ball_color");

        u_line_projection = glGetUniformLocation(line_program, "projection_matrix");
        u_line_ball_radius = glGetUniformLocation(line_program, "ball_radius");
        u_line_width = glGetUniformLocation(line_program, "line_width_pixels");
        u_line_color = glGetUniformLocation(line_program, "line_color");
    }

    void background::setup()
    {
        point_program = shaders::create_program("effect_vert", "effect_frag");
        line_program = shaders::create_program("line_vert", "line_frag");

        fetch_uniforms();

        glGenBuffers(1, &point_vbo);
        glGenBuffers(1, &point_instanced_vbo);
        glGenBuffers(1, &point_ebo);

        glGenBuffers(1, &line_vbo);
        glGenBuffers(1, &line_ebo);

        glGenVertexArrays(1, &line_vao);
        glGenVertexArrays(1, &point_vao);

        constexpr float vertices[]
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

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        constexpr int max_possible_lines = (NUM_BALLS * NUM_BALLS) / 2;

        for (auto &line_buffer : line_buffers)
        {
            line_buffer.start_points.reserve(max_possible_lines);
            line_buffer.end_points.reserve(max_possible_lines);
            line_buffer.alphas.reserve(max_possible_lines);
            line_buffer.count = 0;

            glGenBuffers(1, &line_buffer.start_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, line_buffer.start_vbo);
            glBufferData(GL_ARRAY_BUFFER, max_possible_lines * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

            glGenBuffers(1, &line_buffer.end_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, line_buffer.end_vbo);
            glBufferData(GL_ARRAY_BUFFER, max_possible_lines * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

            glGenBuffers(1, &line_buffer.alpha_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, line_buffer.alpha_vbo);
            glBufferData(GL_ARRAY_BUFFER, max_possible_lines * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        }

        this->blur.setup(width, height);

        physics_running = true;
        physics_thread = std::thread(&background::physics_thread_function, this);
    }

    void background::physics_thread_function()
    {
        while (physics_running)
        {
            const int current = physics_buffer.load();

            line_buffers[current].start_points.clear();
            line_buffers[current].end_points.clear();
            line_buffers[current].alphas.clear();

            update(current);

            const int next_buffer = (current + 1) % 3;

            int wait_attempts = 0;
            constexpr int MAX_WAIT_ATTEMPTS = 100;

            while (next_buffer == render_buffer.load() && wait_attempts < MAX_WAIT_ATTEMPTS)
            {
                std::this_thread::yield();
                wait_attempts++;

                if (wait_attempts % 10 == 0)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }

            physics_buffer.store(next_buffer);

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void background::update(const int buffer_idx)
    {
        auto &buffer = line_buffers[buffer_idx];

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

        const float width = evd.first;
        const float height = evd.second;

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

                        nearby_update(ball_positions[i], ball_positions[j], i, j, width, height, buffer);
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

        buffer.count = buffer.start_points.size();
    }

    /**
     * @param b_1 is ball_positions[i]
     * @param b_2 is ball_positions[j]
     */
    void background::nearby_update(glm::vec2 &b_1, glm::vec2 &b_2, int i, int j, float width, float height, line_buffer_set &buffer)
    {
        const glm::vec2 delta = b_1 - b_2;
        const float distance_squared = glm::dot(delta, delta);
        const float search_radius_squared = searchRadius * searchRadius;
        const float line_radius_squared = searchRadius * 5 * searchRadius * 5;

        if (i < j)
        {
            if (distance_squared > 0.0f && distance_squared < line_radius_squared)
            {
                const float distance = sqrt(distance_squared);

                buffer.start_points.push_back(b_1);
                buffer.end_points.push_back(b_2);

                const float normalized_distance = distance / (searchRadius * 5);
                float distance_alpha = 1.0f - normalized_distance;
                distance_alpha = std::clamp(distance_alpha * 0.9f + 0.1f, 0.1f, 1.0f);

                buffer.alphas.push_back(distance_alpha);
            }
        }

        if (distance_squared > 0.0f && distance_squared < search_radius_squared)
        {
            const float distance = sqrt(distance_squared);

            const glm::vec2 normal = delta / distance;
            const float overlap = searchRadius - distance;

            b_1 += normal * overlap * 0.5f;
            b_2 -= normal * overlap * 0.5f;

            auto &b_1_velocity = ball_velocities[i];
            auto &b_2_velocity = ball_velocities[j];

            auto &b_1_target = ball_target_positions[i];
            auto &b_2_target = ball_target_positions[j];

            const float v1 = glm::dot(b_1_velocity, normal);
            const float v2 = glm::dot(b_2_velocity, normal);

            constexpr float m1 = 1.0F;
            constexpr float m2 = 1.0F;

            const float p = 2.0F * (v1 - v2) / (m1 + m2);

            b_1_velocity = b_1_velocity - p * m2 * normal;
            b_2_velocity = b_2_velocity + p * m1 * normal;

            constexpr float collisionSpeedBoost = 2.0f;

            b_1_velocity += normal * collisionSpeedBoost;
            b_2_velocity -= normal * collisionSpeedBoost;

            constexpr float new_target_distance = 100.0f;

            constexpr float x = 0.0f;
            constexpr float y = 0.0f;

            const glm::vec2 dir_i = glm::normalize(b_1_target - b_1);
            b_1_target = b_1 - dir_i * new_target_distance;

            const glm::vec2 dir_j = glm::normalize(b_2_target - b_2);
            b_2_target = b_2 - dir_j * new_target_distance;

            b_1_target.x = glm::clamp(b_1_target.x, x, width);
            b_1_target.y = glm::clamp(b_1_target.y, y, height);
        }
    }

    void background::draw(const int width, const int height)
    {
        blur.draw(width, height, [this]
                {
                    render();
                });
    }

    void background::setup_buffers(const line_buffer_set &lbs)
    {
        const auto lb_start_points_size = GL_SIZEI_PTR(lbs.start_points.size());
        const auto lb_end_points_size = GL_SIZEI_PTR(lbs.end_points.size());
        const auto lb_alphas_size = GL_SIZEI_PTR(lbs.alphas.size());

        constexpr auto float_size = GL_SIZEI_PTR(sizeof(float));
        constexpr auto vec_size = GL_SIZEI_PTR(sizeof(glm::vec2));

        glBindBuffer(GL_ARRAY_BUFFER, lbs.start_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lb_start_points_size * vec_size, lbs.start_points.data());

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        glVertexAttribDivisor(1, 1);

        glBindBuffer(GL_ARRAY_BUFFER, lbs.end_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lb_end_points_size * vec_size, lbs.end_points.data());

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        glVertexAttribDivisor(2, 1);

        glBindBuffer(GL_ARRAY_BUFFER, lbs.alpha_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lb_alphas_size * float_size, lbs.alphas.data());

        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(3);

        glVertexAttribDivisor(3, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void background::render()
    {
        glUseProgram(point_program);

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

        const int current = render_buffer.load();

        if (const int next_render = (current + 1) % 3; next_render != physics_buffer.load())
        {
            render_buffer.store(next_render);
        }

        glBindVertexArray(line_vao);

        const auto &current_buffer = line_buffers[current];
        setup_buffers(current_buffer);

        if (effect_need_update)
        {
            glUniformMatrix4fv(u_line_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());

            glUniform3f(u_line_color, 1.0f, 1.0f, 1.0f);

            glUniform1f(u_line_ball_radius, 5.0f);
            glUniform1f(u_line_width, 2.0f);
        }

        const auto lb_count = GL_SIZEI(current_buffer.count);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, lb_count);

        glBindVertexArray(0);
        glUseProgram(0);

        effect_need_update = false;
    }

    void background::reshape(const int width, const int height) const
    {
        glUseProgram(point_program);
        glUniformMatrix4fv(u_point_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());
        glUseProgram(0);

        blur.reshape(width, height);
    }

    background::~background()
    {
        if (physics_running)
        {
            physics_running = false;

            if (physics_thread.joinable())
            {
                physics_thread.join();
            }
        }

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