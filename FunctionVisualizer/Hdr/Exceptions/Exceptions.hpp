#pragma once

#include <exception>




class GenericException : public std::exception
{
public:
    virtual const char* what() const noexcept; 
};



class Exception_Win32Initialization : public GenericException
{
public: 
    virtual const char* what() const noexcept;
};

class Exception_D3D11Initialization : public GenericException
{
public:
    virtual const char* what() const noexcept; 
};

class Exception_D3D11FragmentRegistration : public GenericException
{
public:
    virtual const char* what() const noexcept;
};

class Exception_D3D11FragmentUpdate : public GenericException 
{
public: 
    virtual const char* what() const noexcept;
};

class Exception_D3D11CameraUpdate : public GenericException
{
public:
    virtual const char* what() const noexcept;
};

class Exception_D3D11RenderFrame : public GenericException
{
public:
    virtual const char* what() const noexcept;
};

class Exception_D3D11Timer : public GenericException
{
public:
    virtual const char* what() const noexcept;
};

class Exception_ClientResizing : public GenericException
{
public:
    virtual const char* what() const noexcept;
};

class Exception_RawInputDevices : public GenericException
{
public:
    virtual const char* what() const noexcept;
};

class Exception_GraphOptions : public GenericException
{
public:
    virtual const char* what() const noexcept;
};

class Exception_GraphFunction : public GenericException
{
public:
    virtual const char* what() const noexcept;
};



class AdditionalExceptionInformation
{
public:
    enum AdditionalExceptionInformationIndices
    {
        _None,
        _RegisterClass,
        _CreateWindow,
        _GetClientRect,
        _D3DReadFileToBlob,
        _CreateInputLayout,
        _CreateBuffer,
        _CreateVertexShader,
        _CreatePixelShader,
        _New,
        _D3D11CreateDeviceAndSwapChain,
        _GetBuffer,
        _CreateRenderTargetView,
        _CreateTexture2D,
        _CreateDepthStencilView,
        _QueryPerformanceFrequency,
        _QueryPerformanceCounter,
        _DrawFragment,
        _ResizeBuffers,
        _RegisterRawInputDevices,
        _GetRawInputData,
        _Map,
        _CheckRadioButtons,
        _CreateHullShader,
        _CreateDomainShader,
        _CreateSamplerState,
        _CreateShaderResourceView,
        _CreateRasterizerState,
        _CreateBlendState,
    };
    static constexpr const char* AdditionalExceptionInformationTable[] = 
    {
        ("No additional information was reported (not good)."),
        ("Failed to register window class."),
        ("Failed to create window."),
        ("Failed to retrieve the window client dimensions."),
        ("Failed to read shader file into memory (ensure that you don't move executable outside of it's download directory)."),
        ("Failed to create input layout."),
        ("Failed to create gpu buffer."),
        ("Failed to create vertex shader."),
        ("Failed to create pixel shader."),
        ("Failed to allocate cpu heap memory."),
        ("Failed to create d3d11 device, devicecontext, & swapchain."),
        ("Failed to retrieve the swapchain back buffer."),
        ("Failed to create render target view."),
        ("Failed to create a 2D texture."),
        ("Failed to create depth stencil view."),
        ("Failed to retrieve the frequency of the performance counter."),
        ("Failed to retrieve the current value of the performance counter."),
        ("Failed to draw the current fragment."),
        ("Failed to resize the swapchain back buffer."),
        ("Failed to register/unregister the mouse as a raw input device."),
        ("Failed to retrieve raw input data from the mouse."),
        ("Failed to map gpu to cpu memory."),
        ("Failed to update a selection of radio buttons."),
        ("Failed to create hull shader."),
        ("Failed to create domain shader."),
        ("Failed to create sampler state."),
        ("Failed to create a shader resource view."),
        ("Failed to create rasterizer state."),
        ("Failed to create a blend state."),
    };

    static AdditionalExceptionInformation::AdditionalExceptionInformationIndices    GetLastErrorCode();
    static const char*                                                              GetLastErrorDescr();

    static void SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices Code);
    static void SetErrorLocation(const char* pcszTranslationUnit, unsigned int uiLineNumber);

    static void Handler(GenericException& Exception);

private:
    static AdditionalExceptionInformation::AdditionalExceptionInformationIndices    LastErrorCode;
    static const char*                                                              pcszTranslationUnit;
    static unsigned int                                                             uiLineNumber;
};
