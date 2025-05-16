//
// Created by Damian Netter on 14/05/2025.
//

#include "cg_interface.h"

#include "OpenGL/gl3.h"

void cg_window_exit()
{

}

void cg_loop()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

int main(int argc, char *argv[])
{
    zcg_callback_handle handle = {
        .on_exit_callback = cg_window_exit,
        .on_loop_callback = cg_loop
    };
    zcg_window_args_t args = {
        .title = "Core Graphics",
        .x = 0,
        .y = 0,
        .width = 800,
        .height = 600
    };

    const zcg_window_t *window = cg_allocate(&args, &handle);

    return 0;
}
