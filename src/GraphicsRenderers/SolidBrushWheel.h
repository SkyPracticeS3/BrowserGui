#pragma once
#include "../GraphicsManagers/GraphicsManager.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

#define MakeEntry(D2D1Color, ColorString) {ColorString, D2D1::ColorF(D2D1Color)}

class SolidBrushWheel {
public:
    static std::vector<std::pair<std::string_view, D2D1_COLOR_F>> basic_colors;
    std::unordered_map<std::string_view, ComPtr<ID2D1SolidColorBrush>> brushes;

    void init(ComPtr<ID2D1DeviceContext> device_ctx);

    ID2D1SolidColorBrush* brush(std::string_view brush_name);
};