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
        virtual ~scene() = default;

        virtual void initialize(int width, int height) = 0;

        virtual void render() = 0;

        virtual void update() = 0;

        virtual void resize(int width, int height) = 0;

        virtual void destroy() = 0;

        virtual void scroll(const zcg_scroll_event_t &scroll_event) = 0;

        virtual void on_mouse_up(zcg_mouse_pos_t mouse_pos, int button) = 0;

        virtual void on_mouse_move(const zcg_mouse_pos_t &mouse_pos) = 0;

        virtual void on_mouse_down(zcg_mouse_pos_t mouse_pos, int button) = 0;

        virtual void on_key_down(zcg_key_event_t key_event) = 0;

        virtual void on_char_typed(unsigned int char_code) = 0;
    };
}
