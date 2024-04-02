#ifndef SATELLITE_WINDOW_H_
#define SATELLITE_WINDOW_H_

#include "decorated_window.h"

struct mir_positioner_v1;
struct mir_satellite_surface_v1;
struct xdg_toplevel;

namespace mir_flutter_app
{
class SatelliteWindow : public DecoratedWindow
{
public:
    SatelliteWindow(
        wl_surface* surface,
        int32_t width,
        int32_t height,
        mir_positioner_v1* positioner,
        xdg_toplevel* parent);
    ~SatelliteWindow() override;

private:
    mir_satellite_surface_v1* const mir_satellite_surface;

    void handle_repositioned(mir_satellite_surface_v1* mir_satellite_surface_v1, uint32_t token);

    SatelliteWindow(SatelliteWindow const&) = delete;
    SatelliteWindow(SatelliteWindow&&) = delete;
    SatelliteWindow& operator=(SatelliteWindow const&) = delete;
    SatelliteWindow& operator=(SatelliteWindow&&) = delete;
};
}

#endif // SATELLITE_WINDOW_H_