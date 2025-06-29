//
// Created by Damian Netter on 25/06/2025.
//
#pragma once

#include <cmath>
#include <vector>

namespace zc_app
{
    class circle_util
    {
    public:
        static std::vector<float> generate_circle_vertices(const float radius, const int segments)
        {
            std::vector<float> vertices;

            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.5f);
            vertices.push_back(0.5f);

            for (int i = 0; i <= segments; ++i) {
                const float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * M_PI;

                float x = radius * std::cos(angle);
                float y = radius * std::sin(angle);

                float u = 0.5f + 0.5f * std::cos(angle);
                float v = 0.5f + 0.5f * std::sin(angle);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(0.0f);
                vertices.push_back(u);
                vertices.push_back(v);
            }

            return vertices;
        }
    };
}