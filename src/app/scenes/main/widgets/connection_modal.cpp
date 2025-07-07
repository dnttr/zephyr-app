//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/scenes/main/widgets/connection_modal.hpp"

#include "ZCApp/app/scenes/utils.hpp"

void zc_app::connection_modal::set_error_message(const std::string &message)
{
    error_text.set_text(message);

    if (!message.empty())
    {
        const float error_char_height = SDF_BASE_SIZE * 0.05f;
        const float estimated_error_width = message.length() * error_char_height * FONT_ASPECT_RATIO;
        const float modal_x = modal_panel.get_container().get_x();
        const float new_error_x = modal_x + (MODAL_WIDTH - estimated_error_width) / 2.0f;
        const float current_error_y = error_text.get_text_position().second;

        error_text.set_properties_position(new_error_x, current_error_y);
    }
}

void zc_app::connection_modal::execute_connect()
{
    if (is_connecting)
    {
        return;
    }

    const auto username = username_input.get_text();
    const auto ip = ip_address_input.get_text();
    const auto port = port_input.get_text();

    if (!username.empty() && !validation::is_valid_username(username))
    {
        set_error_message("Invalid username (use A-Z, 0-9, _, -)");
        return;
    }
    if (!validation::is_valid_ip_address(ip))
    {
        set_error_message("Invalid IP address format");
        return;
    }
    if (!validation::is_valid_port(port))
    {
        set_error_message("Port must be a number (1-65535)");
        return;
    }

    set_error_message("Connecting...");

    const float x = connect_button_text.get_text_position().first;

    connect_button_text.set_x(x - PADDING);

    is_connecting = true;

    username_input.set_active(false);
    ip_address_input.set_active(false);
    port_input.set_active(false);

    if (on_connect_callback)
    {
        on_connect_callback(username, ip, port);
    }
}

void zc_app::connection_modal::on_connection_result(bool success, const std::string &message)
{
    is_connecting = false;

    if (success)
    {
        set_visible(false);
    }
    else
    {
        set_error_message("Failed to connect: " + message);
        connect_button_text.set_text("Connect");
        username_input.set_active(true);
        ip_address_input.set_active(true);
        port_input.set_active(true);
    }
}

std::string zc_app::connection_modal::get_username() const
{ return username_input.get_text(); }

void zc_app::connection_modal::initialize(int scene_w, int scene_h)
{
    overlay_background.set_container(container(0, 0, static_cast<float>(scene_w), static_cast<float>(scene_h)));

    const float modal_x = (static_cast<float>(scene_w) - MODAL_WIDTH) / 2.0f;
    const float modal_y = (static_cast<float>(scene_h) - MODAL_HEIGHT) / 2.0f;
    modal_panel.set_container(container(modal_x, modal_y, MODAL_WIDTH, MODAL_HEIGHT));

    text_style title_style;
    title_style.text_size_magnification = 0.15F;
    title_style.text_color = colour(255, 255, 255, 255);
    title_style.shadow_enable = true;
    title_style.shadow_offset = {1.0F, 1.0F};
    title_style.shadow_color = colour(0, 0, 0, 80);

    const std::string title_string = "Connect to Server";
    const float title_char_height = SDF_BASE_SIZE * title_style.text_size_magnification;
    const float estimated_title_width = title_string.length() * title_char_height * FONT_ASPECT_RATIO;

    const float title_x = modal_x + (MODAL_WIDTH - estimated_title_width) / 2.0f;
    const float title_y = modal_y + PADDING * 2.5;

    modal_title_text.initialize(title_string, container(title_x, title_y),
                                font_manager::get_font("Roboto-Medium"), title_style);

    const float input_width = MODAL_WIDTH - PADDING * 4;
    const float input_start_x = modal_x + PADDING * 2;
    float current_y = modal_y + 80.0f;
    text_style input_common_style;
    input_common_style.text_color = colour(255, 255, 255, 255);

    username_input.initialize(input_start_x, current_y, input_width, INPUT_HEIGHT, "Username (Optional)",
                              &font_manager::get_font("Roboto-Regular"), input_common_style);
    username_input.set_char_validator([](unsigned int c) { return std::isalnum(c) || c == '_' || c == '-'; });

    current_y += INPUT_HEIGHT + PADDING;
    ip_address_input.initialize(input_start_x, current_y, input_width, INPUT_HEIGHT, "IP Address",
                                &font_manager::get_font("Roboto-Regular"), input_common_style);
    ip_address_input.set_char_validator([](unsigned int c) { return std::isdigit(c) || c == '.'; });

    current_y += INPUT_HEIGHT + PADDING;
    port_input.initialize(input_start_x, current_y, input_width, INPUT_HEIGHT, "Port",
                          &font_manager::get_font("Roboto-Regular"), input_common_style);
    port_input.set_char_validator(&isdigit);

    const float button_x = modal_x + (MODAL_WIDTH - BUTTON_WIDTH) / 2.0f;
    const float button_y = modal_y + MODAL_HEIGHT - BUTTON_HEIGHT - PADDING * 2.5;
    connect_button.set_container(container(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT));

    text_style button_text_style = title_style;
    button_text_style.text_size_magnification = 0.09F;

    const std::string button_string = "Connect";
    const float button_char_height = SDF_BASE_SIZE * button_text_style.text_size_magnification;
    const float estimated_button_text_width = button_string.length() * button_char_height * FONT_ASPECT_RATIO;
    const float btn_text_x = button_x + (BUTTON_WIDTH - estimated_button_text_width) / 2.0f;
    const float btn_text_y = button_y + (BUTTON_HEIGHT - button_char_height) / 2.0f;

    connect_button_text.initialize(button_string, container(btn_text_x, btn_text_y),
                                   font_manager::get_font("Roboto-Medium"), button_text_style);

    text_style error_style;
    error_style.text_size_magnification = 0.05F;
    error_style.text_color = colour(255, 255, 255, 255);
    error_style.shadow_enable = true;

    const float error_y = button_y + BUTTON_HEIGHT + PADDING;
    error_text.initialize("", {modal_x + PADDING * 2, error_y}, font_manager::get_font("Roboto-Regular"),
                          error_style);
}

void zc_app::connection_modal::set_visible(bool visible)
{
    is_visible = visible;
    if (is_visible)
    {
        is_connecting = false;

        set_error_message("");
        username_input.set_active(true);
        ip_address_input.set_active(true);
        port_input.set_active(true);
    }
    else
    {
        username_input.set_active(false);
        ip_address_input.set_active(false);
        port_input.set_active(false);
    }
}

bool zc_app::connection_modal::is_modal_visible() const
{ return is_visible; }

void zc_app::connection_modal::set_on_connect_callback(
    std::function<void(const std::string &, const std::string &, const std::string &)> callback)
{
    on_connect_callback = std::move(callback);
}

void zc_app::connection_modal::update(float delta_time)
{
    if (!is_visible) return;
    for (auto *input_field : {&username_input, &ip_address_input, &port_input})
    {
        input_field->update(delta_time);
    }
}

void zc_app::connection_modal::draw()
{
    if (!is_visible) return;

    overlay_background.draw();
    modal_panel.draw();

    modal_title_text.render();

    username_input.draw();
    ip_address_input.draw();
    port_input.draw();


    if (is_connecting)
    {
        connect_button_text.set_text("Connecting");
    }
    else
    {
        connect_button_text.set_text("Connect");
    }

    connect_button.set_colour(is_button_hovered && !is_connecting ? button_hover_color : button_original_color);
    connect_button.draw();
    connect_button_text.render();
    error_text.render();
}

void zc_app::connection_modal::on_char_typed(unsigned int char_code)
{
    if (!is_visible || is_connecting) return;

    if (username_input.is_active_field())
    {
        username_input.handle_char_typed(char_code);
    }
    else if (ip_address_input.is_active_field())
    {
        ip_address_input.handle_char_typed(char_code);
    }
    else if (port_input.is_active_field())
    {
        port_input.handle_char_typed(char_code);
    }
}

void zc_app::connection_modal::on_mouse_down(const zcg_mouse_pos_t &mouse_pos, int button)
{
    if (!is_visible || button != ZCG_MOUSE_BUTTON_LEFT || is_connecting) return;

    if (is_in_area(connect_button.get_container(), mouse_pos))
    {
        execute_connect();
        return;
    }

    username_input.handle_mouse_down(mouse_pos);
    ip_address_input.handle_mouse_down(mouse_pos);
    port_input.handle_mouse_down(mouse_pos);
}

void zc_app::connection_modal::on_key_down(const zcg_key_event_t &key_event)
{
    if (!is_visible || !key_event.is_pressed || is_connecting) return;

    if (key_event.key_code == ZCG_KEY_ENTER)
    {
        execute_connect();
        return;
    }

    if (key_event.key_code == ZCG_KEY_TAB)
    {
        if (username_input.is_active_field())
        {
            username_input.set_active(false);
            ip_address_input.set_active(true);
        }
        else if (ip_address_input.is_active_field())
        {
            ip_address_input.set_active(false);
            port_input.set_active(true);
        }
        else if (port_input.is_active_field())
        {
            port_input.set_active(false);
            username_input.set_active(true);
        }
        else
        {
            username_input.set_active(true);
        }
        return;
    }

    if (username_input.is_active_field()) username_input.handle_key_down(key_event);
    else if (ip_address_input.is_active_field()) ip_address_input.handle_key_down(key_event);
    else if (port_input.is_active_field()) port_input.handle_key_down(key_event);
}

void zc_app::connection_modal::on_mouse_move(const zcg_mouse_pos_t &mouse_pos)
{
    if (!is_visible) return;
    is_button_hovered = !is_connecting && is_in_area(connect_button.get_container(), mouse_pos);
}
