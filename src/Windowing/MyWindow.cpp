#include "MyWindow.h"
#include <d2d1helper.h>
#include <debugapi.h>
#include <memory>
#include "../HitTester/HitTester.h"
#include <winuser.h>

LRESULT WINAPI MyWindow::handleMessage(UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
        {
            renderer = std::make_unique<GraphicsRenderer>(m_hwnd);
            dom = std::make_shared<Box>(50, 50, 200, 300, 15, true, 1.0f, D2D1::ColorF(D2D1::ColorF::Brown), D2D1::ColorF(D2D1::ColorF::White));
            dom->to<Box>()->appendChild(std::make_shared<Text>(
                renderer->graphics.d_write_factory, L"hello", 50
            ));
            dom->to<Box>()->appendChild(std::make_shared<Text>(
                renderer->graphics.d_write_factory, L"hdssdello", 50
            ));
            dom->to<Box>()->appendChild(std::make_shared<Button>(
                renderer->graphics.d_write_factory
            ));
            dom->to<Box>()->children[2]->to<Button>()->signal_clicked().connect([](){
                MessageBoxW(NULL, L"dsds", L"", MB_OK);
            });
            dom->to<Box>()->appendChild(std::make_shared<Input>(renderer->graphics.d_write_factory));

        }
        break;
        case WM_SIZE:
            renderer->graphics.resize(m_hwnd, LOWORD(lp), HIWORD(lp));
        case WM_PAINT:
            if(dom)
                renderer->draw(dom);
        break;
        case WM_LBUTTONDOWN:{
            POINT mouse_pos;
            GetCursorPos(&mouse_pos);
            ScreenToClient(m_hwnd, &mouse_pos);

            auto focused = HitTester::getTopElementAtPos(dom, mouse_pos);
            if(focused){
                if(focused->elm_type == ButtonElement){
                    focused->to<Button>()->signal_clicked().emit();
                }
                focused->focused = true;
                if(last_focused){
                    last_focused->focused = false;
                }
                if(focused->elm_type == InputElement){
                    focused->to<Input>()->setCaretPosByHitTest({mouse_pos.x - focused->getWin32Rc().left, mouse_pos.y });
                }
                InvalidateRect(m_hwnd, NULL, FALSE);
                last_focused = focused;
            }
            
        }break;
        case WM_CHAR:{
            if(last_focused){
                if(last_focused->elm_type == InputElement){
                    Input* input = last_focused->to<Input>();
                    if(wp != VK_BACK){
                        input->insertChar((wchar_t)wp);
                    } else {
                        input->eraseChar();
                    }
                    InvalidateRect(m_hwnd, NULL, FALSE);
                }
            }
        }
            break;
        case WM_KEYDOWN:
        {
            if(last_focused){
                if(last_focused->elm_type == InputElement){
                    Input* input = last_focused->to<Input>();
                    if(wp == VK_RIGHT){
                        if(input->caret_pos < input->str.size())
                            input->setCaretPos(input->caret_pos + 1);
                        InvalidateRect(m_hwnd, NULL, FALSE);
                    }
                    else if (wp == VK_LEFT){
                        if(input->caret_pos > 0)
                            input->setCaretPos(input->caret_pos - 1);
                        InvalidateRect(m_hwnd, NULL, FALSE);
                    }
                    
                }
            }
        }
        break;
        case WM_CLOSE:
            PostQuitMessage(0);
        default: return DefWindowProcA(m_hwnd, msg, wp, lp);
    }
    return DefWindowProcA(m_hwnd, msg, wp, lp);
};