#include"Windowing/MyWindow.h"
#include <winuser.h>

#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "WindowsCodecs.lib")

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT){
    MyWindow window;
    window.createWindow(L"hello", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        50, 50, 500, 500, NULL, NULL);

    MSG msg = { 0 };
    while(GetMessage(&msg, NULL, NULL, NULL)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}