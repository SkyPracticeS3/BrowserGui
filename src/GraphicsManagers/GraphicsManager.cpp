#include "GraphicsManager.h"
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <dcommon.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgiformat.h>
#include <winnt.h>

void GraphicsManager::createDeviceInDependantResources() {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_ID2D1Factory1,
        (void**)&d2d1_factory);
    createAdditionalDeviceInDependantResources();
};
void GraphicsManager::createDeviceDependantResources(HWND hwnd) {
    if(d2d1_device_context) return;

    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_9_1, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1
    };

    D3D_FEATURE_LEVEL selected_level;

    D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
        &d3d11_device, &selected_level, &d3d11_device_context);

    d3d11_device.As(&dxgi_device);
    dxgi_device->GetAdapter(&dxgi_adapter);
    dxgi_adapter->GetParent(IID_IDXGIFactory2, (void**)&dxgi_factory);
    d2d1_factory->CreateDevice(dxgi_device.Get(), &d2d1_device);
    d2d1_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d1_device_context);

    DXGI_SWAP_CHAIN_DESC1 chain_desc = { 0 };
    chain_desc.SampleDesc.Count = 1;
    chain_desc.SampleDesc.Quality = 0;
    chain_desc.BufferCount = 2;
    chain_desc.Flags = 0;
    chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    chain_desc.Width = 0;
    chain_desc.Height = 0;
    chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    chain_desc.Stereo = false;
    chain_desc.Scaling = DXGI_SCALING_NONE;
    chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    dxgi_factory->CreateSwapChainForHwnd(dxgi_device.Get(), hwnd, &chain_desc,
        nullptr, nullptr, &dxgi_swap_chain);

    D2D1_BITMAP_PROPERTIES1 bmp_props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_CANNOT_DRAW |
        D2D1_BITMAP_OPTIONS_TARGET, 
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
    
    dxgi_swap_chain->GetBuffer(0, IID_IDXGISurface, (void**)&dxgi_surface);

    d2d1_device_context->CreateBitmapFromDxgiSurface(dxgi_surface.Get(), bmp_props,
        &target_bmp);
    d2d1_device_context->SetTarget(target_bmp.Get());

    createAdditionalDeviceDependantResources(hwnd);
    
};

void GraphicsManager::resize(HWND hwnd, UINT new_x, UINT new_y) {
    d2d1_device_context->SetTarget(nullptr);
    target_bmp.Reset();
    dxgi_surface.Reset();

    dxgi_swap_chain->ResizeBuffers(2, new_x, new_y, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

    D2D1_BITMAP_PROPERTIES1 bmp_props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_CANNOT_DRAW |
        D2D1_BITMAP_OPTIONS_TARGET, 
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
    
    dxgi_swap_chain->GetBuffer(0, IID_IDXGISurface, (void**)&dxgi_surface);

    d2d1_device_context->CreateBitmapFromDxgiSurface(dxgi_surface.Get(), bmp_props,
        &target_bmp);
    d2d1_device_context->SetTarget(target_bmp.Get());
};