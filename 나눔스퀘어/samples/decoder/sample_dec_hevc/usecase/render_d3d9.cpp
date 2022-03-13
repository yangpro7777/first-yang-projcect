/**
@brief simple example of how to use the HEVC API

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/
#include <stdio.h>
#include <atomic>
#include <mutex>

#include <windowsx.h>
#include <d3d9.h>
#include <dxva2api.h>
#include <wrl.h>

#include <dec_hevc.h>
#include "../d3d_base.h"

using namespace Microsoft::WRL;

class D3d9Render
{
public:
    bool createDevice(HWND window);
    bool render(const hevc_picture_t* picture);

    bool setupDecoder(bufstream_tt* decoder)
    {
        return BS_OK == decoder->auxinfo(decoder, HEVCVD_DECODING_TOOLSET_D3D9, HWACC_SET_ACC_MODE, 0, 0) &&
               BS_OK == decoder->auxinfo(decoder, 0, SET_HW_ADAPTER, d3d_device_manager.Get(), 0);
    }

private:
    HWND m_window = nullptr;
    ComPtr<IDirect3D9Ex> d3d9;
    ComPtr<IDirect3DDevice9Ex> d3d_device;
    ComPtr<IDirect3DDeviceManager9> d3d_device_manager;
};

bool D3d9Render::render(const hevc_picture_t* picture)
{
    IDirect3DSurface9* surface = picture->surface;

    IDirect3DSurface9* back_surface = nullptr;
    d3d_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_surface);

    if (back_surface) {
        d3d_device->StretchRect(surface, 0, back_surface, 0, D3DTEXF_NONE);
        back_surface->Release();
    }

    while (d3d_device->PresentEx(NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT) == D3DERR_WASSTILLDRAWING)
        ;

    return true;
}

bool D3d9Render::createDevice(HWND window)
{
    m_window = window;
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9)))
        return false;

    D3DPRESENT_PARAMETERS d3dpp{};
    d3dpp.BackBufferWidth = 0;
    d3dpp.BackBufferHeight = 0;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = m_window;
    d3dpp.Windowed = TRUE;
    d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (FAILED(d3d9->CreateDeviceEx(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, NULL, &d3d_device)))
        return false;

    UINT n_reset_token = 0;
    DXVA2CreateDirect3DDeviceManager9(&n_reset_token, &d3d_device_manager);
    if (d3d_device_manager)
        return SUCCEEDED(d3d_device_manager->ResetDevice(d3d_device.Get(), n_reset_token));

    return false;
}

class MainContextD3D9 : public MainContext
{
    bool setUpDecoder(bufstream_tt* decoder) override { return m_render.setupDecoder(decoder); }
    bool createDevice(HWND m_window) override { return m_render.createDevice(m_window); };
    bool render(const hevc_picture_t* picture) override { return m_render.render(picture); };

private:
    D3d9Render m_render;
};

int main(int argc, char** argv)
{
#if defined(DEMO_LOGO)
    printf("Error: sample_dec_hevc_d3d9 doesn't work in demo-mode.\n");
    printf("The sample shows how effectively work with d3d9 hardware decoding and rendering.\n");
    printf("It works only in full-mode due to HEVC decoder design restrictions.\n");
    return 0;
#endif

    MainContextD3D9 main_context{};
    return main_context.process(argc, argv);
}
