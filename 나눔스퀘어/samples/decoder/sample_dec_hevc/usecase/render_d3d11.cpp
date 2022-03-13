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

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <wrl.h>

#include <mcfourcc.h>
#include "dec_hevc.h"
#include "../d3d_base.h"

using namespace DirectX;
using namespace Microsoft::WRL;

struct VertexInput
{
    XMFLOAT3 pos;
    XMFLOAT2 tex;
};

class MultiThreading : public ComPtr<ID3D10Multithread>
{
public:
    MultiThreading(ID3D10Multithread* multithreading) : ComPtr<ID3D10Multithread>(multithreading) {}
    MultiThreading() : ComPtr<ID3D10Multithread>() {}
    void lock() { (*this)->Enter(); }
    void unlock() { (*this)->Leave(); }
    ~MultiThreading() = default;
};

class D3d11Render
{
public:
    bool createDevice(HWND g_window);
    bool render(const hevc_picture_t* picture);

    bool setupDecoder(bufstream_tt* decoder)
    {
        return BS_OK == decoder->auxinfo(decoder, HEVCVD_DECODING_TOOLSET_D3D11, HWACC_SET_ACC_MODE, 0, 0) &&
               BS_OK == decoder->auxinfo(decoder, 0, SET_HW_ADAPTER, device.Get(), 0);
    }

protected:
    bool updateOutputSurface(uint32_t fourcc, uint32_t width, uint32_t height);
    static bool compileShaderFromFile(WCHAR* filename, const char* entry_point, const char* shader_model, ID3DBlob** blob_out);
    ComPtr<ID3D11Device> device;

    ComPtr<ID3D11DeviceContext> immediate_context;
    ComPtr<IDXGISwapChain> swap_chain;
    MultiThreading multithreading;

    ComPtr<ID3D11RenderTargetView> render_target_view;
    ComPtr<ID3D11VertexShader> vertex_shader;
    ComPtr<ID3D11PixelShader> pixel_shader;
    ComPtr<ID3D11InputLayout> vertex_layout;
    ComPtr<ID3D11Buffer> vertex_buffer;
    ComPtr<ID3D11Texture2D> input_texture;
    ComPtr<ID3D11ShaderResourceView> chrominance_view;
    ComPtr<ID3D11ShaderResourceView> luminance_view;
    ComPtr<ID3D11SamplerState> point_sampler;
    HWND handle;

    int vertex_count;
};

bool D3d11Render::createDevice(HWND handle)
{
    this->handle = handle;
    WCHAR shader_filename[512];
    GetModuleFileNameW(nullptr, shader_filename, 512);

    std::wstring temp_buf = shader_filename;
    size_t pos = temp_buf.find_last_of(L'\\');
    temp_buf.resize(pos);
    swprintf(shader_filename, 512, L"%s\\d3d11.hlsl", temp_buf.c_str());

    const static D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };

    DXGI_SWAP_CHAIN_DESC sd{};

    sd.BufferCount = 1;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = handle;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, &sd,
        &swap_chain, &device, 0, &immediate_context);
    if (FAILED(hr))
        return false;

    // Compile the vertex shader
    ComPtr<ID3DBlob> vs_blob;
    if (!compileShaderFromFile(shader_filename, "VS", "vs_4_0_level_9_1", &vs_blob))
        return false;

    // Create the vertex shader
    hr = device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &vertex_shader);
    if (FAILED(hr))
        return false;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexInput, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexInput, tex), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT num_elements = sizeof(layout) / sizeof(layout[0]);

    // Create the input layout
    hr = device->CreateInputLayout(layout, num_elements, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &vertex_layout);
    if (FAILED(hr))
        return false;

    // Set the input layout
    immediate_context->IASetInputLayout(vertex_layout.Get());

    // Compile the pixel shader
    ComPtr<ID3DBlob> ps_blob;
    if (!compileShaderFromFile(shader_filename, "PS", "ps_4_0_level_9_1", &ps_blob))
        return false;

    // Create the pixel shader
    hr = device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), NULL, &pixel_shader);
    if (FAILED(hr))
        return false;

    VertexInput vertices[] = {
        { XMFLOAT3(-1.f, 1.f, 0.f), XMFLOAT2(0.f, 0.f) },
        { XMFLOAT3(1.f, 1.f, 0.f), XMFLOAT2(1.f, 0.f) },
        { XMFLOAT3(1.f, -1.f, 0.f), XMFLOAT2(1.f, 1.f) },

        { XMFLOAT3(1.f, -1.f, 0.f), XMFLOAT2(1.f, 1.f) },
        { XMFLOAT3(-1.f, -1.f, 0.f), XMFLOAT2(0.f, 1.f) },
        { XMFLOAT3(-1.f, 1.f, 0.f), XMFLOAT2(0.f, 0.f) },
    };
    vertex_count = sizeof(vertices) / sizeof(vertices[0]);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA res_data{ vertices, 0, 0 };
    // Create vertex buffer
    hr = device->CreateBuffer(&bd, &res_data, &vertex_buffer);
    if (FAILED(hr))
        return false;

    // Set vertex buffer
    UINT stride = sizeof(VertexInput);
    UINT offset = 0;
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // Set primitive topology
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    device.As(&multithreading);
    multithreading->SetMultithreadProtected(true);

    CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
    hr = device->CreateSamplerState(&desc, &point_sampler);
    if (FAILED(hr))
        return false;

    immediate_context->PSSetSamplers(0, 1, point_sampler.GetAddressOf());
    immediate_context->VSSetShader(vertex_shader.Get(), NULL, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), NULL, 0);

    return true;
}

bool D3d11Render::render(const hevc_picture_t* pic)
{
    std::lock_guard<MultiThreading> lock(multithreading);

    if (!updateOutputSurface(pic->fourcc, pic->width, pic->height))
        return false;

    ComPtr<IDXGIKeyedMutex> mutex;
    pic->d3d11_info->texture->QueryInterface(__uuidof(IDXGIKeyedMutex), &mutex);
    if (mutex)
        mutex->AcquireSync(0, INFINITE);
    const D3D11_BOX box = { 0, 0, 0, pic->width, pic->height, 1 };
    immediate_context->CopySubresourceRegion(input_texture.Get(), 0, 0, 0, 0, pic->d3d11_info->texture, pic->d3d11_info->subresource_id, &box);
    if (mutex)
        mutex->ReleaseSync(0);
    immediate_context->Draw(vertex_count, 0);

    // Present the information rendered to the back buffer to the front buffer (the screen)
    swap_chain->Present(1, 0);
    return true;
}

bool D3d11Render::updateOutputSurface(uint32_t fourcc, uint32_t width, uint32_t height)
{
    if (input_texture.Get()) {
        D3D11_TEXTURE2D_DESC desc;
        input_texture->GetDesc(&desc);

        uint32_t old_fourcc = 0;
        if (desc.Format == DXGI_FORMAT_NV12)
            old_fourcc = FOURCC_NV12; // video is 8 bit
        else if (desc.Format == DXGI_FORMAT_P010)
            old_fourcc = FOURCC_P010; // video is 10 bit

        if (old_fourcc == fourcc && desc.Height == height && desc.Width == width)
            return true;
    }

    DXGI_FORMAT input_format;
    DXGI_FORMAT luminance_format;
    DXGI_FORMAT chrominance_format;

    if (fourcc == FOURCC_NV12) {
        input_format = DXGI_FORMAT_NV12;
        luminance_format = DXGI_FORMAT_R8_UNORM;
        chrominance_format = DXGI_FORMAT_R8G8_UNORM;
    }
    else if (fourcc == FOURCC_P010) {
        input_format = DXGI_FORMAT_P010;
        luminance_format = DXGI_FORMAT_R16_UNORM;
        chrominance_format = DXGI_FORMAT_R16G16_UNORM;
    }
    else {
        printf("Colorspace is not supported\n");
        assert(false);
    }

    CD3D11_TEXTURE2D_DESC texture_desc_in(input_format, // common for video sources
        width,                                          // Width of the video frames
        height,                                         // Height of the video frames
        1,                                              // Number of textures in the array
        1,                                              // Number of miplevels in each texture
        D3D11_BIND_SHADER_RESOURCE,                     // We read from this texture in the shader
        D3D11_USAGE_DYNAMIC,                            // Because we'll be copying from CPU memory
        D3D11_CPU_ACCESS_WRITE                          // We only need to write into the texture
    );

    HRESULT hr = device->CreateTexture2D(&texture_desc_in, 0, &input_texture);
    if (FAILED(hr)) {
        printf("Failed to create surface for color conversion\n");
        return false;
    }

    // Creating a view of the texture to be used when binding it on a shader to sample
    CD3D11_SHADER_RESOURCE_VIEW_DESC luminancePlaneDesc(input_texture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, luminance_format);

    hr = device->CreateShaderResourceView(input_texture.Get(), &luminancePlaneDesc, &luminance_view);
    if (FAILED(hr))
        return false;

    CD3D11_SHADER_RESOURCE_VIEW_DESC chrominancePlaneDesc(input_texture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, chrominance_format);

    hr = device->CreateShaderResourceView(input_texture.Get(), &chrominancePlaneDesc, &chrominance_view);
    if (FAILED(hr))
        return false;

    ID3D11ShaderResourceView* textureViews[] = { luminance_view.Get(), chrominance_view.Get() };

    // Bind the NV12 channels to the shader.
    immediate_context->PSSetShaderResources(0, sizeof(textureViews) / sizeof(textureViews[0]), textureViews);

    // Reinitialization render buffers for new resolution
    render_target_view.Reset();
    immediate_context->OMSetRenderTargets(0, 0, 0);
    immediate_context->Flush();

    RECT rect;
    GetWindowRect(handle, &rect);
    uint32_t max_screen_width = rect.right - rect.left;
    uint32_t max_screen_height = rect.bottom - rect.top;

    width = min(width, max_screen_width);
    height = min(height, max_screen_height);
    hr = swap_chain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
        return false;

    DXGI_MODE_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    hr = swap_chain->ResizeTarget(&desc);

    // Create a render target view
    ComPtr<ID3D11Texture2D> back_buffer;
    hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
    if (FAILED(hr))
        return false;

    hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, &render_target_view);
    if (FAILED(hr))
        return false;

    immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), NULL);

    // Setup the viewport
    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 0.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    immediate_context->RSSetViewports(1, &viewport);

    return true;
}

bool D3d11Render::compileShaderFromFile(WCHAR* filename, const char* entry_point, const char* shader_model, ID3DBlob** blob_out)
{
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> error_blob;
    HRESULT hr = D3DCompileFromFile(filename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, dwShaderFlags, 0, blob_out, &error_blob);
    if (FAILED(hr)) {
        printf("The shader %S cannot be compiled.\n", filename);
        if (error_blob)
            printf("Error blob: %s\n", (char*)error_blob->GetBufferPointer());

        return false;
    }

    return true;
}

class MainContextD3D11 : public MainContext
{
    bool setUpDecoder(bufstream_tt* decoder) override { return m_render.setupDecoder(decoder); }
    bool createDevice(HWND g_window) override { return m_render.createDevice(g_window); };
    bool render(const hevc_picture_t* picture) override { return m_render.render(picture); };

private:
    D3d11Render m_render;
};

int main(int argc, char** argv)
{
#if defined(DEMO_LOGO)
    printf("Error: sample_dec_hevc_d3d11 doesn't work in demo-mode.\n");
    printf("The sample shows how effectively work with d3d11 hardware decoding and rendering.\n");
    printf("It works only in full-mode due to HEVC decoder design restrictions.\n");
    return 0;
#endif

    MainContextD3D11 main_context{};
    return main_context.process(argc, argv);
}
