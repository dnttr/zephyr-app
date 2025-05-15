//
//  libcoregraphics.h
//  libcoregraphics
//
//  Created by Damian Netter on 14/05/2025.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct
    {
        const char *title;

        int x, y;

        int width, height;
    } cg_window_args_t;

    typedef struct
    {
        void (*exit)();

        void (*resize)(int width, int height);

        bool (*loop)();
    } cg_window_t;

    typedef struct
    {
        void (*on_exit_callback)();
    } cg_callback_handle;

    cg_window_t *cg_allocate(cg_window_args_t *args, cg_callback_handle *handle);


#ifdef __cplusplus
} // extern "C"
#endif
