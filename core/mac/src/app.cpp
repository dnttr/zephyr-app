//
// Created by Damian Netter on 14/05/2025.
//

#include "core_graphics.h"

void cg_window_exit()
{

}

int main(int argc, char *argv[])
{
    zcg_callback_handle handle = {
        .on_exit_callback = cg_window_exit
    };
    zcg_window_args_t args = {
        .title = "Core Graphics",
        .x = 0,
        .y = 0,
        .width = 800,
        .height = 600
    };

    zcg_window_t *window = cg_allocate(&args, &handle);

    while (window && window->loop())
    {
    }

    return 0;
}
