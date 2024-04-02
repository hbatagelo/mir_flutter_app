#include "window.h"
#include "globals.h"

#include <wayland-client.h>

#include <cairo.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace
{
auto make_shm_pool(wl_shm* shm, int size, void** data) -> wl_shm_pool*
{
    int fd{memfd_create("make_shm_pool", MFD_CLOEXEC)};
    if (fd < 0) return nullptr;

    posix_fallocate(fd, 0, size);

    *data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (*data == MAP_FAILED)
    {
        close(fd);
        return nullptr;
    }

    wl_shm_pool* pool{wl_shm_create_pool(shm, fd, size)};

    close(fd);

    return pool;
}
}

namespace mfa = mir_flutter_app;

mfa::Window::Window(wl_surface* surface, int32_t width, int32_t height) :
    surface{surface},
    width{width},
    height{height}
{
    for (auto& buffer_ : buffers)
    {
        prepare_buffer(buffer_);
    }
}

mfa::Window::~Window()
{
    for (auto& buffer_ : buffers)
    {
        cairo_surface_destroy(buffer_.cairo_surface);
        cairo_destroy(buffer_.cairo_context);
        wl_buffer_destroy(buffer_.buffer);
    }
}

void mfa::Window::redraw()
{
    static wl_callback_listener const frame_listener{
        .done = [](void* ctx, auto... args) { static_cast<Window*>(ctx)->handle_frame_callback(args...); }};

    if (auto* const buffer_{find_free_buffer()})
    {
        draw_new_content(buffer_);

        auto* const new_frame_signal{wl_surface_frame(surface)};
        wl_callback_add_listener(new_frame_signal, &frame_listener, this);
        wl_surface_attach(surface, buffer_->buffer, 0, 0);
        wl_surface_commit(surface);
        need_to_draw = false;
    }
    else
    {
        need_to_draw = true;
    }
}

void mfa::Window::resize(int32_t width_, int32_t height_)
{
    if (width == width_ && height == height_) return;
    if (width_ > 0) width = width_;
    if (height_ > 0) height = height_;
    if (width_ > 0 || height_ > 0) redraw();
}

void mfa::Window::handle_frame_callback(wl_callback* callback, uint32_t /*time*/)
{
    wl_callback_destroy(callback);

    if (need_to_draw)
    {
        redraw();
    }
}

void mfa::Window::update_free_buffers(wl_buffer* buffer)
{
    for (auto& buffer_ : buffers)
    {
        if (buffer_.buffer == buffer)
        {
            buffer_.available = true;
        }
    }
}

void mfa::Window::prepare_buffer(Buffer& buffer)
{
    static wl_buffer_listener const buffer_listener{
        .release = [](void* ctx, auto... args) { static_cast<Window*>(ctx)->update_free_buffers(args...); }};

    auto const stride{width * Window::pixel_size};

    void* pool_data;
    wl_shm_pool* shm_pool{make_shm_pool(Globals::instance().shm(), width * height * Window::pixel_size, &pool_data)};

    buffer.buffer = wl_shm_pool_create_buffer(shm_pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
    buffer.available = true;
    buffer.width = width;
    buffer.height = height;
    buffer.content_area = pool_data;

    buffer.cairo_surface = cairo_image_surface_create_for_data(
        static_cast<unsigned char*>(pool_data),
        CAIRO_FORMAT_ARGB32,
        width,
        height,
        stride);
    buffer.cairo_context = cairo_create(buffer.cairo_surface);

    wl_buffer_add_listener(buffer.buffer, &buffer_listener, this);
    wl_shm_pool_destroy(shm_pool);
}

auto mfa::Window::find_free_buffer() -> Buffer*
{
    for (auto& buffer_ : buffers)
    {
        if (buffer_.available)
        {
            if (buffer_.width != width || buffer_.height != height)
            {
                cairo_surface_destroy(buffer_.cairo_surface);
                cairo_destroy(buffer_.cairo_context);
                wl_buffer_destroy(buffer_.buffer);
                prepare_buffer(buffer_);
            }

            buffer_.available = false;
            return &buffer_;
        }
    }
    return nullptr;
}
