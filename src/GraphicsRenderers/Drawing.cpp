#include "GraphicsRenderer.h"
#include <d2d1helper.h>

void GraphicsRenderer::draw(std::shared_ptr<GuiElement> gui_elm/*DOM Basically*/){
    PAINTSTRUCT ps;
    BeginPaint(window, &ps);
    graphics.createDeviceDependantResources(window);
    d2d1_device_context->BeginDraw();

    d2d1_device_context->Clear(D2D1::ColorF(D2D1::ColorF::White));

    gui_elm->draw(d2d1_device_context);

    d2d1_device_context->EndDraw();
    graphics.dxgi_swap_chain->Present(1, 0);

    EndPaint(window, &ps);
}