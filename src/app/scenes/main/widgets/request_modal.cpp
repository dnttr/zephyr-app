//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/scenes/main/widgets/request_modal.hpp"

namespace zc_app
{
    void request_modal::initialize(const int scene_width, const int scene_height)
    {
        overlay_background.set_container(container(0, 0, static_cast<float>(scene_width), static_cast<float>(scene_height)));

        constexpr float modal_width = 350.0f;
        constexpr float modal_height = 150.0f;

        const float modal_x = (static_cast<float>(scene_width) - modal_width) / 2.0f;
        const float modal_y = (static_cast<float>(scene_height) - modal_height) / 2.0f;

        modal_panel.set_container(container(modal_x, modal_y, modal_width, modal_height));

        text_style main_style;
        main_style.text_size_magnification = 0.13F;
        main_style.text_color = colour(255, 255, 255, 255);

        const container title_text_container(modal_x + modal_width / 4 - PADDING, modal_y + modal_height / 2 - PADDING * 2.5F);
        title_text.initialize("", title_text_container, font_manager::get_font("Roboto-Medium"), main_style);

        constexpr float button_width = 120.0f;
        constexpr float button_height = 40.0f;

        const float button_y = modal_y + modal_height - button_height - PADDING * 2;

        decline_button.set_container(container(modal_x + PADDING * 2, button_y, button_width, button_height));
        accept_button.set_container(container(modal_x + modal_width - button_width - PADDING * 2, button_y,
                                              button_width, button_height));

        text_style btn_style;

        btn_style.text_size_magnification = 0.09F;
        btn_style.text_color = colour(255, 255, 255, 255);

        accept_text.initialize("Accept", container(0, 0), font_manager::get_font("Roboto-Medium"), btn_style);
        decline_text.initialize("Decline", container(0, 0), font_manager::get_font("Roboto-Medium"), btn_style);
    }

    void request_modal::show(const std::string &sender_name)
    {
        title_text.set_text("Chat request from '" + sender_name + "'");
        is_visible = true;
    }

    void request_modal::set_on_answered_callback(std::function<void(bool)> callback)
    {
        on_answered_callback = std::move(callback);
    }

    void request_modal::draw()
    {
        if (!is_visible)
        {
            return;
        }

        overlay_background.draw();
        modal_panel.draw();
        accept_button.draw();
        decline_button.draw();

        title_text.render();

        const auto &accept_cont = accept_button.get_container();

        accept_text.set_properties_position(accept_cont.get_x() + accept_cont.get_width() / 4.0F + PADDING, accept_cont.get_y() + (accept_cont.get_height() - PADDING) / 2.0F);
        accept_text.render();

        const auto &decline_cont = decline_button.get_container();

        decline_text.set_properties_position(decline_cont.get_x() + decline_cont.get_width() / 4.0F + PADDING, decline_cont.get_y() + (accept_cont.get_height() - PADDING) / 2.0F);
        decline_text.render();
    }

    void request_modal::on_mouse_down(const zcg_mouse_pos_t &mouse_pos, int button)
    {
        if (!is_visible || button != ZCG_MOUSE_BUTTON_LEFT)
        {
            return;
        }

        if (is_in_area(accept_button.get_container(), mouse_pos))
        {
            if (on_answered_callback)
            {
                on_answered_callback(true);
            }

            is_visible = false;
        }
        else if (is_in_area(decline_button.get_container(), mouse_pos))
        {
            if (on_answered_callback)
            {
                on_answered_callback(false);
            }

            is_visible = false;
        }
    }

    bool request_modal::is_modal_visible() const
    {
        return is_visible;
    }
}
