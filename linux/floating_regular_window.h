#ifndef FLOATING_REGULAR_WINDOW_H_
#define FLOATING_REGULAR_WINDOW_H_

#include "decorated_window.h"

struct mir_floating_regular_surface_v1;

namespace mir_flutter_app
{
class FloatingRegularWindow : public DecoratedWindow
{
public:
    FloatingRegularWindow(wl_surface* surface, int32_t width, int32_t height);
    ~FloatingRegularWindow() override;

protected:
    void handle_keyboard_key(wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
        override;

private:
    mir_floating_regular_surface_v1* const mir_floating_regular_surface;

    FloatingRegularWindow(FloatingRegularWindow const&) = delete;
    FloatingRegularWindow(FloatingRegularWindow&&) = delete;
    FloatingRegularWindow& operator=(FloatingRegularWindow const&) = delete;
    FloatingRegularWindow& operator=(FloatingRegularWindow&&) = delete;
};
}

#endif // FLOATING_REGULAR_WINDOW_H_