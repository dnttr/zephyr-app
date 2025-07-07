//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/scenes/main/widgets/connection_list.hpp"

#include "ZCApp/graphics/utils/math_util.hpp"
#include "ZCApp/graphics/utils/scissor.hpp"

namespace zc_app
{
    void connection_list::create_connections_list_uic(const float begin_y, const float sidebar_width,
        const float sidebar_height)
    {
        const float start_y = begin_y;

        connections_container.set_x(PADDING * 2);
        connections_container.set_y(start_y);
        connections_container.set_width(sidebar_width - PADDING * 4);
        connections_container.set_height(sidebar_height - start_y);
    }

    void connection_list::initialize(const float begin_y, const float sidebar_width, const float sidebar_height)
    {
        create_connections_list_uic(begin_y, sidebar_width, sidebar_height);
    }

    void connection_list::set_on_connection_selected_callback(std::function<void(const std::string &)> callback)
    {
        on_connection_selected_callback = std::move(callback);
    }

    void connection_list::populate_connections(const std::vector<connection_data> &new_connections)
    {
        connections_list.clear();

        float current_item_relative_connections = 0.0f;

        for (const auto&[name, status] : new_connections)
        {
            std::string status_string = "Offline";
            switch (status) {
            case 1: status_string = "Online"; break;
            case 2: status_string = "In Chat"; break;
            default: throw std::runtime_error("Unknown status code: " + std::to_string(status));
            }

            auto new_button = std::make_unique<connection_button>(
                name,
                name,
                status_string,
                "avatar.png"
            );

            container button_container(
                connections_container.get_x(),
                connections_container.get_y() + current_item_relative_connections,
                connections_container.get_width(),
                default_item_height
            );

            new_button->setup(button_container);
            new_button->set_on_click_callback([this](const std::string& id) {
                if(on_connection_selected_callback)
                {
                    on_connection_selected_callback(id);
                }
            });

            connections_list.push_back(std::move(new_button));
            current_item_relative_connections += default_item_height + default_item_spacing;
        }
    }

    void connection_list::update(const float delta_time)
    {
        if (!is_scrolling_smoothly)
        {
            return;
        }

        scroll_animation_timer += delta_time;
        const float t_norm = scroll_animation_timer / scroll_animation_duration;

        if (t_norm >= 1.0f)
        {
            current_scroll_offset_y = target_scroll_offset_y;
            is_scrolling_smoothly = false;
            return;
        }

        const float eased_progress = math_util::ease_out(t_norm);

        current_scroll_offset_y = scroll_start_offset_y + (target_scroll_offset_y - scroll_start_offset_y) * eased_progress;

        const float content_height = get_total_content_height();
        const float viewport_height = connections_container.get_height();

        constexpr float scroll_boundary_top = 0.0f;
        const float scroll_boundary_bottom = content_height <= viewport_height ? 0.0f : -(content_height - viewport_height);

        current_scroll_offset_y = std::max(current_scroll_offset_y, scroll_boundary_bottom);
        current_scroll_offset_y = std::min(current_scroll_offset_y, scroll_boundary_top);
    }

    void connection_list::draw()
    {
        scissor::glScissorOp([this]
        {
            for (const auto &button_ptr : connections_list)
            {
                container current_button_container_for_draw = button_ptr->get_container();

                current_button_container_for_draw.set_y(current_button_container_for_draw.get_y() + current_scroll_offset_y);

                if (current_button_container_for_draw.get_y() + current_button_container_for_draw.get_height() >= connections_container.get_y() && current_button_container_for_draw.get_y() <= connections_container.get_y() + connections_container.get_height())
                {
                    container original_button_shape_container = button_ptr->button_shape.get_container();
                    button_ptr->button_shape.set_container(current_button_container_for_draw);
                    button_ptr->render();
                    button_ptr->button_shape.set_container(original_button_shape_container);
                }
            }
        }, connections_container, {});

        const float content_height = get_total_content_height();

        if (const float viewport_height = connections_container.get_height(); content_height > viewport_height)
        {
            scrollbar_track_rect.set_container(container(
                connections_container.get_x() + connections_container.get_width() + 2,
                connections_container.get_y(),
                SCROLLBAR_WIDTH,
                connections_container.get_height()
            ));

            scrollbar_track_rect.draw();

            const float thumb_proportional_height = viewport_height / content_height;
            float thumb_height = viewport_height * thumb_proportional_height;

            thumb_height = std::max(thumb_height, min_thumb_height);

            const float max_scroll_y = get_max_scroll_y();
            const float normalized_scroll = max_scroll_y != 0 ? current_scroll_offset_y / max_scroll_y : 0;

            const float thumb_movable_range_in_track = connections_container.get_height() - thumb_height;
            const float thumb_y_offset_in_track = normalized_scroll * thumb_movable_range_in_track;

            scrollbar_thumb_rect.set_container(container(
                scrollbar_track_rect.get_container().get_x(),
                scrollbar_track_rect.get_container().get_y() + thumb_y_offset_in_track,
                scrollbar_track_rect.get_container().get_width(),
                thumb_height
            ));

            scrollbar_thumb_rect.draw();
        }
    }

    float connection_list::get_total_content_height() const
    {
        if (connections_list.empty())
        {
            return 0.0F;
        }

        const auto connections_size = static_cast<float>(connections_list.size());

        return connections_size * default_item_height +
            (connections_size > 0 ? (connections_size - 1) * default_item_spacing : 0.0F);
    }

    float connection_list::get_max_scroll_y() const
    {
        const float viewport_height = connections_container.get_height();
        const float content_height = get_total_content_height();

        if (content_height <= viewport_height)
        {
            return 0.0F;
        }

        return -(content_height - viewport_height);
    }

    float connection_list::get_min_scroll_y() const
    {
        return 0.0f;
    }

    void connection_list::on_mouse_down(const zcg_mouse_pos_t &mouse_pos_physical, int button) const
    {
        if (!is_in_area(connections_container, mouse_pos_physical))
        {
            return;
        }

        for (auto &btn_ptr : connections_list)
        {
            btn_ptr->on_mouse_down(mouse_pos_physical, button);
        }
    }

    void connection_list::on_mouse_move(const zcg_mouse_pos_t &mouse_pos_physical) const
    {
        for (auto &btn_ptr : connections_list)
        {
            btn_ptr->on_mouse_move(mouse_pos_physical);
        }
    }

    void connection_list::scroll(const zcg_scroll_event_t &scroll_event)
    {
        if (!is_in_area(connections_container, scroll_event.mouse_x, scroll_event.mouse_y))
        {
            return;
        }

        constexpr float max_scroll_offset = 0.0f;

        const float scroll_amount_virtual = scroll_event.delta_y * scroll_sensitivity_multiplier;
        const float immediate_desired_offset_y = current_scroll_offset_y + scroll_amount_virtual;

        const float min_scroll_offset = get_max_scroll_y();

        float clamped_target_y = std::max(immediate_desired_offset_y, min_scroll_offset);
        clamped_target_y = std::min(clamped_target_y, max_scroll_offset);

        if (std::abs(clamped_target_y - current_scroll_offset_y) < 0.1F && !is_scrolling_smoothly)
        {
            return;
        }

        target_scroll_offset_y = clamped_target_y;
        scroll_start_offset_y = current_scroll_offset_y;
        scroll_animation_timer = 0.0F;

        is_scrolling_smoothly = true;
    }
}
