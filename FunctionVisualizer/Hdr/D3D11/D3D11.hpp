#pragma once

#include "D3D11Fragment.hpp"
#include "../Data Structures/List.hpp"
#include "../Win32/UpdateTypes.hpp"

#include <d3d11.h>
#include <wrl/client.h>



class D3D11
{
public:
    static bool InitializeD3D11(HWND hwndWindow, int cxClientWidth, int cyClientHeight);    
    static bool ResizeSwapChainBuffers(int cxClientWidth, int cyClientHeight);

    static bool RegisterFragment(D3D11Fragment* pFragment);

    static bool UpdateFragments(UpdateTypes Type);

    static bool ProcessKeyboard();
    static bool RenderFrame();

    static ID3D11Device*            GetD3D11Device();
    static ID3D11DeviceContext*     GetD3D11DeviceContext();
    static IDXGISwapChain*          GetSwapChain();
    static ID3D11RenderTargetView*  GetSCBackBufferRTV();
    static ID3D11DepthStencilView*  GetSCDepthStencilBufferDSV();

    static void Cleanup();

private:
    static Microsoft::WRL::ComPtr<ID3D11Device>        cpDevice;
    static Microsoft::WRL::ComPtr<ID3D11DeviceContext> cpDeviceContext;
    static Microsoft::WRL::ComPtr<IDXGISwapChain>      cpSwapChain;

    static Microsoft::WRL::ComPtr<ID3D11Texture2D>          cpSCBackBuffer, cpSCDepthStencilBuffer;
    static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   cpSCBackBufferRTV;
    static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   cpSCDepthStencilBufferDSV;

    static List<D3D11Fragment*> Fragments;
};