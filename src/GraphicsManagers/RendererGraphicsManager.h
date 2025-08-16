#pragma once
#include "GraphicsManager.h"
#include <combaseapi.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <unknwnbase.h>
#include <wincodec.h>
#include <winuser.h>
#include <string>
#include <wincodec.h>
#include <wrl/client.h>

class RendererGraphicsManager : public GraphicsManager {
public:
    ComPtr<IDWriteFactory> d_write_factory;
    ComPtr<IWICImagingFactory> wic_factory;

    void createAdditionalDeviceInDependantResources() override {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            (IUnknown**)&d_write_factory);
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_ALL, IID_IWICImagingFactory, (void**)&wic_factory);
        if(FAILED(hr)){
            MessageBoxW(NULL, std::to_wstring(hr).c_str(), L"", MB_OK);
        }
    };
    void createAdditionalDeviceDependantResources(HWND hwnd) override {};
};