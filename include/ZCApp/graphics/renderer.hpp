//
// Created by Damian Netter on 23/06/2025.
//

#pragma once
#include "ZCGKit/zcg_interface.h"

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

        void on_mouse_up(const zcg_mouse_pos_t &mouse_pos, int button);

        void on_mouse_down(const zcg_mouse_pos_t &mouse_pos, int button);

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos);

        void on_mouse_enter(const zcg_mouse_pos_t &mouse_pos);

        void on_mouse_exit(const zcg_mouse_pos_t &mouse_pos);

        void on_scroll(const zcg_scroll_event_t &scroll_event);
    };
}
