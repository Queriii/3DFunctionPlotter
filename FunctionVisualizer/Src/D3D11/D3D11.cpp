#include "../../Hdr/D3D11/D3D11.hpp"
#include "../../Hdr/D3D11/D3D11Camera.hpp"
#include "../../Hdr/Win32/Timer.hpp"
#include "../../Hdr/Exceptions/Exceptions.hpp"

#include <d3dcompiler.h>
#include <cassert>




#ifdef  _DEBUG
#define SWAP_CHAIN_CREATION_FLAGS D3D11_CREATE_DEVICE_DEBUG
#else
#define SWAP_CHAIN_CREATION_FLAGS NULL
#endif




//Static Inits
Microsoft::WRL::ComPtr<ID3D11Device>        D3D11::cpDevice                         = nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3D11::cpDeviceContext                  = nullptr;
Microsoft::WRL::ComPtr<IDXGISwapChain>      D3D11::cpSwapChain                      = nullptr;

Microsoft::WRL::ComPtr<ID3D11Texture2D>         D3D11::cpSCBackBuffer               = nullptr;
Microsoft::WRL::ComPtr<ID3D11Texture2D>         D3D11::cpSCDepthStencilBuffer       = nullptr;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  D3D11::cpSCBackBufferRTV            = nullptr;
Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  D3D11::cpSCDepthStencilBufferDSV    = nullptr;

List<D3D11Fragment*> D3D11::Fragments;




//Public
bool D3D11::InitializeD3D11(HWND hwndWindow, int cxClientWidth, int cyClientHeight)
{
    assert(cxClientWidth != 0 && cyClientHeight != 0);



    D3D11Camera::InitializeDefault();



    DXGI_SWAP_CHAIN_DESC SwapChainDesc =
    {
        {
            static_cast<UINT>(cxClientWidth),
            static_cast<UINT>(cyClientHeight),
            {144, 1},
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
            DXGI_MODE_SCALING_UNSPECIFIED
        },
        {
            1,
            0
        },
        DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER,
        1,
        hwndWindow,
        TRUE,
        DXGI_SWAP_EFFECT_DISCARD,
        NULL
    };
    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, SWAP_CHAIN_CREATION_FLAGS, nullptr, NULL, D3D11_SDK_VERSION, &SwapChainDesc, D3D11::cpSwapChain.GetAddressOf(), D3D11::cpDevice.GetAddressOf(), nullptr, D3D11::cpDeviceContext.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_D3D11CreateDeviceAndSwapChain);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    if (FAILED(D3D11::cpSwapChain->GetBuffer(0, __uuidof(D3D11::cpSCBackBuffer), reinterpret_cast<void**>(D3D11::cpSCBackBuffer.GetAddressOf()))))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_GetBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_RENDER_TARGET_VIEW_DESC SCBackBufferRTVDesc   = {};
    SCBackBufferRTVDesc.Format                          = SwapChainDesc.BufferDesc.Format;
    SCBackBufferRTVDesc.ViewDimension                   = D3D11_RTV_DIMENSION_TEXTURE2D;
    SCBackBufferRTVDesc.Texture2D.MipSlice              = 0;
    if (FAILED(D3D11::cpDevice->CreateRenderTargetView(D3D11::cpSCBackBuffer.Get(), &SCBackBufferRTVDesc, D3D11::cpSCBackBufferRTV.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateRenderTargetView); 
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
        return false;
    }


     
    D3D11_TEXTURE2D_DESC SCDepthStencilBufferDesc = 
    {
        static_cast<UINT>(SwapChainDesc.BufferDesc.Width),
        static_cast<UINT>(SwapChainDesc.BufferDesc.Height),
        1,
        1,
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        SwapChainDesc.SampleDesc,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_DEPTH_STENCIL, 
        NULL,
        NULL
    };
    if (FAILED(D3D11::cpDevice->CreateTexture2D(&SCDepthStencilBufferDesc, nullptr, D3D11::cpSCDepthStencilBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateTexture2D);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC SCDepthStencilBufferDSVDesc   = {};
    SCDepthStencilBufferDSVDesc.Format                          = SCDepthStencilBufferDesc.Format;
    SCDepthStencilBufferDSVDesc.ViewDimension                   = D3D11_DSV_DIMENSION_TEXTURE2D;
    SCDepthStencilBufferDSVDesc.Flags                           = NULL;
    SCDepthStencilBufferDSVDesc.Texture2D.MipSlice              = 0;
    if (FAILED(D3D11::cpDevice->CreateDepthStencilView(D3D11::cpSCDepthStencilBuffer.Get(), &SCDepthStencilBufferDSVDesc, D3D11::cpSCDepthStencilBufferDSV.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateDepthStencilView);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    return true;
}

bool D3D11::ResizeSwapChainBuffers(int cxClientWidth, int cyClientHeight)
{
    assert(cxClientWidth != 0 && cyClientHeight != 0 && D3D11::cpSwapChain != nullptr);

    D3D11::cpDeviceContext->OMSetRenderTargets(NULL, nullptr, nullptr);
    D3D11::cpSCBackBufferRTV.Reset();
    D3D11::cpSCDepthStencilBufferDSV.Reset();
    D3D11::cpSCBackBuffer.Reset();
    D3D11::cpSCDepthStencilBuffer.Reset();

    if (FAILED(D3D11::cpSwapChain->ResizeBuffers(1, cxClientWidth, cyClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, NULL)))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_ResizeBuffers);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    if (FAILED(D3D11::cpSwapChain->GetBuffer(0, __uuidof(D3D11::cpSCBackBuffer), reinterpret_cast<void**>(D3D11::cpSCBackBuffer.GetAddressOf()))))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_GetBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_RENDER_TARGET_VIEW_DESC SCBackBufferRTVDesc = {};
    SCBackBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SCBackBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    SCBackBufferRTVDesc.Texture2D.MipSlice = 0;
    if (FAILED(D3D11::cpDevice->CreateRenderTargetView(D3D11::cpSCBackBuffer.Get(), &SCBackBufferRTVDesc, D3D11::cpSCBackBufferRTV.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateRenderTargetView);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    D3D11_TEXTURE2D_DESC SCDepthStencilBufferDesc =
    {
        static_cast<UINT>(cxClientWidth),
        static_cast<UINT>(cyClientHeight),
        1,
        1,
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        {1, 0},
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_DEPTH_STENCIL, 
        NULL,
        NULL
    };
    if (FAILED(D3D11::cpDevice->CreateTexture2D(&SCDepthStencilBufferDesc, nullptr, D3D11::cpSCDepthStencilBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateTexture2D);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC SCDepthStencilBufferDSVDesc   = {};
    SCDepthStencilBufferDSVDesc.Format                          = SCDepthStencilBufferDesc.Format;
    SCDepthStencilBufferDSVDesc.ViewDimension                   = D3D11_DSV_DIMENSION_TEXTURE2D;
    SCDepthStencilBufferDSVDesc.Flags                           = NULL;
    SCDepthStencilBufferDSVDesc.Texture2D.MipSlice              = 0;
    if (FAILED(D3D11::cpDevice->CreateDepthStencilView(D3D11::cpSCDepthStencilBuffer.Get(), &SCDepthStencilBufferDSVDesc, D3D11::cpSCDepthStencilBufferDSV.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateDepthStencilView);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    return true;
}

bool D3D11::RegisterFragment(D3D11Fragment* pFragment)
{
    assert(pFragment != nullptr);

    if (!pFragment->InitializeFragment())
    {
        return false;
    }

    D3D11::Fragments.Append(static_cast<D3D11Fragment*>(pFragment));
    return true;
}

bool D3D11::UpdateFragments(UpdateTypes Type)
{
    for (UINT i = 0; i < D3D11::Fragments.Size(); i++)
    {
        switch (Type)
        {

        case UpdateTypes::GraphOptionsUpdate:
        {
            if (!D3D11::Fragments[i]->GraphOptionsUpdated())
            {
                return false;
            }
            break;
        }

        case UpdateTypes::GraphFunctionUpdate:
        {
            if (!D3D11::Fragments[i]->GraphFunctionUpdated())
            {
                return false;
            }
            break;
        }

        }

    }

    return true;
}

bool D3D11::ProcessKeyboard()
{
    if (GetAsyncKeyState('W') & 0x8000)
    {
        D3D11Camera::OffsetCameraPosition(0.0f, 0.0f, 1.0f * static_cast<float>(Timer::GetPrevFrameRenderTime()));
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        D3D11Camera::OffsetCameraPosition(0.0f, 0.0f, -1.0f * static_cast<float>(Timer::GetPrevFrameRenderTime()));
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        D3D11Camera::OffsetCameraPosition(1.0f * static_cast<float>(Timer::GetPrevFrameRenderTime()), 0.0f, 0.0f);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        D3D11Camera::OffsetCameraPosition(-1.0f * static_cast<float>(Timer::GetPrevFrameRenderTime()), 0.0f, 0.0f);
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        D3D11Camera::OffsetCameraPosition(0.0f, 1.0f * static_cast<float>(Timer::GetPrevFrameRenderTime()), 0.0f);
    }
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
    {
        D3D11Camera::OffsetCameraPosition(0.0f, -1.0f * static_cast<float>(Timer::GetPrevFrameRenderTime()), 0.0f);
    }

    return true;
}

bool D3D11::RenderFrame()
{
    D3D11::ProcessKeyboard();       //Can't return false...


    float BkColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    D3D11::cpDeviceContext->ClearRenderTargetView(D3D11::cpSCBackBufferRTV.Get(), BkColor);
    D3D11::cpDeviceContext->ClearDepthStencilView(D3D11::cpSCDepthStencilBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, NULL);

    for (UINT i = 0; i < D3D11::Fragments.Size(); i++)
    {
        if (D3D11::Fragments[i]->ShouldDraw())
        {
            if (!D3D11::Fragments[i]->DrawFragment())
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_DrawFragment);
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
                return false;
            }
        }
    }

    D3D11::cpSwapChain->Present(1, 0);

    return true;
}

ID3D11Device* D3D11::GetD3D11Device()
{
    assert(D3D11::cpDevice != nullptr && D3D11::cpDevice.Get() != nullptr);
    return (D3D11::cpDevice.Get());
}

ID3D11DeviceContext* D3D11::GetD3D11DeviceContext()
{
    assert(D3D11::cpDeviceContext != nullptr && D3D11::cpDeviceContext.Get() != nullptr);
    return (D3D11::cpDeviceContext.Get());
}

IDXGISwapChain* D3D11::GetSwapChain()
{
    assert(D3D11::cpSwapChain != nullptr && D3D11::cpSwapChain.Get() != nullptr);
    return (D3D11::cpSwapChain.Get());
}

ID3D11RenderTargetView* D3D11::GetSCBackBufferRTV()
{
    assert(D3D11::cpSCBackBufferRTV != nullptr && D3D11::cpSCBackBuffer.Get() != nullptr);
    return (D3D11::cpSCBackBufferRTV.Get());
}

ID3D11DepthStencilView* D3D11::GetSCDepthStencilBufferDSV()
{
    assert(D3D11::cpSCDepthStencilBufferDSV != nullptr && D3D11::cpSCDepthStencilBufferDSV.Get() != nullptr);
    return (D3D11::cpSCDepthStencilBufferDSV.Get());
}

void D3D11::Cleanup()
{
    for (UINT i = 0; i < D3D11::Fragments.Size(); i++)
    {
        if (D3D11::Fragments[i])
        {
            delete D3D11::Fragments[i];
        }
    }
}