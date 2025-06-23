//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

namespace zc_app
{
    class renderer
    {
        int w_width = 800;
        int w_height = 600;
    public:
        void destroy();

        void initialize();

        void render();

        void update();

        void reshape(int width, int height);

        void run();
    };
}
