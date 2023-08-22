#ifdef  UNICODE
#define WIN_MAIN wWinMain
#else
#define WIN_MAIN WinMain
#endif



#include "../Hdr/Win32/Window.hpp"
#include "../Hdr/D3D11/D3D11.hpp"
#include "../Hdr/D3D11/Fragments/CartesianAxis.hpp"
#include "../Hdr/D3D11/Fragments/CartesianGrid.hpp"
#include "../Hdr/D3D11/Fragments/TwoVarFunction.hpp"
#include "../Hdr/D3D11/D3D11Camera.hpp"
#include "../Hdr/Exceptions/Exceptions.hpp"



int WINAPI WIN_MAIN(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrev, _In_ PTSTR ptszCommandArgs, _In_ int iWindowState)
{
    try
    {
        if (!Window::RegisterWindowClass(hInstance, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), __TEXT("FunctionVisualizer_Menu"), __TEXT("FunctionVisualizer_Main")))
        {
            throw Exception_Win32Initialization();
        }
        if (!Window::RegisterCallbacks(D3D11::ResizeSwapChainBuffers, D3D11::RenderFrame, D3D11::UpdateFragments, D3D11Camera::CameraSettingsUpdated, D3D11Camera::OffsetCameraOrientation))
        {
            throw Exception_Win32Initialization();
        }
        if (!Window::RegisterWindow(__TEXT("3D-Plotter"), 600, 600))
        {
            throw Exception_Win32Initialization();
        }

        if (!D3D11::InitializeD3D11(Window::GetWindow(), Window::GetClientWidth(), Window::GetClientHeight()))
        {
            throw Exception_D3D11Initialization(); 
        }

        D3D11Fragment* pCartesianAxis = new CartesianAxis;
        if (!pCartesianAxis || !D3D11::RegisterFragment(pCartesianAxis))
        {
            if (!pCartesianAxis)
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_New);
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            }

            throw Exception_D3D11FragmentRegistration();
        }

        D3D11Fragment* pCartesianGrid = new CartesianGrid;
        if (!pCartesianGrid || !D3D11::RegisterFragment(pCartesianGrid))
        {
            if (!pCartesianGrid)
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_New); 
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            }

            throw Exception_D3D11FragmentRegistration();
        }

        D3D11Fragment* pTwoVarFunction = new TwoVarFunction;
        if (!pTwoVarFunction || !D3D11::RegisterFragment(pTwoVarFunction))
        {
            if (!pTwoVarFunction)
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_New);
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            }

            throw Exception_D3D11FragmentRegistration();
        }
    }
    catch (GenericException& Exception)
    {
        AdditionalExceptionInformation::Handler(Exception);
        return -1;
    }


    int Ret = static_cast<int>(Window::WindowLoop());

    D3D11::Cleanup();
    Window::Cleanup();

    return 0;
}