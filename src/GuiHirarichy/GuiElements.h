#pragma once
#include "Events.h"
#include <cfloat>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <functional>
#include <minwindef.h>
#include <vector>
#include <wrl.h>
#include <dwrite.h>
#include <memory>
#include <wrl/client.h>
#include <wincodec.h>
#include <string>

#define setDim original_rc.right = original_rc.left + w;\
    original_rc.bottom = original_rc.top + h;\
    rc.right = rc.left + w;\
    rc.bottom = rc.top + h;\

#define setLoc float width = original_rc.right - original_rc.left;\
    float height = original_rc.bottom - original_rc.top;\
    float old_left = original_rc.left;\
    float old_top = original_rc.top;\
    float x_diff = x - old_left;\
    float y_diff = y - old_top;\
    rc.left = (rc.left + x_diff);\
    rc.top = (rc.top + y_diff);\
    rc.right = rc.left + width;\
    rc.bottom = rc.top + height;\
    original_rc.left = x;\
    original_rc.top = y;\
    original_rc.right = original_rc.left + width;\
    original_rc.bottom = original_rc.top + height;

#define posBasedOnParent rc.left = _rc.left + original_rc.left; \
    rc.top = _rc.top + original_rc.top; \
    rc.bottom = rc.top + (original_rc.bottom - original_rc.top); \
    rc.right = rc.left + (original_rc.right - original_rc.left);   

using Microsoft::WRL::ComPtr;

enum ElementType {
    TextElement, ButtonElement, InputElement, ImageElement, BoxElement
};

enum Display {
    Block, Flex, Grid
};

class GuiElement : public std::enable_shared_from_this<GuiElement> {
public:
    ElementType elm_type;
    bool recreate_device_resources = false;
    bool focused = false;

    GuiElement(ElementType elm_type_arg): elm_type(elm_type_arg) {};

    virtual void draw(ComPtr<ID2D1DeviceContext> ctx) = 0;
    virtual void createDeviceInDependantResources() = 0;
    virtual void createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) = 0;
    virtual void positionBasedOnParentRect(D2D1_RECT_F rc) = 0;

    // used for hit testing
    virtual std::vector<std::shared_ptr<GuiElement>> reverseFlatten() { return {}; };
    virtual void reverseFlattenRecurse(std::vector<std::shared_ptr<GuiElement>>& result_elms) {
        result_elms.push_back(shared_from_this());
    };

    virtual D2D1_RECT_F getRc() = 0;
    virtual D2D1_RECT_F getOriginalRc() = 0;
    RECT getWin32Rc(){
        D2D1_RECT_F rec = getRc();
        return {
            (long)rec.left, (long)rec.top, (long)rec.right, (long)rec.bottom
        };
    }
    virtual void setDimensions(float w, float h) = 0;
    virtual void setLocation(float x, float y) = 0;

    template<typename T>
    T* to(){
        return dynamic_cast<T*>(this);
    }
};

class Text : public GuiElement {
    ComPtr<IDWriteFactory> d_write_factory;
    ComPtr<IDWriteTextFormat> txt_format;
    ComPtr<IDWriteTextLayout> txt_layout;
    D2D1_COLOR_F clr;
    DWRITE_FONT_WEIGHT font_weight;
    DWRITE_FONT_STYLE font_style;
    DWRITE_FONT_STRETCH font_stretch;
    std::wstring font_family;
    float font_size;
    float max_width;
    float max_height;
    float x;
    float y;
    float original_x;
    float original_y;
    bool auto_width;
    bool auto_height;
    ComPtr<ID2D1SolidColorBrush> txt_brush;
public:

    std::wstring text;

    Text(ComPtr<IDWriteFactory> _d_write_factory, std::wstring txt = L"", float _font_size = 20,
        std::wstring _font_family = L"Consolas",
        float _x = 0, float _y = 0, float _width = FLT_MAX, float _height = FLT_MAX,
         DWRITE_FONT_WEIGHT _font_weight = DWRITE_FONT_WEIGHT_NORMAL,
            bool is_italic = false, D2D1_COLOR_F txt_clr = D2D1::ColorF(0, 0, 0));

    void setText(std::wstring str);
    void setText(std::string str);

    void setFontSize(float _font_size);

    void setDimensions(float w, float h) override;
    void setLocation(float x, float y) override;
    
    void setTextColor(D2D1_COLOR_F clr);

    void applyChanges();

    void createDeviceInDependantResources() override;
    void createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) override;

    void draw(ComPtr<ID2D1DeviceContext> ctx) override;

    void positionBasedOnParentRect(D2D1_RECT_F rc) override;

    D2D1_RECT_F getRc() override;
    D2D1_RECT_F getOriginalRc() override;
};

class Box : public GuiElement {
    D2D1_RECT_F original_rc;
    D2D1_RECT_F rc;
    float border_radius;
    ComPtr<ID2D1SolidColorBrush> bk_brush;
    ComPtr<ID2D1SolidColorBrush> border_brush;
    D2D1_COLOR_F bk_color;
    D2D1_COLOR_F border_color;
    bool has_border;
    float border_weight;
    Display display;
public:

    std::vector<std::shared_ptr<GuiElement>> children;

    Box(float _x = 0.0f, float _y = 0.0f, float _w = 0.0f, float _h = 0.0f, float _border_rad = 0.0f, Display displ = Block,
        bool _has_border = false, float _border_weight = 0.0f,  
        D2D1_COLOR_F bk_clr = D2D1::ColorF(0.0f, 0.0f, 0.0f), D2D1_COLOR_F border_clr = 
            D2D1::ColorF(0.0f, 0.0f, 0.0f));

    void setDimensions(float w, float h) override;
    void setLocation(float x, float y) override;

    void applyChanges();

    void setBorderColor(D2D1_COLOR_F _clr);
    void setBkColor(D2D1_COLOR_F _clr);

    void createDeviceInDependantResources() override;
    void createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) override;

    void draw(ComPtr<ID2D1DeviceContext> ctx) override;

    void appendChild(std::shared_ptr<GuiElement> elm);

    void positionBasedOnParentRect(D2D1_RECT_F rc) override;
    void rePositionAllChildrenBasedOnDisplay();

    std::vector<std::shared_ptr<GuiElement>> reverseFlatten() override;
    void reverseFlattenRecurse(std::vector<std::shared_ptr<GuiElement>>& result_elms) override;

    D2D1_RECT_F getRc() override;
    D2D1_RECT_F getOriginalRc() override;
};

class BasicPositioningManager {
protected:
    D2D1_RECT_F original_rc;
    D2D1_RECT_F rc;
public:
    BasicPositioningManager(D2D1_RECT_F org_rc): original_rc(org_rc) {
        rc = original_rc;
    };

    void setBoxDimensions(float w, float h);
    void setBoxLocation(float x, float y);

    void positionBoxBasedOnParentRect(D2D1_RECT_F rc);

    D2D1_RECT_F getRc();
    D2D1_RECT_F getOriginalRc();

};

class Image :  public GuiElement {
    ComPtr<ID2D1Bitmap> bitmap;
    ComPtr<IWICFormatConverter> conv;
    ComPtr<IWICImagingFactory> factory;
    D2D1_RECT_F original_rc;
    D2D1_RECT_F rc;
    float border_radius;    
    bool has_border;
    float border_weight;
    std::wstring src;
public:
    Image(ComPtr<IWICImagingFactory> _factory, std::wstring source, int _x = 0.0f, int _y = 0.0f, int _w = 0.0f,
        int _h = 0.0f,
        float _border_rad = 0.0f, bool _has_border = false, float _border_weight = 0.0f);
    
    void setDimensions(float w, float h) override;
    void setLocation(float x, float y) override;

    void positionBasedOnParentRect(D2D1_RECT_F rc) override;

    void createDeviceInDependantResources() override;
    void createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) override;

    D2D1_RECT_F getRc() override;
    D2D1_RECT_F getOriginalRc() override;

    void draw(ComPtr<ID2D1DeviceContext> ctx) override;
};

class Button : public GuiElement {

    ComPtr<IDWriteFactory> d_write_factory;
    ComPtr<IDWriteTextFormat> txt_format;
    ComPtr<IDWriteTextLayout> txt_layout;

    ComPtr<ID2D1SolidColorBrush> bk_brush;
    ComPtr<ID2D1SolidColorBrush> border_brush;
    ComPtr<ID2D1SolidColorBrush> txt_brush;

    DWRITE_FONT_WEIGHT font_weight;
    DWRITE_FONT_STYLE font_style;
    DWRITE_FONT_STRETCH font_stretch;

    D2D1_RECT_F original_rc;
    D2D1_RECT_F rc;

    float border_radius;

    D2D1_COLOR_F bk_clr;
    D2D1_COLOR_F border_clr;
    D2D1_COLOR_F txt_clr;

    Event<> click_event;

    float font_size;

    std::wstring font_family;
    std::wstring str;

    bool has_border;
    float border_weight;

public:

    Button(ComPtr<IDWriteFactory> factory = nullptr, float _x = 0, float _y = 0, float _w = 70,
        float _h = 30, std::wstring _str = L"Submit",
        std::wstring _font_family = L"Arial", float _font_size = 12,
        bool _has_border = true, float _border_weight = 1.0f,
        DWRITE_FONT_WEIGHT _font_weight = DWRITE_FONT_WEIGHT_NORMAL, bool _is_italic = false,
        float _border_radius = 3.0f, D2D1_COLOR_F _bk_clr = D2D1::ColorF(D2D1::ColorF::Gray),
        D2D1_COLOR_F _border_clr = D2D1::ColorF(D2D1::ColorF::WhiteSmoke), D2D1_COLOR_F _txt_clr = 
        D2D1::ColorF(D2D1::ColorF::Black));

    void setText(std::wstring str);
    void setText(std::string str);

    void setFontSize(float _font_size);

    void setDimensions(float w, float h) override;
    void setLocation(float x, float y) override;
    
    void setTextColor(D2D1_COLOR_F clr);
    void setBkColor(D2D1_COLOR_F clr);
    void setBorderColor(D2D1_COLOR_F clr);

    void applyChanges();

    Event<>& signal_clicked();

    void createDeviceInDependantResources() override;
    void createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) override;

    void draw(ComPtr<ID2D1DeviceContext> ctx) override;

    void positionBasedOnParentRect(D2D1_RECT_F rc) override;

    D2D1_RECT_F getRc() override;
    D2D1_RECT_F getOriginalRc() override;

};

struct Point {
    FLOAT x;
    FLOAT y;
};

// most complex probably
class Input : public GuiElement {

    ComPtr<IDWriteFactory> d_write_factory;
    ComPtr<IDWriteTextFormat> txt_format;
    ComPtr<IDWriteTextLayout> txt_layout;

    ComPtr<ID2D1SolidColorBrush> bk_brush;
    ComPtr<ID2D1SolidColorBrush> border_brush;
    ComPtr<ID2D1SolidColorBrush> txt_brush;

    DWRITE_FONT_WEIGHT font_weight;
    DWRITE_FONT_STYLE font_style;
    DWRITE_FONT_STRETCH font_stretch;

    D2D1_RECT_F original_rc;
    D2D1_RECT_F rc;

    float border_radius;

    D2D1_COLOR_F bk_clr;
    D2D1_COLOR_F border_clr;
    D2D1_COLOR_F txt_clr;

    Event<unsigned long long /*key*/> keyboard_event;
    Event<> click_event;

    float font_size;

    std::wstring font_family;

    bool has_border;
    float border_weight;

    bool auto_caret_height;
    size_t caret_height;
    BOOL trailing;
    Point caret_draw_position = { 0 , 0 };

    float padding_left;
    bool is_centered_horizontally;

public:
    size_t caret_pos;
    std::wstring str;

    Input(ComPtr<IDWriteFactory> factory = nullptr, float _x = 0, float _y = 0, float _w = 120,
        float _h = 25, std::wstring _str = L"Submit",
        std::wstring _font_family = L"Arial", float _font_size = 12,
        bool _has_border = true, float _border_weight = 2.0f,
        DWRITE_FONT_WEIGHT _font_weight = DWRITE_FONT_WEIGHT_NORMAL, bool _is_italic = false,
        float _border_radius = 0.0f, D2D1_COLOR_F _bk_clr = D2D1::ColorF(D2D1::ColorF::White),
        D2D1_COLOR_F _border_clr = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F _txt_clr = 
        D2D1::ColorF(D2D1::ColorF::Black), bool _auto_caret_height = true, size_t _caret_height = 0,
        float _padding_left = 5.0f, bool _is_centered_horizontally = false);

    void setText(std::wstring str);
    void setText(std::string str);

    // the position is basically the caret pos
    void insertChar(wchar_t target_char);
    void eraseChar();
    void setCaretPos(size_t new_pos);
    void setCaretPosByHitTest(POINT pos /*relative to the input*/);
    void setCaretDrawPos();

    void setFontSize(float _font_size);

    void setDimensions(float w, float h) override;
    void setLocation(float x, float y) override;
    
    void setTextColor(D2D1_COLOR_F clr);
    void setBkColor(D2D1_COLOR_F clr);
    void setBorderColor(D2D1_COLOR_F clr);

    void applyChanges();

    Event<>& signal_clicked();
    Event<unsigned long long>& signal_keyboard();

    void createDeviceInDependantResources() override;
    void createDeviceResources(ComPtr<ID2D1DeviceContext> ctx) override;

    void draw(ComPtr<ID2D1DeviceContext> ctx) override;

    void positionBasedOnParentRect(D2D1_RECT_F rc) override;

    D2D1_RECT_F getRc() override;
    D2D1_RECT_F getOriginalRc() override;

};