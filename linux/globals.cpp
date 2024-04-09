#include "globals.h"
#include "dialog_window.h"
#include "floating_regular_window.h"
#include "mir-shell.h"
#include "mir_window.h"
#include "regular_window.h"
#include "satellite_window.h"
#include "xdg-shell.h"

#include <gdk/gdkwayland.h>

#include <vector>

namespace mfa = mir_flutter_app;

void mfa::Globals::bind_interfaces(GdkDisplay* gdk_display)
{
    if (display_)
    {
        return;
    }

    static wl_registry_listener const registry_listener{
        .global = [](void* ctx, auto... args) { static_cast<Globals*>(ctx)->handle_wl_registry_global(args...); },
        .global_remove = [](auto...) {}};

    static xdg_wm_base_listener const shell_listener{
        .ping = [](void*, xdg_wm_base* shell, uint32_t serial) { xdg_wm_base_pong(shell, serial); }};

    static wl_pointer_listener const pointer_listener{
        .enter = [](void* ctx, auto... args) { static_cast<Globals*>(ctx)->handle_mouse_enter(args...); },
        .leave = [](void* ctx, auto... args) { static_cast<Globals*>(ctx)->handle_mouse_leave(args...); },
        .motion = [](void* ctx, auto... args) { static_cast<Globals*>(ctx)->handle_mouse_motion(args...); },
        .button = [](void* ctx, auto... args) { static_cast<Globals*>(ctx)->handle_mouse_button(args...); },
        .axis = [](void* ctx, auto... args) { static_cast<Globals*>(ctx)->handle_mouse_axis(args...); }};

    static wl_keyboard_listener const keyboard_listener{
        .keymap = [](void* self, auto... args) { static_cast<Globals*>(self)->handle_keyboard_keymap(args...); },
        .enter = [](void* self, auto... args) { static_cast<Globals*>(self)->handle_keyboard_enter(args...); },
        .leave = [](void* self, auto... args) { static_cast<Globals*>(self)->handle_keyboard_leave(args...); },
        .key = [](void* self, auto... args) { static_cast<Globals*>(self)->handle_keyboard_key(args...); },
        .modifiers = [](void* self, auto... args) { static_cast<Globals*>(self)->handle_keyboard_modifiers(args...); },
        .repeat_info = [](void* self, auto... args)
        { static_cast<Globals*>(self)->handle_keyboard_repeat_info(args...); }};

    g_return_if_fail(gdk_display);
    g_return_if_fail(GDK_IS_WAYLAND_DISPLAY(gdk_display) == true);

    compositor_ = gdk_wayland_display_get_wl_compositor(gdk_display);
    if (!compositor())
    {
        g_critical("Cannot bind to wl_compositor");
    }

    display_ = gdk_wayland_display_get_wl_display(gdk_display);
    if (!display())
    {
        g_critical("Cannot bind to wl_display");
    }

    wl_registry* const registry{wl_display_get_registry(display())};
    g_return_if_fail(registry);
    wl_registry_add_listener(registry, &registry_listener, this);
    wl_display_roundtrip(display());

    if (!output_)
    {
        g_critical("Cannot bind to wl_output");
    }
    if (!seat_)
    {
        g_critical("Cannot bind to wl_seat");
    }
    if (!shm_)
    {
        g_critical("Cannot bind to wl_shm");
    }
    if (!wm_base_)
    {
        g_critical("Cannot bind to wm_base");
    }
    if (!mir_shell_)
    {
        g_critical("Cannot bind to mir_shell");
    }

    xdg_wm_base_add_listener(wm_base(), &shell_listener, nullptr);
    wl_display_roundtrip(display());

    pointer = wl_seat_get_pointer(seat_);
    keyboard = wl_seat_get_keyboard(seat_);

    wl_keyboard_add_listener(keyboard, &keyboard_listener, this);
    wl_pointer_add_listener(pointer, &pointer_listener, this);
}

auto mfa::Globals::make_regular_window(MirWindow* window) -> std::unique_ptr<ToplevelWindow>
{
    register_window(window);

    return std::make_unique<RegularWindow>(window->surface, window->size.width, window->size.height);
}

auto mfa::Globals::make_floating_regular_window(MirWindow* window) -> std::unique_ptr<ToplevelWindow>
{
    register_window(window);

    return std::make_unique<FloatingRegularWindow>(window->surface, window->size.width, window->size.height);
}

auto mfa::Globals::make_satellite_window(MirWindow* window) -> std::unique_ptr<ToplevelWindow>
{
    register_window(window);

    auto* const positioner{mir_shell_v1_create_positioner(mir_shell_)};
    mir_positioner_v1_set_anchor_rect(
        positioner,
        window->positioner.anchor_rect.x,
        window->positioner.anchor_rect.y,
        window->positioner.anchor_rect.width,
        window->positioner.anchor_rect.height);
    mir_positioner_v1_set_anchor(positioner, window->positioner.anchor);
    mir_positioner_v1_set_gravity(positioner, window->positioner.gravity);
    mir_positioner_v1_set_offset(positioner, window->positioner.offset.dx, window->positioner.offset.dy);
    mir_positioner_v1_set_constraint_adjustment(positioner, window->positioner.constraint_adjustment);

    if (!window->parent)
    {
        g_critical("Satellite window must have a parent");
    }

    auto* const parent{static_cast<xdg_toplevel*>(*window->parent->toplevel)};
    return std::make_unique<SatelliteWindow>(
        window->surface,
        window->size.width,
        window->size.height,
        positioner,
        parent);
}

auto mfa::Globals::make_dialog_window(MirWindow* window) -> std::unique_ptr<ToplevelWindow>
{
    register_window(window);

    auto* const parent{window->parent ? static_cast<xdg_toplevel*>(*window->parent->toplevel) : nullptr};
    return std::make_unique<DialogWindow>(window->surface, window->size.width, window->size.height, parent);
}

void mfa::Globals::close_window(wl_surface* surface)
{
    auto* const mir_window{window_for(surface)};
    mir_flutter_app::Globals::instance().deregister_window(mir_window);

    std::vector<wl_surface*> children;
    for (auto const [_, window] : windows)
    {
        if (window->surface != surface && window->parent && window->parent->surface == surface)
        {
            children.push_back(window->surface);
        }
    }

    for (auto* const surface : children)
    {
        close_window(surface);
    }

    gtk_widget_destroy(GTK_WIDGET(mir_window));
}

void mfa::Globals::register_window(MirWindow* window) { windows[window->surface] = window; }

void mfa::Globals::deregister_window(MirWindow* window) { windows.erase(window->surface); }

auto mfa::Globals::window_for(wl_surface* surface) -> MirWindow*
{
    return windows.contains(surface) ? windows[surface] : nullptr;
}

bool mfa::Globals::is_registered(MirWindow* window) const { return window && windows.contains(window->surface); }

void mfa::Globals::handle_wl_registry_global(
    wl_registry* registry,
    uint32_t id,
    char const* interface,
    uint32_t version)
{
    if (std::string_view{interface} == wl_output_interface.name)
    {
        g_warn_if_fail(wl_output_interface.version >= 1);
        auto const version_{std::min(version, 2u)};
        output_ = static_cast<wl_output*>(wl_registry_bind(registry, id, &wl_output_interface, version_));
        g_print("Bound to %s (v%d)\n", wl_output_interface.name, version_);
    }
    else if (std::string_view{interface} == wl_shm_interface.name)
    {
        g_warn_if_fail(wl_shm_interface.version >= 1);
        auto const version_{std::min(version, 1u)};
        shm_ = static_cast<wl_shm*>(wl_registry_bind(registry, id, &wl_shm_interface, version_));
        // Normally we'd add a listener to pick up the supported formats here
        // As luck would have it, I know that argb8888 is the only format we support :)
        g_print("Bound to %s (v%d)\n", wl_shm_interface.name, version_);
    }
    else if (std::string_view{interface} == wl_seat_interface.name)
    {
        g_warn_if_fail(wl_seat_interface.version >= 1);
        auto const version_{std::min(version, 4u)};
        seat_ = static_cast<wl_seat*>(wl_registry_bind(registry, id, &wl_seat_interface, version_));
        g_print("Bound to %s (v%d)\n", wl_seat_interface.name, version_);
    }
    else if (std::string_view{interface} == mir_shell_v1_interface.name)
    {
        g_warn_if_fail(mir_shell_v1_interface.version >= 1);
        auto const version_{std::min(version, 1u)};
        mir_shell_ =
            static_cast<mir_shell_v1*>(wl_registry_bind(registry, id, &mir_shell_v1_interface, std::min(version, 1u)));
        g_print("Bound to %s (v%d)\n", mir_shell_v1_interface.name, version_);
    }
    else if (std::string_view{interface} == xdg_wm_base_interface.name)
    {
        g_warn_if_fail(xdg_wm_base_interface.version >= 1);
        auto const version_{std::min(version, 1u)};
        wm_base_ =
            static_cast<xdg_wm_base*>(wl_registry_bind(registry, id, &xdg_wm_base_interface, std::min(version, 1u)));
        g_print("Bound to %s (v%d)\n", xdg_wm_base_interface.name, version_);
    }
}

void mfa::Globals::handle_mouse_enter(
    wl_pointer* /*pointer*/,
    uint32_t /*serial*/,
    wl_surface* surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    mouse_focus = window_for(surface);

    g_print(
        "mouse_enter %p: (%.2f, %.2f)\n",
        mouse_focus,
        wl_fixed_to_double(surface_x),
        wl_fixed_to_double(surface_y));
}

void mfa::Globals::handle_mouse_leave(wl_pointer* /*pointer*/, uint32_t /*serial*/, wl_surface* surface)
{
    g_print("mouse_leave %p\n", mouse_focus);

    if (mouse_focus == window_for(surface))
    {
        mouse_focus = nullptr;
    }
}

void mfa::Globals::handle_mouse_motion(
    wl_pointer* /*pointer*/,
    uint32_t/*time*/,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    // g_print("mouse_motion: (%.2f, %.2f) @ %i\n", wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y), time);
    pointer_position_ = {wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y)};
}

void mfa::Globals::handle_mouse_button(
    wl_pointer* pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state)
{
    g_print("mouse_button: button %i, state %i @ %i\n", button, state, time);

    if (is_registered(mouse_focus))
    {
        mouse_focus->toplevel->handle_mouse_button(pointer, serial, time, button, state);
    }
}

void mfa::Globals::handle_mouse_axis(wl_pointer* /*pointer*/, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    g_print("mouse_axis: axis %i, value %f @ %i\n", axis, wl_fixed_to_double(value), time);
}

void mfa::Globals::handle_keyboard_keymap(wl_keyboard* /*keyboard*/, uint32_t format, int32_t /*fd*/, uint32_t size)
{
    g_print("keyboard_keymap: format %i, size %i\n", format, size);
}

void mfa::Globals::handle_keyboard_enter(wl_keyboard* /*keyboard*/, uint32_t, wl_surface* surface, wl_array*)
{
    keyboard_focus = window_for(surface);

    g_print("keyboard_enter %p\n", keyboard_focus);
}

void mfa::Globals::handle_keyboard_leave(wl_keyboard* /*keyboard*/, uint32_t, wl_surface* surface)
{
    g_print("keyboard_leave %p\n", keyboard_focus);

    if (keyboard_focus == window_for(surface))
    {
        keyboard_focus = nullptr;
    }
}

void mfa::Globals::handle_keyboard_repeat_info(wl_keyboard* /*keyboard*/, int32_t rate, int32_t delay)
{
    g_print("keyboard_modifiers: rate %i, delay %i\n", rate, delay);
}

void mfa::Globals::handle_keyboard_key(
    wl_keyboard* keyboard,
    uint32_t serial,
    uint32_t time,
    uint32_t key,
    uint32_t state)
{
    g_print("keyboard_key: key %i, state %i\n", key, state);

    if (is_registered(keyboard_focus))
    {
        keyboard_focus->toplevel->handle_keyboard_key(keyboard, serial, time, key, state);
    }
}

void mfa::Globals::handle_keyboard_modifiers(
    wl_keyboard* keyboard,
    uint32_t serial,
    uint32_t mods_depressed,
    uint32_t mods_latched,
    uint32_t mods_locked,
    uint32_t group)
{
    g_print(
        "keyboard_modifiers: depressed %i, latched %i, locked %i, group %i\n",
        mods_depressed,
        mods_latched,
        mods_locked,
        group);

    if (is_registered(keyboard_focus))
    {
        keyboard_focus->toplevel
            ->handle_keyboard_modifiers(keyboard, serial, mods_depressed, mods_latched, mods_locked, group);
    }
}
