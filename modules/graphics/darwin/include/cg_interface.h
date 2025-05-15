//
//  Part of ZCG (Zephyr-Core-Graphics Kit)
//
//  Created by Damian Netter on 14/05/2025.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct cg_renderer_t *cg_renderer;

    typedef struct
    {
        const char *title;

        int x, y;

        int width, height;
    } zcg_window_args_t;

    typedef struct
    {
        cg_renderer (*create)();

        void (*exit)();

        void (*resize)(int width, int height);

        void (*initialize)(cg_renderer, int width, int height);

        bool (*loop)();
    } zcg_window_t;

    typedef struct
    {
        void (*on_exit_callback)();
    } zcg_callback_handle;

    zcg_window_t *cg_allocate(zcg_window_args_t *args, zcg_callback_handle *handle);


#ifdef __cplusplus
} // extern "C"
#endif
