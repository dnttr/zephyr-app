//
// Created by Damian Netter on 24/06/2025.
//

#pragma once

#include <glm/glm.hpp>

namespace zc_app
{
    class colour
    {
        float red, green, blue, alpha;

    public:

        colour() : red(0.0F), green(0.0F), blue(0.0F), alpha(0.0F) {}

        explicit constexpr colour(const uint8_t red, const uint8_t green, const uint8_t blue,
                    const uint8_t alpha = 255)
               : red(static_cast<float>(red) / 255.0F), green(static_cast<float>(green) / 255.0F), blue(static_cast<float>(blue) / 255.0F), alpha(static_cast<float>(alpha) / 255.0F)
           {
           }

        void set_red_u8(const uint8_t r)
        {
            red = static_cast<float>(r) / 255.0F;
        }

        void set_green_u8(const uint8_t g)
        {
            green = static_cast<float>(g) / 255.0F;
        }

        void set_blue_u8(const uint8_t b)
        {
            blue = static_cast<float>(b) / 255.0F;
        }

        void set_alpha_u8(const uint8_t a)
        {
            alpha = static_cast<float>(a) / 255.0F;
        }

        void set_red_direct(const float r)
        {
            red = r;
        }

        void set_green_direct(const float g)
        {
            green = g;
        }

        void set_blue_direct(const float b)
        {
            blue = b;
        }

        void set_alpha_direct(const float a)
        {
            alpha = a;
        }

        [[nodiscard]] uint32_t get_rgba_u32() const
        {
            return static_cast<uint32_t>(red * 255.0F) << 24 |
                   static_cast<uint32_t>(green * 255.0F) << 16 |
                   static_cast<uint32_t>(blue * 255.0F) << 8 |
                   static_cast<uint32_t>(alpha * 255.0F);
        }

        [[nodiscard]] uint8_t get_red_u8() const
        {
            return static_cast<uint8_t>(red * 255.0F);
        }

        [[nodiscard]] uint8_t get_green_u8() const
        {
            return static_cast<uint8_t>(green * 255.0F);
        }

        [[nodiscard]] uint8_t get_blue_u8() const
        {
            return static_cast<uint8_t>(blue * 255.0F);
        }

        [[nodiscard]] uint8_t get_alpha_u8() const
        {
            return static_cast<uint8_t>(alpha * 255.0F);
        }

        [[nodiscard]] float get_red_direct() const
        {
            return red;
        }

        [[nodiscard]] float get_green_direct() const
        {
            return green;
        }

        [[nodiscard]] float get_blue_direct() const
        {
            return blue;
        }

        [[nodiscard]] float get_alpha_direct() const
        {
            return alpha;
        }

        [[nodiscard]] glm::vec4 get_vec4() const
        {
            return {red, green, blue, alpha};
        }

        [[nodiscard]] glm::vec3 get_vec3() const
        {
            return {red, green, blue};
        }

        void set_colours(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
        {
            set_red_u8(r);
            set_green_u8(g);
            set_blue_u8(b);
            set_alpha_u8(a);
        }

        [[nodiscard]] bool operator==(const colour &other) const
        {
            return red == other.red && green == other.green && blue == other.blue && alpha == other.alpha;
        }

        [[nodiscard]] bool operator!=(const colour &other) const
        {
            return !(*this == other);
        }
    };
}
