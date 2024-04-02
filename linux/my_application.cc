#include "my_application.h"
#include "globals.h"

#include <flutter_linux/flutter_linux.h>
#include <gdk/gdkwayland.h>

#include "flutter/generated_plugin_registrant.h"

#include "mir_window.h"
#include "toplevel_window.h"

namespace
{
gchar const* const CHANNEL = "io.mir-server/window";
}

struct _MyApplication
{
    GtkApplication parent_instance;
    char** dart_entrypoint_arguments;

    FlMethodChannel* mir_window_channel;

    GtkWindow* main_window;

    std::map<int, MirWindow*> windows;
};

G_DEFINE_TYPE(MyApplication, my_application, GTK_TYPE_APPLICATION)

G_DEFINE_TYPE(MirWindow, mir_window, GTK_TYPE_WINDOW)

static void mir_window_show(GtkWidget* widget)
{
    gtk_window_set_decorated(GTK_WINDOW(widget), FALSE);
    GTK_WIDGET_CLASS(mir_window_parent_class)->show(widget);
}

static void mir_window_realize(GtkWidget* widget)
{
    GTK_WIDGET_CLASS(mir_window_parent_class)->realize(widget);
    GdkWindow* const gdk_window = gtk_widget_get_window(widget);
    gdk_wayland_window_set_use_custom_surface(gdk_window);
    g_return_if_fail(GDK_IS_WAYLAND_WINDOW(gdk_window) == true);

    mir_flutter_app::Globals::instance().bind_interfaces(gdk_window_get_display(gdk_window));
}

static void mir_window_map(GtkWidget* widget)
{
    MirWindow* self = MIR_WINDOW(widget);

    gtk_widget_set_size_request(widget, self->size.width, self->size.height);

    GdkWindow* const gdk_window = gtk_widget_get_window(widget);
    self->surface = gdk_wayland_window_get_wl_surface(gdk_window);

    switch(self->archetype)
    {
    case MirWindowArchetype::regular:
        self->toplevel = mir_flutter_app::Globals::instance().make_regular_window(self);
        break;
    case MirWindowArchetype::floating_regular:
        self->toplevel = mir_flutter_app::Globals::instance().make_floating_regular_window(self);
        break;
    case MirWindowArchetype::satellite:
        self->toplevel = mir_flutter_app::Globals::instance().make_satellite_window(self);
        break;
    case MirWindowArchetype::dialog:
        self->toplevel = mir_flutter_app::Globals::instance().make_dialog_window(self);
        break;
    }
}

static void method_response_cb(GObject* object, GAsyncResult* result, gpointer user_data)
{
    g_autoptr(GError) error = nullptr;
    g_autoptr(FlMethodResponse) response =
        fl_method_channel_invoke_method_finish(FL_METHOD_CHANNEL(object), result, &error);
    if (response == nullptr)
    {
        g_warning("Failed to call method: %s", error->message);
        return;
    }

    fl_method_response_get_result(response, &error);
    if (response == nullptr)
    {
        g_warning("Method returned error: %s", error->message);
        return;
    }
}

static void mir_window_destroy(GtkWidget* widget)
{
    MirWindow* self = MIR_WINDOW(widget);

    if (MyApplication* application = MY_APPLICATION(gtk_window_get_application(GTK_WINDOW(self))))
    {
        g_autoptr(FlValue) args = fl_value_new_int(self->id);
        fl_method_channel_invoke_method(
            application->mir_window_channel,
            "resetWindowId",
            args,
            nullptr,
            method_response_cb,
            nullptr);

        if (application->windows.contains(self->id))
        {
            application->windows.erase(self->id);
        }
        else
        {
            g_critical("Could not find window id in the window map");
        }
    }

    GTK_WIDGET_CLASS(mir_window_parent_class)->destroy(widget);
}

static void mir_window_class_init(MirWindowClass* klass)
{
    GTK_WIDGET_CLASS(klass)->show = mir_window_show;
    GTK_WIDGET_CLASS(klass)->realize = mir_window_realize;
    GTK_WIDGET_CLASS(klass)->map = mir_window_map;
    GTK_WIDGET_CLASS(klass)->destroy = mir_window_destroy;
}

static void mir_window_init(MirWindow* self) {}

static MirWindow* mir_window_new(
    MirWindowArchetype archetype,
    MirWindowSize size,
    MirWindowPositioner positioner,
    MirWindow* parent, int id)
{
    MirWindow* self = MIR_WINDOW(g_object_new(mir_window_get_type(), nullptr));
    self->archetype = archetype;
    self->size = size;
    self->positioner = positioner;
    self->parent = parent;
    self->id = id;
    return self;
}

static void mir_window_method_cb(FlMethodChannel* /*channel*/, FlMethodCall* method_call, gpointer user_data)
{
    MyApplication* self = MY_APPLICATION(user_data);

    auto const get_new_window_id{[](const std::map<int, MirWindow*>& windows)
        {
            auto new_id{0};
            for (auto const& [id, _] : windows)
            {
                if (id == new_id)
                {
                    ++new_id;
                }
                else
                {
                    break;
                }
            }
            return new_id;
        }};

    gchar const* name = fl_method_call_get_name(method_call);
    if (strcmp(name, "createRegularWindow") == 0)
    {
        FlValue* args = fl_method_call_get_args(method_call);
        if (fl_value_get_type(args) != FL_VALUE_TYPE_LIST || fl_value_get_length(args) != 2 ||
            fl_value_get_type(fl_value_get_list_value(args, 0)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 1)) != FL_VALUE_TYPE_FLOAT)
        {
            fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
            return;
        }
        MirWindowSize size{
            .width = static_cast<int>(fl_value_get_float(fl_value_get_list_value(args, 0))),
            .height = static_cast<int>(fl_value_get_float(fl_value_get_list_value(args, 1)))};

        auto const new_id{get_new_window_id(self->windows)};
        MirWindow* window = mir_window_new(MirWindowArchetype::regular, size, {}, nullptr, new_id);
        self->windows[window->id] = window;
        gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(self));
        gtk_widget_show(GTK_WIDGET(window));

        g_autoptr(FlValue) result = fl_value_new_int(window->id);
        fl_method_call_respond_success(method_call, result, nullptr);
    }
    else if (strcmp(name, "createFloatingRegularWindow") == 0)
    {
        FlValue* args = fl_method_call_get_args(method_call);
        if (fl_value_get_type(args) != FL_VALUE_TYPE_LIST || fl_value_get_length(args) != 2 ||
            fl_value_get_type(fl_value_get_list_value(args, 0)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 1)) != FL_VALUE_TYPE_FLOAT)
        {
            fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
            return;
        }
        MirWindowSize size{
            .width = static_cast<int>(fl_value_get_float(fl_value_get_list_value(args, 0))),
            .height = static_cast<int>(fl_value_get_float(fl_value_get_list_value(args, 1)))};

        auto const new_id{get_new_window_id(self->windows)};
        MirWindow* window = mir_window_new(MirWindowArchetype::floating_regular, size, {}, nullptr, new_id);
        self->windows[window->id] = window;
        gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(self));
        gtk_widget_show(GTK_WIDGET(window));

        g_autoptr(FlValue) result = fl_value_new_int(window->id);
        fl_method_call_respond_success(method_call, result, nullptr);
    }
    else if (strcmp(name, "createSatelliteWindow") == 0)
    {
        FlValue* args = fl_method_call_get_args(method_call);
        if (fl_value_get_type(args) != FL_VALUE_TYPE_LIST || fl_value_get_length(args) != 12 ||
            fl_value_get_type(fl_value_get_list_value(args, 0)) != FL_VALUE_TYPE_INT ||
            fl_value_get_type(fl_value_get_list_value(args, 1)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 2)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 3)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 4)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 5)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 6)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 7)) != FL_VALUE_TYPE_INT ||
            fl_value_get_type(fl_value_get_list_value(args, 8)) != FL_VALUE_TYPE_INT ||
            fl_value_get_type(fl_value_get_list_value(args, 9)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 10)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 11)) != FL_VALUE_TYPE_INT)
        {
            fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
            return;
        }
        int64_t parent_id = fl_value_get_int(fl_value_get_list_value(args, 0));
        MirWindowSize size{
            .width = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 1))),
            .height = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 2)))};
        MirWindowPositioner positioner{
            .anchor_rect =
                {.x = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 3))),
                              .y = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 4))),
                              .width = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 5))),
                              .height = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 6)))},
            .anchor = static_cast<mir_positioner_v1_anchor>(fl_value_get_int(fl_value_get_list_value(args, 7))),
            .gravity = static_cast<mir_positioner_v1_anchor>(fl_value_get_int(fl_value_get_list_value(args, 8))),
            .offset =
                {.dx = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 9))),
                              .dy = static_cast<int32_t>(fl_value_get_float(fl_value_get_list_value(args, 10)))},
            .constraint_adjustment = static_cast<uint32_t>(fl_value_get_int(fl_value_get_list_value(args, 11)))
        };

        if (!self->windows.contains(parent_id))
        {
            fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
            return;
        }

        MirWindow* parent_mir_window = self->windows[parent_id];

        auto const new_id{get_new_window_id(self->windows)};
        MirWindow* window = mir_window_new(MirWindowArchetype::satellite, size, positioner, parent_mir_window, new_id);
        self->windows[window->id] = window;
        gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(self));
        gtk_widget_show(GTK_WIDGET(window));

        g_autoptr(FlValue) result = fl_value_new_int(window->id);
        fl_method_call_respond_success(method_call, result, nullptr);
    }
    else if (strcmp(name, "createDialogWindow") == 0)
    {
        FlValue* args = fl_method_call_get_args(method_call);
        if (fl_value_get_type(args) != FL_VALUE_TYPE_LIST || fl_value_get_length(args) != 3 ||
            fl_value_get_type(fl_value_get_list_value(args, 0)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 1)) != FL_VALUE_TYPE_FLOAT ||
            fl_value_get_type(fl_value_get_list_value(args, 2)) != FL_VALUE_TYPE_INT)
        {
            fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
            return;
        }
        MirWindowSize size{
            .width = static_cast<int>(fl_value_get_float(fl_value_get_list_value(args, 0))),
            .height = static_cast<int>(fl_value_get_float(fl_value_get_list_value(args, 1)))};
        int64_t parent_id = fl_value_get_int(fl_value_get_list_value(args, 2));

        MirWindow* parent_mir_window{};
        if (parent_id >= 0)
        {
            if (!self->windows.contains(parent_id))
            {
                fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
                return;
            }

            parent_mir_window = self->windows[parent_id];
        }

        auto const new_id{get_new_window_id(self->windows)};
        MirWindow* window = mir_window_new(MirWindowArchetype::dialog, size, {}, parent_mir_window, new_id);
        self->windows[window->id] = window;
        gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(self));
        gtk_widget_show(GTK_WIDGET(window));

        g_autoptr(FlValue) result = fl_value_new_int(window->id);
        fl_method_call_respond_success(method_call, result, nullptr);
    }
    else if (strcmp(name, "closeWindow") == 0)
    {
        FlValue* args = fl_method_call_get_args(method_call);
        if (fl_value_get_type(args) != FL_VALUE_TYPE_LIST || fl_value_get_length(args) != 1 ||
            fl_value_get_type(fl_value_get_list_value(args, 0)) != FL_VALUE_TYPE_INT)
        {
            fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
            return;
        }

        int64_t window_id = fl_value_get_int(fl_value_get_list_value(args, 0));
        if (!self->windows.contains(window_id))
        {
            fl_method_call_respond_error(method_call, "Bad Arguments", "", nullptr, nullptr);
            return;
        }

        MirWindow* mir_window = self->windows[window_id];
        mir_flutter_app::Globals::instance().close_window(mir_window->surface);
    }
    else
    {
        fl_method_call_respond_not_implemented(method_call, nullptr);
    }
}

// Implements GApplication::activate.
static void my_application_activate(GApplication* application)
{
    MyApplication* self = MY_APPLICATION(application);
    self->main_window = GTK_WINDOW(gtk_application_window_new(GTK_APPLICATION(application)));
    gtk_window_set_title(self->main_window, "mir_flutter_app");

    gtk_window_set_default_size(self->main_window, 1280, 720);
    gtk_widget_show(GTK_WIDGET(self->main_window));

    g_autoptr(FlDartProject) project = fl_dart_project_new();
    fl_dart_project_set_dart_entrypoint_arguments(project, self->dart_entrypoint_arguments);

    FlView* view = fl_view_new(project);
    gtk_widget_show(GTK_WIDGET(view));
    gtk_container_add(GTK_CONTAINER(self->main_window), GTK_WIDGET(view));

    FlBinaryMessenger* messenger = fl_engine_get_binary_messenger(fl_view_get_engine(view));
    g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
    self->mir_window_channel = fl_method_channel_new(messenger, CHANNEL, FL_METHOD_CODEC(codec));
    fl_method_channel_set_method_call_handler(self->mir_window_channel, mir_window_method_cb, self, nullptr);

   self->windows = {};

    fl_register_plugins(FL_PLUGIN_REGISTRY(view));

    gtk_widget_grab_focus(GTK_WIDGET(view));
}

// Implements GApplication::local_command_line.
static gboolean my_application_local_command_line(GApplication* application, gchar*** arguments, int* exit_status)
{
    MyApplication* self = MY_APPLICATION(application);
    // Strip out the first argument as it is the binary name.
    self->dart_entrypoint_arguments = g_strdupv(*arguments + 1);

    g_autoptr(GError) error = nullptr;
    if (!g_application_register(application, nullptr, &error))
    {
        g_warning("Failed to register: %s", error->message);
        *exit_status = 1;
        return TRUE;
    }

    g_application_activate(application);
    *exit_status = 0;

    return TRUE;
}

// Implements GObject::dispose.
static void my_application_dispose(GObject* object)
{
    MyApplication* self = MY_APPLICATION(object);
    g_clear_pointer(&self->dart_entrypoint_arguments, g_strfreev);
    g_clear_object(&self->mir_window_channel);
    G_OBJECT_CLASS(my_application_parent_class)->dispose(object);
}

static void my_application_class_init(MyApplicationClass* klass)
{
    G_APPLICATION_CLASS(klass)->activate = my_application_activate;
    G_APPLICATION_CLASS(klass)->local_command_line = my_application_local_command_line;
    G_OBJECT_CLASS(klass)->dispose = my_application_dispose;
}

static void my_application_init(MyApplication* self) {}

MyApplication* my_application_new()
{
    return MY_APPLICATION(g_object_new(
        my_application_get_type(),
        "application-id",
        APPLICATION_ID,
        "flags",
        G_APPLICATION_NON_UNIQUE,
        nullptr));
}
