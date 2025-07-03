//
//  Part of ZCG (Zephyr-Core-Graphics Kit)
//
//  Created by Damian Netter on 14/05/2025.
//

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define ZCG_MOUSE_BUTTON_LEFT   0
#define ZCG_MOUSE_BUTTON_RIGHT  1
#define ZCG_MOUSE_BUTTON_MIDDLE 2

#define ZCG_KEY_BACKSPACE     8
#define ZCG_KEY_TAB           9
#define ZCG_KEY_ENTER         13
#define ZCG_KEY_SHIFT         16
#define ZCG_KEY_CTRL          17
#define ZCG_KEY_ALT           18
#define ZCG_KEY_PAUSE         19
#define ZCG_KEY_CAPS_LOCK     20
#define ZCG_KEY_ESCAPE        27
#define ZCG_KEY_SPACE         32
#define ZCG_KEY_PAGE_UP       33
#define ZCG_KEY_PAGE_DOWN     34
#define ZCG_KEY_END           35
#define ZCG_KEY_HOME          36
#define ZCG_KEY_LEFT          37
#define ZCG_KEY_UP            38
#define ZCG_KEY_RIGHT         39
#define ZCG_KEY_DOWN          40
#define ZCG_KEY_DELETE        46
#define ZCG_KEY_0             48
#define ZCG_KEY_1             49
#define ZCG_KEY_2             50
#define ZCG_KEY_3             51
#define ZCG_KEY_4             52
#define ZCG_KEY_5             53
#define ZCG_KEY_6             54
#define ZCG_KEY_7             55
#define ZCG_KEY_8             56
#define ZCG_KEY_9             57
#define ZCG_KEY_A             65
#define ZCG_KEY_B             66
#define ZCG_KEY_C             67
#define ZCG_KEY_D             68
#define ZCG_KEY_E             69
#define ZCG_KEY_F             70
#define ZCG_KEY_G             71
#define ZCG_KEY_H             72
#define ZCG_KEY_I             73
#define ZCG_KEY_J             74
#define ZCG_KEY_K             75
#define ZCG_KEY_L             76
#define ZCG_KEY_M             77
#define ZCG_KEY_N             78
#define ZCG_KEY_O             79
#define ZCG_KEY_P             80
#define ZCG_KEY_Q             81
#define ZCG_KEY_R             82
#define ZCG_KEY_S             83
#define ZCG_KEY_T             84
#define ZCG_KEY_U             85
#define ZCG_KEY_V             86
#define ZCG_KEY_W             87
#define ZCG_KEY_X             88
#define ZCG_KEY_Y             89
#define ZCG_KEY_Z             90
#define ZCG_KEY_F1            112
#define ZCG_KEY_F2            113
#define ZCG_KEY_F3            114
#define ZCG_KEY_F4            115
#define ZCG_KEY_F5            116
#define ZCG_KEY_F6            117
#define ZCG_KEY_F7            118
#define ZCG_KEY_F8            119
#define ZCG_KEY_F9            120
#define ZCG_KEY_F10           121
#define ZCG_KEY_F11           122
#define ZCG_KEY_F12           123

typedef struct
{
    const char *title;

    int x, y;

    int width, height;

    int min_width, min_height;
    int max_width, max_height;
} zcg_window_args_t;

typedef struct
{
    void (* exit)();

    void (* resize)(int width, int height);

    bool (* is_retina)();

    void (* get_mouse_position)(float *x, float *y);

    bool (* is_mouse_in_window)();
} zcg_window_t;

typedef struct
{
    float x, y;
} zcg_mouse_pos_t;

typedef struct
{
    float delta_x, delta_y;
    float mouse_x, mouse_y;

    bool is_precise;
} zcg_scroll_event_t;

typedef struct
{
    unsigned int key_code;

    bool is_pressed;
    bool shift, ctrl, alt, cmd;
} zcg_key_event_t;

typedef struct
{
    void (* on_exit_callback)();

    void (* on_render_callback)();

    void (* on_reshape_callback)(int width, int height);

    void (* on_init_callback)();

    void (* on_update_callback)();

    void (* on_mouse_move_callback)(zcg_mouse_pos_t mouse_pos);

    void (* on_mouse_enter_callback)(zcg_mouse_pos_t mouse_pos);

    void (* on_mouse_exit_callback)(zcg_mouse_pos_t mouse_pos);

    void (* on_mouse_down_callback)(zcg_mouse_pos_t mouse_pos, int button);

    void (* on_mouse_up_callback)(zcg_mouse_pos_t mouse_pos, int button);

    void (* on_scroll_callback)(zcg_scroll_event_t scroll_event);

    void (* on_key_down_callback)(zcg_key_event_t key_event);

    void (* on_key_up_callback)(zcg_key_event_t key_event);
} zcg_callback_handle;

zcg_window_t *zcg_allocate(zcg_window_args_t *args, zcg_callback_handle *handle);

void zcg_run(const zcg_window_t *window);

#ifdef __cplusplus
} // extern "C"
#endif
