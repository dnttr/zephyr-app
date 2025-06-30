//
// Created by Damian Netter on 30/06/2025.
//

#pragma once

#include <random>
#include <utility>
#include <OpenGL/gl3.h>

namespace zc_app
{
    class background
    {
        static std::uniform_real_distribution<float> alpha;

        GLuint point_vbo{}, point_instanced_vbo{}, line_vbo{}, line_start_vbo{}, line_end_vbo{}, line_alpha_vbo{};
        GLuint point_vao{}, line_vao{};
        GLuint point_ebo{}, line_ebo{};

        GLuint point_program{}, line_program{};

        GLint u_line_projection{};
        GLint u_line_ball_radius{};
        GLint u_line_width{};
        GLint u_line_color{};

        GLint u_point_projection{};
        GLint u_point_ball_radius{};
        GLint u_point_color{};

        bool need_update = true;

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

        void fetch_uniforms();
        void update();

    public:
        background() = default;

        void setup();

        void render();
        void reshape() const;

        ~background();
    };
}
