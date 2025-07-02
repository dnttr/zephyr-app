//
// Created by Damian Netter on 01/07/2025.
//

#pragma once

namespace zc_app
{
    class scene
    {
        static scene *current;

    public:
        virtual void initialize(int width, int height) = 0;

        virtual void render() = 0;

        virtual void update() = 0;

        virtual void resize(int width, int height) = 0;

        virtual void destroy() = 0;
    };
}