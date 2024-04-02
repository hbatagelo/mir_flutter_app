#include "toplevel_window.h"
#include "globals.h"
#include "xdg-shell.h"

#include <gdk/gdkwayland.h>

#include <linux/input-event-codes.h>

namespace mfa = mir_flutter_app;

mfa::ToplevelWindow::ToplevelWindow(wl_surface* surface, int32_t width, int32_t height) :
    Window{surface, width, height},
    xdgsurface{xdg_wm_base_get_xdg_surface(Globals::instance().wm_base(), static_cast<wl_surface*>(*this))},
    xdgtoplevel{xdg_surface_get_toplevel(xdgsurface)}
{
    static xdg_toplevel_listener const shell_toplevel_listener{
        .configure = [](void* ctx, auto... args)
            {
                static_cast<ToplevelWindow*>(ctx)->handle_xdg_toplevel_configure(args...);
            },
        .close = [](auto...) {},
        .configure_bounds = [](auto...) {},
        .wm_capabilities = [](auto...) {}};
    static xdg_surface_listener const shell_surface_listener{.configure = [](void* ctx, auto... args) {
        static_cast<ToplevelWindow*>(ctx)->handle_xdg_surface_configure(args...);
    }};

    xdg_toplevel_add_listener(xdgtoplevel, &shell_toplevel_listener, this);
    xdg_surface_add_listener(xdgsurface, &shell_surface_listener, this);
}

mfa::ToplevelWindow::~ToplevelWindow()
{
    xdg_toplevel_destroy(xdgtoplevel);
    xdg_surface_destroy(xdgsurface);
}

void mfa::ToplevelWindow::handle_mouse_button(
    wl_pointer* pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state)
{
    Window::handle_mouse_button(pointer, serial, time, button, state);

    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
    {
        xdg_toplevel_move(xdgtoplevel, Globals::instance().seat(), serial);
    }

    if (button == BTN_RIGHT && state == WL_POINTER_BUTTON_STATE_PRESSED)
    {
        xdg_toplevel_resize(xdgtoplevel, Globals::instance().seat(), serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT);
    }
}

void mfa::ToplevelWindow::handle_xdg_surface_configure(xdg_surface* surface, uint32_t serial)
{
    g_print("Received xdg_surface_configure: %d\n", serial);

    if (is_activated)
    {
        show_activated();
    }
    else
    {
        show_unactivated();
    }

    resize(pending_width, pending_height);

    xdg_surface_ack_configure(surface, serial);
}

void mfa::ToplevelWindow::handle_xdg_toplevel_configure(
    xdg_toplevel* /*toplevel*/,
    int32_t width_,
    int32_t height_,
    wl_array* states)
{
    g_print("Received xdg_toplevel_configure: width %d, height %d\n", width_, height_);

    is_activated = false;
    pending_width = width_;
    pending_height = height_;

    for (auto* state{static_cast<xdg_toplevel_state*>((states)->data)};
         (char const*)state < ((char const*)(states)->data + (states)->size);
         state++)
    {
        if (*state == XDG_TOPLEVEL_STATE_ACTIVATED)
        {
            is_activated = true;
        }
    }
}
