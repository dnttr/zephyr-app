//
// Created by Damian Netter on 23/06/2025.
//

#pragma once
#include <glm/fwd.hpp>

#include "utils/glm_util.hpp"

namespace zc_app
{
    class renderer
    {
        int w_width = 800;
        int w_height = 600;

        inline static glm::mat4 matrix = glm_util::get_projection(1920.0F, 1080.0F);

    public:
        void destroy();

        void initialize() const;

        void render() const;

        void update();

        void reshape(int width, int height);

        void run();

        static glm::mat4 get_projection() { return matrix; }
    };
}
