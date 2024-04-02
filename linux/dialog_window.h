#ifndef DIALOG_WINDOW_H_
#define DIALOG_WINDOW_H_

#include "decorated_window.h"

struct mir_dialog_surface_v1;

namespace mir_flutter_app
{
class DialogWindow : public DecoratedWindow
{
public:
    DialogWindow(wl_surface* surface, int32_t width, int32_t height, xdg_toplevel* parent);
    ~DialogWindow() override;

protected:
    void handle_keyboard_key(wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
        override;

private:
    mir_dialog_surface_v1* const mir_dialog_surface;

    DialogWindow(DialogWindow const&) = delete;
    DialogWindow(DialogWindow&&) = delete;
    DialogWindow& operator=(DialogWindow const&) = delete;
    DialogWindow& operator=(DialogWindow&&) = delete;
};
}

#endif // DIALOG_WINDOW_H_