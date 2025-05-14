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

    void w_allocate(const char *title, int x, int y, int width, int height);

    bool w_closing(void);

    void w_on_close(void (*callback)(void));

    void w_close(void);

#ifdef __cplusplus
} // extern "C"
#endif
