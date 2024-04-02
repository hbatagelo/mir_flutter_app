#include "decorated_window.h"
#include "globals.h"
#include "mir_window.h"

#include <cairo.h>

#include <cassert>
#include <numbers>
#include <string>

namespace mfa = mir_flutter_app;

mfa::DecoratedWindow::DecoratedWindow(wl_surface* surface, int32_t width, int32_t height, Configuration config) :
    ToplevelWindow(surface, width, height),
    config_{std::move(config)}
{
}

void mfa::DecoratedWindow::draw_new_content(Buffer* buffer)
{
    auto* mir_window{Globals::instance().window_for(static_cast<wl_surface*>(*this))};
    assert(mir_window);

    auto const x{config_.stroke_width / 2};
    auto const y{config_.stroke_width / 2};
    auto const width{buffer->width - (x * 2)};
    [[maybe_unused]] auto const height{buffer->height - (y * 2)};

    auto const pi{std::numbers::pi};
    auto const pi_2{pi / 2.0};

    cairo_set_source_rgba(buffer->cairo_context, 0, 0, 0, 0);
    cairo_paint(buffer->cairo_context);

    // Title bar
    {
        // Background
        auto const i{std::max(config_.title_bar_intensity - current_intensity_offset, 0.0)};
        cairo_set_source_rgba(buffer->cairo_context, i, i, i, alpha);
        cairo_set_line_width(buffer->cairo_context, config_.stroke_width);

        cairo_new_sub_path(buffer->cairo_context);
        cairo_arc(buffer->cairo_context, x, y + config_.title_bar_height, 0, 0, 0);
        cairo_arc(buffer->cairo_context, x + config_.title_bar_corner_radius, y + config_.title_bar_corner_radius, config_.title_bar_corner_radius, pi, -pi_2);
        cairo_arc(buffer->cairo_context, x + width - config_.title_bar_corner_radius, y + config_.title_bar_corner_radius, config_.title_bar_corner_radius, -pi_2, 0);
        cairo_arc(buffer->cairo_context, x + width, y + config_.title_bar_height, 0, 0, 0);
        cairo_fill_preserve(buffer->cairo_context);

        cairo_set_source_rgba(buffer->cairo_context, config_.stroke_intensity, config_.stroke_intensity, config_.stroke_intensity, 1);
        cairo_stroke(buffer->cairo_context);

        // Text
        cairo_set_source_rgb(buffer->cairo_context, 1, 1, 1);
        cairo_select_font_face(buffer->cairo_context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(buffer->cairo_context, config_.title_bar_font_size);
        cairo_text_extents_t text_extents;
        cairo_text_extents(buffer->cairo_context, config_.title_bar_text.c_str(), &text_extents);
        cairo_move_to(
            buffer->cairo_context,
            (buffer->width - text_extents.width) / 2.0 - text_extents.x_bearing,
            y + (config_.title_bar_height - text_extents.height) / 2.0 - text_extents.y_bearing);
        cairo_show_text(buffer->cairo_context, config_.title_bar_text.c_str());
    }

    // Client rectangle
    {
        auto const i{config_.background_intensity};
        cairo_set_source_rgba(buffer->cairo_context, i, i, i, alpha);
        cairo_rectangle(buffer->cairo_context, x, y + config_.title_bar_height, width, height - config_.title_bar_height);
        cairo_fill_preserve(buffer->cairo_context);

        cairo_set_source_rgba(buffer->cairo_context, config_.stroke_intensity, config_.stroke_intensity, config_.stroke_intensity, 1);
        cairo_stroke(buffer->cairo_context);
    }

    // Text
    {
        cairo_set_source_rgb(buffer->cairo_context, 0, 0, 0);
        cairo_select_font_face(buffer->cairo_context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(buffer->cairo_context, 14);

        auto print_line{
            [&, line = 0](std::string const& text, double line_spacing = 1.25) mutable
            {
                cairo_text_extents_t text_extents;
                cairo_text_extents(buffer->cairo_context, text.c_str(), &text_extents);

                cairo_move_to(
                    buffer->cairo_context,
                    config().stroke_width + 10 - text_extents.x_bearing,
                    config().title_bar_height + 10 - text_extents.y_bearing + (line * text_extents.height * line_spacing));

                cairo_show_text(buffer->cairo_context, text.c_str());
                ++line;
            }};

        print_line("id: " + std::to_string(mir_window->id));
        print_line("parent_id: " + (mir_window->parent ? std::to_string(mir_window->parent->id) : "(nil)"));
    }
}

void mfa::DecoratedWindow::show_activated()
{
    current_intensity_offset = intensity_offset;
    redraw();
}

void mfa::DecoratedWindow::show_unactivated()
{
    current_intensity_offset = 0;
    redraw();
}
