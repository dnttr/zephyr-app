//
//  Part of ZCG (Zephyr-Core-Graphics Kit)
//
//  Created by Damian Netter on 14/05/2025.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    const char* title;

    int x, y;

    int width, height;

    int min_width, min_height;
    int max_width, max_height;
} zcg_window_args_t;

typedef struct
{
    void (*exit)();

    void (*resize)(int width, int height);

    bool (*is_retina)();
} zcg_window_t;

typedef struct
{
    void (*on_exit_callback)();

    void (*on_render_callback)();

    void (*on_reshape_callback)(int width, int height);

    void (*on_init_callback)();

    void (*on_update_callback)();
} zcg_callback_handle;

zcg_window_t* zcg_allocate(zcg_window_args_t* args, zcg_callback_handle* handle);

void zcg_run(const zcg_window_t* window);

#ifdef __cplusplus
} // extern "C"
#endif
