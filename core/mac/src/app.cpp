//
// Created by Damian Netter on 14/05/2025.
//

#include "core_graphics.h"

void cg_window_exit()
{
    // Handle window exit
}

int main(int argc, char *argv[])
{
    cg_callback_handle handle = {
        .on_exit_callback = cg_window_exit
    };
    cg_window_args_t args = {
        .title = "Core Graphics",
        .x = 0,
        .y = 0,
        .width = 800,
        .height = 600
    };

    cg_window_t *window = cg_allocate(&args, &handle);

    return 0;
}
