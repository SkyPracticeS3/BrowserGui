#include "GuiElements.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <intsafe.h>
#include <memory>
#include <ostream>
#include <ranges>
#include <sstream>
#include <string>
#include <stringapiset.h>
#include <iostream>
#include <vector>
#include <wincodec.h>
#include <winnls.h>
#include <winnt.h>
#include <winuser.h>
#include <wrl/client.h>

Text::Text(ComPtr<IDWriteFactory> _d_write_factory, std::wstring txt, float _font_size,
        std::wstring _font_family,
        float _x, float _y, float _width, float _height,
         DWRITE_FONT_WEIGHT _font_weight, bool is_italic, D2D1_COLOR_F txt_clr):
        GuiElement(TextElement),
        text(std::move(txt)), font_weight(_font_weight), font_family(std::move(_font_family)),
        font_style(is_italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL),
        clr(txt_clr), font_stretch(DWRITE_FONT_STRETCH_NORMAL), d_write_factory(_d_write_factory), 
        font_size(_font_size), max_width(_width), max_height(_height), x(_x), y(_y), original_x(_x), original_y(_y){
    if(_width == FLT_MAX){
        auto_width = true;
    }
    if(_height == FLT_MAX){
        auto_height = true;
    }
    createDeviceInDependantResources();
};

void Text::setTextColor(D2D1_COLOR_F _clr) {
    clr = _clr;
    applyChanges();
};

void Text::setText(std::wstring str) {
    text = str;
    applyChanges();
};
void Text::setText(std::string str) {
    std::wstring new_str;
    int size = MultiByteToWideChar(CP_UTF8, NULL, &str[0],
        str.size(), nullptr, NULL);
    new_str.resize(size);
    MultiByteToWideChar(CP_UTF8, NULL, &str[0], str.size(), &new_str[0], new_str.size());

    text = new_str;

    applyChanges();
};
void Text::setFontSize(float _font_size) {
    font_size = _font_size;
    applyChanges();
};
void Text::setDimensions(float w, float h) {
    max_width = w;
    max_height = h;
    applyChanges();
};
// SetMargin
void Text::setLocation(float _x, float _y) {

    x = x + (_x - original_x);
    y = y + (_y - original_y);

    original_x = _x;
    original_y = _y;
    

};
void Text::applyChanges(){
    createDeviceInDependantResources();
    recreate_device_resources = true;
};
void Text::positionBasedOnParentRect(D2D1_RECT_F _rc){
    x = original_x + _rc.left;
    y = original_y + _rc.top;
    if(auto_width) { max_width = _rc.right - _rc.left; }
    createDeviceInDependantResources();
}; 
void Text::createDeviceInDependantResources() {
    HRESULT hr = d_write_factory->CreateTextFormat(font_family.c_str(), nullptr, font_weight,
        font_style, font_stretch, font_size, L"en-us", &txt_format);
    hr = d_write_factory->CreateTextLayout(text.c_str(), text.size(), txt_format.Get(),
        max_width, max_height, &txt_layout);
    DWRITE_TEXT_METRICS metrics = { 0 };
    txt_layout->GetMetrics(&metrics);
    if(auto_width){
        max_width = metrics.width;
    }
    if(auto_height){
        max_height = metrics.height;
    }
};
void Text::createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) {
    if(recreate_device_resources || !txt_brush){
        ctx->CreateSolidColorBrush(clr, &txt_brush);
    }
    if(recreate_device_resources) { recreate_device_resources = false; };
};
void Text::draw(ComPtr<ID2D1DeviceContext> ctx){
    std::cout << "indrawing" << std::endl;
    createDeviceResources(ctx);

    ctx->DrawTextLayout(D2D1::Point2F(x, y), txt_layout.Get(), txt_brush.Get(), 
        D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
}
D2D1_RECT_F Text::getRc(){
    return D2D1::Rect(x, y, x + max_width, x + max_height);
}
D2D1_RECT_F Text::getOriginalRc(){
    return D2D1::Rect(original_x, original_y, original_x + max_width, original_y + max_height);
}
Box::Box(float _x, float _y, float _w, float _h, float _border_rad, bool _has_border, float _border_weight,  
    D2D1_COLOR_F bk_clr, D2D1_COLOR_F border_clr): GuiElement(BoxElement),
    original_rc(_x, _y, _x + _w, _y + _h), border_radius(_border_rad), has_border(_has_border), border_weight(_border_weight), 
    bk_color(bk_clr), border_color(border_clr){
    rc = original_rc;
};

void Box::setDimensions(float w, float h) {
    original_rc.right = original_rc.left + w;
    original_rc.bottom = original_rc.top + h;

    rc.right = rc.right + w;
    rc.bottom = rc.bottom = h;

};
// setMargin
void Box::setLocation(float x, float y) {
    float width = original_rc.right - original_rc.left;
    float height = original_rc.bottom - original_rc.top;

    float old_left = original_rc.left;
    float old_top = original_rc.top;

    float x_diff = x - old_left;
    float y_diff = y - old_top;

    rc.left = (rc.left + x_diff);
    rc.top = (rc.top + y_diff);
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;

    original_rc.left = x + width;
    original_rc.top = y + height;
    original_rc.right = original_rc.left + width;
    original_rc.bottom = original_rc.top + height;

    for(auto& child : children)
        child->positionBasedOnParentRect(rc);

};
void Box::applyChanges() {
    recreate_device_resources = true;
};
void Box::createDeviceInDependantResources() {};
void Box::createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) {
    if(!bk_brush || recreate_device_resources)
        ctx->CreateSolidColorBrush(bk_color, &bk_brush);
    if(!(border_brush && has_border) || recreate_device_resources)
        ctx->CreateSolidColorBrush(border_color, &border_brush);
    if(recreate_device_resources) { recreate_device_resources = false; };
};

void Box::setBorderColor(D2D1_COLOR_F _clr){
    border_color = _clr;
    applyChanges();
};
void Box::setBkColor(D2D1_COLOR_F _clr){
    bk_color = _clr;
    applyChanges();
};

void Box::draw(ComPtr<ID2D1DeviceContext> ctx) {
    createDeviceResources(ctx);

    if(has_border){
        D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(rc.left + 0.5f, rc.top + 0.5f, rc.right + 0.5f, rc.bottom + 0.5f), border_radius, border_radius);
        ctx->FillRoundedRectangle(rect, bk_brush.Get());
        ctx->DrawRoundedRectangle(rect, border_brush.Get(), border_weight);
        for(auto& child : children)
            child->draw(ctx);
        return;
    }
    D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(rc.left, rc.top, rc.right,
        rc.bottom), border_radius, border_radius);
    ctx->FillRoundedRectangle(rect, bk_brush.Get());

    for(auto& child : children)
        child->draw(ctx);
};

void Box::positionBasedOnParentRect(D2D1_RECT_F _rc) {
    rc.left = _rc.left + original_rc.left; // parent x + left margin
    rc.top = _rc.top + original_rc.top; // parent y + top margin
    rc.bottom = rc.top + (original_rc.bottom - original_rc.top); // new y + height
    rc.right = rc.left + (original_rc.right - original_rc.left); // new x + width
}
void Box::appendChild(std::shared_ptr<GuiElement> elm) {
    elm->positionBasedOnParentRect(rc);

    if(display == Block){
        if(children.size() > 0){
            D2D1_RECT_F current_elm_rc = elm->getOriginalRc();
            D2D1_RECT_F last_child_rc = children.back()->getOriginalRc();
            elm->setLocation(current_elm_rc.left, current_elm_rc.top + last_child_rc.bottom);
        }
    }

    children.push_back(elm);
    

};
D2D1_RECT_F Box::getRc(){
    return rc;
}
D2D1_RECT_F Box::getOriginalRc(){
    return original_rc;
}

Button::Button(ComPtr<IDWriteFactory> factory, float _x, float _y, float _w,
    float _h, std::wstring _str,
    std::wstring _font_family, float _font_size,
    bool _has_border, float _border_weight,
    DWRITE_FONT_WEIGHT _font_weight, bool _is_italic,
    float _border_radius, D2D1_COLOR_F _bk_clr,
    D2D1_COLOR_F _border_clr, D2D1_COLOR_F _txt_clr):
    GuiElement(ButtonElement), d_write_factory(factory), original_rc(_x, _y, _x + _w, _y + _h),
    font_weight(_font_weight), font_style(_is_italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL),
    font_stretch(DWRITE_FONT_STRETCH_NORMAL), bk_clr(_bk_clr), txt_clr(_txt_clr), border_clr(_border_clr),
    font_size(_font_size), font_family(std::move(_font_family)), str(std::move(_str)),
    border_weight(_border_weight), has_border(_has_border), border_radius(_border_radius) {
    

    rc = original_rc;
    createDeviceInDependantResources();

};

void Button::setText(std::wstring _str) {
    str = _str;
    applyChanges();
};
void Button::setText(std::string _str){
    std::wstring new_str;
    int size = MultiByteToWideChar(CP_UTF8, NULL, &_str[0], _str.size(), nullptr, NULL);
    new_str.resize(size);
    MultiByteToWideChar(CP_UTF8, NULL, &_str[0], _str.size(), &new_str[0], new_str.size());

    str = std::move(new_str);
    createDeviceInDependantResources    ();
};

void Button::setFontSize(float _font_size) {
    font_size = _font_size;
    createDeviceInDependantResources();
};

void Button::setDimensions(float w, float h) {
    original_rc.right = original_rc.left + w;
    original_rc.bottom = original_rc.top + h;

    rc.right = rc.left + w;
    rc.bottom = rc.top + h;

    createDeviceInDependantResources();
};
void Button::setLocation(float x, float y) {
    float width = original_rc.right - original_rc.left;
    float height = original_rc.bottom - original_rc.top;

    float old_left = original_rc.left;
    float old_top = original_rc.top;

    float x_diff = x - old_left;
    float y_diff = y - old_top;

    rc.left = (rc.left + x_diff);
    rc.top = (rc.top + y_diff);
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;

    original_rc.left = x + width;
    original_rc.top = y + height;
    original_rc.right = original_rc.left + width;
    original_rc.bottom = original_rc.top + height;
};

void Button::setTextColor(D2D1_COLOR_F clr) {
    txt_clr = clr;
    recreate_device_resources = true;
};
void Button::applyChanges() {
    createDeviceInDependantResources();
    recreate_device_resources = true;
};
void Button::createDeviceInDependantResources() {
    HRESULT hr = d_write_factory->CreateTextFormat(font_family.c_str(), nullptr, font_weight,
        font_style, font_stretch, font_size, L"en-us", &txt_format);
    txt_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    txt_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    hr = d_write_factory->CreateTextLayout(str.c_str(), str.size(), txt_format.Get(),
        rc.right - rc.left, rc.bottom - rc.top, &txt_layout);  
};
void Button::createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) {
    if(!bk_brush || recreate_device_resources)
        ctx->CreateSolidColorBrush(bk_clr, &bk_brush);
    if(!border_brush || recreate_device_resources)
        ctx->CreateSolidColorBrush(border_clr, &border_brush);
    if(!txt_brush || recreate_device_resources)
        ctx->CreateSolidColorBrush(txt_clr, &txt_brush);
    if(recreate_device_resources) { recreate_device_resources = false; };
};
void Button::draw(ComPtr<ID2D1DeviceContext> ctx) {
    createDeviceResources(ctx);

    if(has_border){
        ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rc.left + 0.5f, rc.top + 0.5f, rc.right + 0.5f, rc.bottom + 0.5f), border_radius, border_radius),
            bk_brush.Get());
        ctx->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rc.left + 0.5f, rc.top + 0.5f, rc.right + 0.5f, rc.bottom + 0.5f), border_radius, border_radius),
            border_brush.Get(), border_weight);
        ctx->DrawTextLayout(D2D1::Point2F(rc.left, rc.top), txt_layout.Get(), txt_brush.Get());
        return;
    }
    ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rc.left, rc.top,
        rc.right, rc.bottom), border_radius, border_radius),
        bk_brush.Get());
    ctx->DrawTextLayout(D2D1::Point2F(rc.left, rc.top), txt_layout.Get(), txt_brush.Get());
    
};
void Button::positionBasedOnParentRect(D2D1_RECT_F _rc) {
    rc.left = _rc.left + original_rc.left; // parent x + left margin
    rc.top = _rc.top + original_rc.top; // parent y + top margin
    rc.bottom = rc.top + (original_rc.bottom - original_rc.top); // new y + height
    rc.right = rc.left + (original_rc.right - original_rc.left); // new x + width
};
D2D1_RECT_F Button::getRc() { return rc; };
D2D1_RECT_F Button::getOriginalRc() { return original_rc; };


std::vector<std::shared_ptr<GuiElement>> Box::reverseFlatten() {
    std::vector<std::shared_ptr<GuiElement>> gui_elms;
    reverseFlattenRecurse(gui_elms);
    return gui_elms;
};
void Box::reverseFlattenRecurse(std::vector<std::shared_ptr<GuiElement>>& result_elms) {
    
    for(auto& child : std::ranges::reverse_view(children)){
        child->reverseFlattenRecurse(result_elms);
    }

    result_elms.push_back(shared_from_this());
};
Event<>& Button::signal_clicked() {
    return click_event;
}

Input::Input(ComPtr<IDWriteFactory> factory, float _x, float _y, float _w,
    float _h, std::wstring _str,
    std::wstring _font_family, float _font_size,
    bool _has_border, float _border_weight,
    DWRITE_FONT_WEIGHT _font_weight, bool _is_italic,
    float _border_radius, D2D1_COLOR_F _bk_clr,
    D2D1_COLOR_F _border_clr, D2D1_COLOR_F _txt_clr, bool _auto_caret_height, size_t _caret_height,
    float _padding_left, bool _is_centered_horizontally): GuiElement(InputElement), 
    original_rc(_x, _y, _x + _w, _y + _h), str(std::move(_str)), font_family(std::move(_font_family)),
    font_size(_font_size), has_border(_has_border), border_weight(_border_weight),
    font_style(_is_italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL), border_radius(_border_radius),
    bk_clr(_bk_clr), border_clr(_border_clr), txt_clr(_txt_clr), auto_caret_height(_auto_caret_height),
    caret_height(_caret_height), padding_left(_padding_left), is_centered_horizontally(_is_centered_horizontally),
    font_stretch(DWRITE_FONT_STRETCH_NORMAL), font_weight(_font_weight), d_write_factory(factory),
    caret_pos(0), trailing(FALSE) {
    rc = original_rc;
    createDeviceInDependantResources();
};

void Input::setText(std::wstring _str) {
    str = _str;
    createDeviceInDependantResources();
};
void Input::setText(std::string _str){
    std::wstring new_str;
    int size = MultiByteToWideChar(CP_UTF8, NULL, &_str[0], _str.size(), nullptr, NULL);
    new_str.resize(size);
    MultiByteToWideChar(CP_UTF8, NULL, &_str[0], _str.size(), &new_str[0], new_str.size());

    str = std::move(new_str);
    createDeviceInDependantResources();
};

void Input::setFontSize(float _font_size) {
    font_size = _font_size;
    createDeviceInDependantResources();
};
void Input::setDimensions(float w, float h) {
    original_rc.right = original_rc.left + w;
    original_rc.bottom = original_rc.top + h;

    rc.right = rc.left + w;
    rc.bottom = rc.top + h;
    
    createDeviceInDependantResources();
};
void Input::setLocation(float x, float y) {
    float width = original_rc.right - original_rc.left;
    float height = original_rc.bottom - original_rc.top;

    float old_left = original_rc.left;
    float old_top = original_rc.top;

    float x_diff = x - old_left;
    float y_diff = y - old_top;

    rc.left = (rc.left + x_diff);
    rc.top = (rc.top + y_diff);
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;

    original_rc.left = x + width;
    original_rc.top = y + height;
    original_rc.right = original_rc.left + width;
    original_rc.bottom = original_rc.top + height;
};

void Input::setTextColor(D2D1_COLOR_F clr) { txt_clr = clr; recreate_device_resources = true; };
void Input::setBkColor(D2D1_COLOR_F clr) { bk_clr = clr; recreate_device_resources = true; };
void Input::setBorderColor(D2D1_COLOR_F clr) { border_clr = clr; recreate_device_resources = true; };
void Input::applyChanges(){
    recreate_device_resources = true;
    createDeviceInDependantResources();
};
Event<>& Input::signal_clicked(){ return click_event; };
Event<unsigned long long>& Input::signal_keyboard() { return keyboard_event; };

void Input::createDeviceInDependantResources() {
    HRESULT hr = d_write_factory->CreateTextFormat(font_family.c_str(), nullptr, font_weight,
        font_style, font_stretch, font_size, L"en-us", &txt_format);
    if(is_centered_horizontally)
        txt_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    txt_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    hr = d_write_factory->CreateTextLayout(str.c_str(), str.size(), txt_format.Get(),
        rc.right - rc.left, rc.bottom - rc.top, &txt_layout);  
};
void Input::createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) {
    if(!bk_brush || recreate_device_resources)
        ctx->CreateSolidColorBrush(bk_clr, &bk_brush);
    if(!border_brush || recreate_device_resources)
        ctx->CreateSolidColorBrush(border_clr, &border_brush);
    if(!txt_brush || recreate_device_resources)
        ctx->CreateSolidColorBrush(txt_clr, &txt_brush);
    if(recreate_device_resources) { recreate_device_resources = false; };
};

void Input::draw(ComPtr<ID2D1DeviceContext> ctx) {
    createDeviceResources(ctx);
    if(has_border){
        ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rc.left + 0.5f, rc.top + 0.5f, rc.right + 0.5f, rc.bottom + 0.5f), border_radius, border_radius),
            bk_brush.Get());
        ctx->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rc.left + 0.5f, rc.top + 0.5f, rc.right + 0.5f, rc.bottom + 0.5f), border_radius, border_radius),
            border_brush.Get(), border_weight);
        ctx->DrawTextLayout(D2D1::Point2F(rc.left + padding_left, rc.top), txt_layout.Get(), txt_brush.Get());
        if(focused)
            ctx->DrawLine(D2D1::Point2F(caret_draw_position.x, caret_draw_position.y),
                D2D1::Point2F(caret_draw_position.x, caret_draw_position.y + 15), txt_brush.Get(), 1.0f);
        return;
    }
    ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rc.left, rc.top,
        rc.right, rc.bottom), border_radius, border_radius),
        bk_brush.Get());
    ctx->DrawTextLayout(D2D1::Point2F(rc.left + padding_left, rc.top), txt_layout.Get(), txt_brush.Get());
    if(focused)
    ctx->DrawLine(D2D1::Point2F(caret_draw_position.x, caret_draw_position.y),
        D2D1::Point2F(caret_draw_position.x, caret_draw_position.y + 15), txt_brush.Get(), 1);
};

void Input::positionBasedOnParentRect(D2D1_RECT_F _rc) {
    rc.left = original_rc.left + _rc.left;
    rc.top = original_rc.top + _rc.top;
    rc.right = rc.left + (original_rc.right - original_rc.left);
    rc.bottom = rc.top + (original_rc.bottom - original_rc.top);
};

D2D1_RECT_F Input::getRc() { return rc; };
D2D1_RECT_F Input::getOriginalRc() { return original_rc; };

// the caret part is basically ai cause im not a nerd yk

// Function to insert a character.
void Input::insertChar(wchar_t target_char) {
    // Insert the character at the current caret position.
    str.insert(str.begin() + caret_pos, target_char);
    // Increment the caret position to be after the new character.
    caret_pos++;
    // Rebuild resources *before* recalculating the caret's drawing position.
    createDeviceInDependantResources();
    // Now, update the caret's drawing position.
    setCaretDrawPos();
}

// Function to erase the character to the left of the caret.
void Input::eraseChar() {
    // Only erase if the caret is not at the start and the string is not empty.
    if (!str.empty() && caret_pos > 0) {
        str.erase(caret_pos - 1, 1);
        // Decrement the caret position.
        caret_pos--;
        // Rebuild resources.
        createDeviceInDependantResources();
        // Now, update the caret's drawing position.
        setCaretDrawPos();
    }
}

// Helper function to update only the drawing position of the caret.
// This separates the logical position from the visual representation.
void Input::setCaretDrawPos() {
    if (!txt_layout) return;

    // Use a bool to indicate if the caret should be placed on the trailing edge.
    // This is especially important for the end of the line.
    BOOL isTrailing = (caret_pos == str.length());
    
    DWRITE_HIT_TEST_METRICS met = { 0 };
    float hit_x, hit_y;

    // DirectWrite API call to get the exact coordinates for the text position.
    txt_layout->HitTestTextPosition(caret_pos, isTrailing, &hit_x, &hit_y, &met);
    
    // Set the caret's drawing coordinates based on the DirectWrite output.
    caret_draw_position.x = static_cast<long>(hit_x + padding_left + rc.left);
    caret_draw_position.y = static_cast<long>(hit_y + rc.top);
}

// Sets the logical caret position and then updates the drawing position.
void Input::setCaretPos(size_t new_pos) {
    // Clamp the new position to be within the valid range of the string.
    if (new_pos > str.length()) {
        new_pos = str.length();
    }
    
    caret_pos = new_pos;
    setCaretDrawPos();
}

// Sets the caret position based on a mouse click.
void Input::setCaretPosByHitTest(POINT pos) {
    if (txt_layout) {
        pos.x -= padding_left;
        BOOL inside;
        DWRITE_HIT_TEST_METRICS met = { 0 };
        
        // Perform the hit test to find the text position and trailing state.
        txt_layout->HitTestPoint(static_cast<float>(pos.x), static_cast<float>(pos.y), &trailing, &inside, &met);
        
        // Update the caret's text position.
        caret_pos = met.textPosition;
        if (trailing) {
            caret_pos += met.length;
        }

        // Now, update the drawing position.
        setCaretDrawPos();
    }
}

void Image::createDeviceInDependantResources() {
    ComPtr<IWICBitmapDecoder> decoder;
    factory->CreateDecoderFromFilename(src.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand,
        &decoder);
    ComPtr<IWICBitmapFrameDecode> frame_decode;
    decoder->GetFrame(0, &frame_decode);
    factory->CreateFormatConverter(&conv);
    conv->Initialize(frame_decode.Get(), GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone, nullptr , 1.0, WICBitmapPaletteTypeMedianCut);
};
void Image::createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) {
    if(!bitmap || recreate_device_resources)
        ctx->CreateBitmapFromWicBitmap(conv.Get(), &bitmap);
};
D2D1_RECT_F Image::getRc() {
    return rc;
};
D2D1_RECT_F Image::getOriginalRc() {
    return original_rc;
};


void Image::setDimensions(float w, float h) {
    original_rc.right = original_rc.left + w;
    original_rc.bottom = original_rc.top + h;

    rc.right = rc.right + w;
    rc.bottom = rc.bottom = h;

};
// setMargin
void Image::setLocation(float x, float y) {
    float width = original_rc.right - original_rc.left;
    float height = original_rc.bottom - original_rc.top;

    float old_left = original_rc.left;
    float old_top = original_rc.top;

    float x_diff = x - old_left;
    float y_diff = y - old_top;

    rc.left = (rc.left + x_diff);
    rc.top = (rc.top + y_diff);
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;

    original_rc.left = x + width;
    original_rc.top = y + height;
    original_rc.right = original_rc.left + width;
    original_rc.bottom = original_rc.top + height;
}

void Image::positionBasedOnParentRect(D2D1_RECT_F _rc){
    rc.left = _rc.left + original_rc.left; // parent x + left margin
    rc.top = _rc.top + original_rc.top; // parent y + top margin
    rc.bottom = rc.top + (original_rc.bottom - original_rc.top); // new y + height
    rc.right = rc.left + (original_rc.right - original_rc.left); // new x + width   
}

void Image::draw(ComPtr<ID2D1DeviceContext> ctx){
    ctx->DrawBitmap(bitmap.Get(), rc, 1.0f, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
}

Image::Image(ComPtr<IWICImagingFactory> _factory, std::wstring source, int _x, int _y, int _w,
    int _h,
    float _border_rad, bool _has_border, float _border_weight): GuiElement(ImageElement), factory(_factory),
    src(source), original_rc(_x, _y, _x + _w, _y + _h), border_radius(_border_rad), has_border(_has_border),
    border_weight(_border_weight) {
    rc = original_rc;
    createDeviceInDependantResources();
};