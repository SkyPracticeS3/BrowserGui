#include "SolidBrushWheel.h"
#include <d2d1helper.h>
#include <string_view>

std::vector<std::pair<std::string_view, D2D1_COLOR_F>> SolidBrushWheel::basic_colors = {
    MakeEntry(D2D1::ColorF::White, "White"),
    MakeEntry(D2D1::ColorF::Red, "Red"),
    MakeEntry(D2D1::ColorF::Blue, "Blue"),
    MakeEntry(D2D1::ColorF::Green, "Green"),
    MakeEntry(D2D1::ColorF::Yellow, "Yellow"),
    MakeEntry(D2D1::ColorF::Brown, "Brown"),
    MakeEntry(D2D1::ColorF::Black, "Black"),
    MakeEntry(D2D1::ColorF::Aqua, "Aqua"),
    MakeEntry(D2D1::ColorF::DarkRed, "DarkRed"),
    MakeEntry(D2D1::ColorF::DarkGreen, "DarkGreen"),
    MakeEntry(D2D1::ColorF::DarkBlue, "DarkBlue"),
    MakeEntry(D2D1::ColorF::Cyan, "Cyan"),
    MakeEntry(D2D1::ColorF::Violet, "Violet"),
    MakeEntry(D2D1::ColorF::Crimson, "Crimson"),
    MakeEntry(D2D1::ColorF::LightGreen, "LightGreen"),
    MakeEntry(D2D1::ColorF::LightBlue, "LightBlue"),
    MakeEntry(D2D1::ColorF::LightCyan, "LightCyan"),
    MakeEntry(D2D1::ColorF::DarkCyan, "DarkCyan"),
    MakeEntry(D2D1::ColorF::LightYellow, "LightYellow"),
    MakeEntry(D2D1::ColorF::Pink, "Pink"),
    MakeEntry(D2D1::ColorF::LightPink, "LightPink"),
    MakeEntry(D2D1::ColorF(30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f), "LightBlack"),
    MakeEntry(D2D1::ColorF(25.0f / 255.0f, 25.0f / 255.0f, 25.0f / 255.0f), "LessLighterBlack")
    };

void SolidBrushWheel::init(ComPtr<ID2D1DeviceContext> device_ctx){
    for(auto& [name, clr] : basic_colors){
        ComPtr<ID2D1SolidColorBrush> temp_brush_ptr;
        device_ctx->CreateSolidColorBrush(clr, &temp_brush_ptr);
        brushes.insert({name, temp_brush_ptr});
    }
}
ID2D1SolidColorBrush* SolidBrushWheel::brush(std::string_view brush_name) {
    return brushes.at(brush_name).Get();
};