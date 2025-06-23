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
        float virtualWidth = 1920.0F;
        float virtualHeight = 1080.0F;

        int window_width = 0;
        int window_height = 0;
    public:
        void destroy();

        void initialize();

        void render() const;

        void update();

        void reshape(int width, int height);

        void run();
    };
}
