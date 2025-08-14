#pragma once
#include <d3d11.h>
#include <d2d1.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <wrl.h>
#include <d2d1_1.h>

using Microsoft::WRL::ComPtr;

// A Graphics Container For Initializing + Cleaning Up Graphics
class GraphicsManager {
    ComPtr<ID3D11Device> d3d11_device;
    ComPtr<ID3D11DeviceContext> d3d11_device_context;
    ComPtr<IDXGIDevice1> dxgi_device;
    ComPtr<IDXGIAdapter> dxgi_adapter;
    ComPtr<IDXGIFactory2> dxgi_factory;
    ComPtr<ID2D1Device> d2d1_device;
    ComPtr<IDXGISurface> dxgi_surface;

public:
    ComPtr<ID2D1Factory1> d2d1_factory;
    ComPtr<ID2D1Bitmap1> target_bmp;
    ComPtr<ID2D1DeviceContext> d2d1_device_context;
    ComPtr<IDXGISwapChain1> dxgi_swap_chain;

    void createDeviceInDependantResources();
    void createDeviceDependantResources(HWND hwnd);
    void resize(HWND hwnd, UINT new_x, UINT new_y);
    
protected:
    virtual void createAdditionalDeviceInDependantResources() {};
    virtual void createAdditionalDeviceDependantResources(HWND hwnd) {};
};