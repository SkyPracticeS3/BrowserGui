#pragma once
#include "GraphicsManager.h"
#include <d2d1_1.h>
#include <dwrite.h>
#include <unknwnbase.h>

class RendererGraphicsManager : public GraphicsManager {
public:
    ComPtr<IDWriteFactory> d_write_factory;

    void createAdditionalDeviceInDependantResources() override {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            (IUnknown**)&d_write_factory);
    };
    void createAdditionalDeviceDependantResources(HWND hwnd) override {};
};