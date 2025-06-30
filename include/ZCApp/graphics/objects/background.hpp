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
        GLuint point_vbo{}, point_instanced_vbo{}, line_vbo{};
        GLuint point_vao{}, line_vao{};
        GLuint point_ebo{}, line_ebo{};

        GLuint point_program{}, line_program{};

        GLint u_point_projection{};
        GLint u_point_ball_radius{};
        GLint u_point_color{};

        bool need_update = true;

        std::pair<float, float> evd{};
        std::uniform_real_distribution<float> dist_x{}, dist_y{};

        std::random_device rd;
        std::mt19937 random;

        std::vector<std::vector<int>> grid;

        const float searchRadiusSquared = 20.0f;
        const float searchRadius = 10.0f;
        const float speed = 75.0f;
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
