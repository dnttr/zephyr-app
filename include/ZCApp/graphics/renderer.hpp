//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

namespace zc_app
{
    class renderer
    {
    public:

        void destroy();

        void initialize();

        void render() const;

        void update();

        void reshape(int width, int height);

        void run();
    };
}
