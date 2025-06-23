//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

namespace zc_app
{
    class renderer
    {
        int w_width = 0;
        int w_height = 0;
    public:
        void destroy();

        void initialize() const;

        void render();

        void update();

        void reshape(int width, int height);

        void run();
    };
}
