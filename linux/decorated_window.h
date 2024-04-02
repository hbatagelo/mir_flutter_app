#ifndef DECORATED_WINDOW_H_
#define DECORATED_WINDOW_H_

#include "toplevel_window.h"

#include <string>

namespace mir_flutter_app
{
class DecoratedWindow : public ToplevelWindow
{
public:
    struct Configuration
    {
        double background_intensity{0.9};

        std::string title_bar_text{"mir_shell"};
        double title_bar_corner_radius{14};
        double title_bar_intensity{0.4};
        double title_bar_height{36.0};
        double title_bar_font_size{16.0};

        double stroke_width{1.0};
        double stroke_intensity{0.2};
    };

    DecoratedWindow(wl_surface* surface, int32_t width, int32_t height, Configuration config);

    auto config() -> Configuration const& { return config_; }

protected:
    void handle_mouse_button(wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
        override;

    void draw_new_content(Buffer* buffer) override;

private:
    struct Rectangle
    {
        double left{};
        double top{};
        double right{};
        double bottom{};
    };

    Configuration const config_;
    double const close_button_scale{0.25};

    double alpha{1};
    double intensity_offset{0.1};
    double current_intensity_offset{};

    Rectangle close_button_rect{};
    bool pressed_close_button{};

    void show_activated() override;
    void show_unactivated() override;
};
}

#endif // DECORATED_WINDOW_H_
