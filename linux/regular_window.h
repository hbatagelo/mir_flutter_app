#ifndef REGULAR_WINDOW_H_
#define REGULAR_WINDOW_H_

#include "decorated_window.h"

struct mir_regular_surface_v1;

namespace mir_flutter_app
{
class RegularWindow : public DecoratedWindow
{
public:
    RegularWindow(wl_surface* surface, int32_t width, int32_t height);
    ~RegularWindow() override;

protected:
    void handle_keyboard_key(wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
        override;
    void handle_keyboard_modifiers(
        wl_keyboard* keyboard,
        uint32_t serial,
        uint32_t mods_depressed,
        uint32_t mods_latched,
        uint32_t mods_locked,
        uint32_t group) override;

    void draw_new_content(Buffer* buffer) override;

private:
    mir_regular_surface_v1* const mir_regular_surface;
    uint32_t modifiers{};

    RegularWindow(RegularWindow const&) = delete;
    RegularWindow(RegularWindow&&) = delete;
    RegularWindow& operator=(RegularWindow const&) = delete;
    RegularWindow& operator=(RegularWindow&&) = delete;
};
}

#endif // REGULAR_WINDOW_H_