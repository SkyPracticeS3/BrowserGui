#pragma once
#include "BaseWindow.h"
#include <memory>
#include "../GraphicsRenderers/GraphicsRenderer.h"

class MyWindow : public BaseWindow<MyWindow> {
public:
    std::unique_ptr<GraphicsRenderer> renderer;
    std::shared_ptr<GuiElement> dom;
    std::shared_ptr<GuiElement> last_focused = nullptr;

    LPCWSTR className() const { return L"TestWindow"; };
    LRESULT WINAPI handleMessage(UINT msg, WPARAM wp, LPARAM lp);
};