//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace zc_app
{
    class glm_util
    {
    public:

        static glm::mat4 get_projection(const float width, const float height)
        {
            return glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
        }
    };
}
