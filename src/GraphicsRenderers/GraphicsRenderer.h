#pragma once
#include "../GraphicsManagers/RendererGraphicsManager.h"
#include <d2d1_1.h>
#include <memory>
#include <windef.h>
#include <wrl/client.h>
#include "../GuiHirarichy/GuiElements.h"
#include "SolidBrushWheel.h"

// main class, shouldn't be subclassed here
// should be used by a std::unique_ptr
class GraphicsRenderer {
public:
    HWND window;
    RendererGraphicsManager graphics;
    SolidBrushWheel color_wheel;
    ComPtr<ID2D1DeviceContext> d2d1_device_context;

    GraphicsRenderer(HWND hwnd);

    void draw(std::shared_ptr<GuiElement> elm);
};