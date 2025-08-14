#include "GraphicsRenderer.h"
#include <winuser.h>

GraphicsRenderer::GraphicsRenderer(HWND hwnd): window(hwnd) {


    graphics.createDeviceInDependantResources();
    graphics.createDeviceDependantResources(hwnd);
    color_wheel.init(graphics.d2d1_device_context);
    d2d1_device_context = graphics.d2d1_device_context;

    
};