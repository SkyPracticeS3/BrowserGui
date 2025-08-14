#pragma once
#include <Windows.h>
#include <libloaderapi.h>
#include <minwindef.h>
#include <winuser.h>

template<typename Derived>
class BaseWindow {
public:
    static LRESULT WINAPI windowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
        Derived* derived = nullptr;

        if(msg == WM_NCCREATE){
            LPCREATESTRUCT create_struct = (LPCREATESTRUCT)lp;
            derived = (Derived*)create_struct->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)derived);
            derived->m_hwnd = hwnd;
        } else {
            derived = (Derived*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }

        if(derived) return derived->handleMessage(msg, wp, lp);
        else return DefWindowProc(hwnd, msg, wp, lp);
    };

    BOOL createWindow(LPCWSTR window_name, DWORD style,
        INT y, INT x, INT width, INT height, HWND parent, HMENU menu){
        WNDCLASSW cl = { 0 };
        cl.hCursor = LoadCursor(NULL, IDC_ARROW);
        cl.lpfnWndProc = windowProc;
        cl.lpszClassName = className();

        RegisterClassW(&cl);

        m_hwnd = CreateWindowW(className(), window_name, style, x, y, width, height, parent, menu,
            GetModuleHandleW(NULL), this);

        return (m_hwnd ? TRUE : FALSE);
    }
protected:
    HWND m_hwnd;
    
    virtual LPCWSTR className() const = 0;
    virtual LRESULT WINAPI handleMessage(UINT msg, WPARAM wp, LPARAM lp) = 0;
};