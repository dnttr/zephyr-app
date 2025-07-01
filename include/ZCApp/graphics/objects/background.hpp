//
// Created by Damian Netter on 30/06/2025.
//

#pragma once

#include <atomic>
#include <mutex>
#include <random>
#include <thread>
#include <utility>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>

namespace zc_app
{
    class background
    {
        static std::uniform_real_distribution<float> alpha;

        static constexpr int NUM_BALLS = 150;

        glm::vec2 ball_target_positions[NUM_BALLS];
        glm::vec2 ball_positions[NUM_BALLS];
        glm::vec2 ball_velocities[NUM_BALLS];

        std::thread physics_thread;
        std::atomic<bool> physics_running{false};
        std::mutex data_mutex;


        struct line_buffer_set {
            GLuint start_vbo;
            GLuint end_vbo;
            GLuint alpha_vbo;

            glm::vec2* start_ptr;
            glm::vec2* end_ptr;
            float* alpha_ptr;

            std::vector<glm::vec2> start_points;
            std::vector<glm::vec2> end_points;
            std::vector<float> alphas;

            size_t count;
        };

        line_buffer_set line_buffers[3];

        GLuint point_vbo{}, point_instanced_vbo{}, line_vbo{},
               blur_vbo{};
        GLuint point_vao{}, line_vao{}, blur_vao{};
        GLuint point_ebo{}, line_ebo{}, blur_ebo{};

        GLuint point_program{}, line_program{}, blur_program{}, passthrough_program{};

        GLint u_line_projection{};
        GLint u_line_ball_radius{};
        GLint u_line_width{};
        GLint u_line_color{};

        GLint u_point_projection{};
        GLint u_point_ball_radius{};
        GLint u_point_color{};

        GLint u_blur_projection{};
        GLint u_blur_radius{};
        GLint u_blur_texture{};
        GLint u_blur_direction{};
        GLint u_blur_size{};
        GLint u_blur_quality{};
        GLint u_blur_tint_strength{};
        GLint u_blur_tint_color{};

        GLint u_passthrough_screen_texture{};

        bool effect_need_update = true;
        bool blur_need_update = true;

        std::pair<float, float> evd{};
        std::vector<std::vector<int>> grid;

        std::uniform_real_distribution<float> dist_x{}, dist_y{};

        std::random_device rd;
        std::mt19937 random;

        const float searchRadiusSquared = 20.0f;
        const float searchRadius = 10.0f;
        const float speed = 50.0f;
        const int cellSize = 30;

        int cells_count_x{};
        int cells_count_y{};

        void physics_thread_function();

        void fetch_uniforms();

        void update();

        void draw_quad();

    public:
        background() = default;

        void setup();
        void draw(int width, int height);
        void render();
        void reshape(int width, int height) const;

        ~background();
    };
}
