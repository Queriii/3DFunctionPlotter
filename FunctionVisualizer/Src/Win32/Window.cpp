#include "../../Hdr/Win32/Window.hpp"
#include "../../Hdr/Win32/Timer.hpp"
#include "../../Resources/resource.h"
#include "../../Hdr/Data Structures/Parser.hpp"
#include "../../Hdr/Exceptions/Exceptions.hpp"

#include <cassert>
#include <tchar.h>
#include <cmath>
#include <commctrl.h>




//Glbal exception variable 
static std::exception_ptr WndProcException = nullptr;




//Static Inits
WNDCLASS    Window::wcWindowClass       = {};
HWND        Window::hwndWindow          = nullptr;

POINT       Window::ClientDimensions    = {};

_ExitSizeMove               Window::pExitSizeMove               = nullptr;
_RenderFrame                Window::pRenderFrame                = nullptr;
_UpdateFragments            Window::pUpdateFragments            = nullptr;
_UpdateCameraSettings       Window::pUpdateCameraSettings       = nullptr;
_UpdateCameraOrientation    Window::pUpdateCameraOrientation    = nullptr;

double Window::dTimeToRenderPreviousFrame = 0.0;

GraphFunction Window::GraphFunctionConfig =
{
    true,

    GraphFunction::Function::TwoVariable,
    false,
    __TEXT("x + z")
};
GraphOptions Window::GraphOptionsConfig =
{
    10, 10, 10,
    false,
    static_cast<UINT>(1),
    static_cast<UINT>(1),
    true,
    false
};




//Public
bool Window::RegisterWindowClass(HINSTANCE hInstance, HICON hIcon, HCURSOR hCursor, PCTSTR pctszMenuName, PCTSTR pctszClassName)
{
    assert(hInstance != nullptr && hIcon != nullptr && hCursor != nullptr && pctszMenuName != nullptr && pctszClassName != nullptr);

    Window::wcWindowClass =
    {
        NULL,
        Window::Win32MessageHandler,
        0,
        0,
        hInstance,
        hIcon,
        hCursor,
        reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)),
        pctszMenuName, 
        pctszClassName
    };

    if (!RegisterClass(&(Window::wcWindowClass))) 
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_RegisterClass);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }
    
    return true;
}

bool Window::RegisterWindow(PCTSTR pctszWindowTitle, int cxWindowWidth, int cyWindowHeight)
{
    assert(Window::pExitSizeMove != nullptr && Window::pRenderFrame != nullptr);

    Window::hwndWindow = CreateWindow(Window::wcWindowClass.lpszClassName, pctszWindowTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, cxWindowWidth, cyWindowHeight, nullptr, nullptr, Window::wcWindowClass.hInstance, nullptr);
    if (!Window::hwndWindow)
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateWindow);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    RECT rectClientDimensions;
    if (!GetClientRect(Window::hwndWindow, &rectClientDimensions))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_GetClientRect);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
        return false;
    }

    Window::ClientDimensions.x = rectClientDimensions.right;
    Window::ClientDimensions.y = rectClientDimensions.bottom;

    return true;
}

bool Window::RegisterCallbacks(_ExitSizeMove pExitSizeMove, _RenderFrame pRenderFrame, _UpdateFragments pUpdateFragments, _UpdateCameraSettings pUpdateCameraSettings, _UpdateCameraOrientation pUpdateCameraOrientation)
{
    assert(pExitSizeMove != nullptr && pRenderFrame != nullptr && pUpdateFragments != nullptr && pUpdateCameraSettings != nullptr && pUpdateCameraOrientation != nullptr);

    Window::pExitSizeMove               = pExitSizeMove;
    Window::pRenderFrame                = pRenderFrame;
    Window::pUpdateFragments            = pUpdateFragments;
    Window::pUpdateCameraSettings       = pUpdateCameraSettings;
    Window::pUpdateCameraOrientation    = pUpdateCameraOrientation;

    return true;
}

WPARAM Window::WindowLoop()
{
    try
    {
        MSG Msg;
        for (;;)
        {
            if (WndProcException != nullptr)
            {
                std::rethrow_exception(WndProcException);
            }

            if (PeekMessage(&Msg, nullptr, NULL, NULL, PM_REMOVE))
            {
                if (Msg.message == WM_QUIT)
                {
                    return Msg.wParam;
                }
                else
                {
                    TranslateMessage(&Msg);
                    DispatchMessage(&Msg);
                }
            }
            else
            {
                Timer::Start();
                if (!Window::pRenderFrame())
                {
                    throw Exception_D3D11RenderFrame(); 
                }
                Timer::SetPrevFrameRenderTime(Timer::ElapsedTime()); 
            }
        }
    }
    catch (GenericException& Exception)
    {
        AdditionalExceptionInformation::Handler(Exception);
        return -1;
    }
}

WNDCLASS Window::GetWindowClass() 
{
    return Window::wcWindowClass;
}

HWND Window::GetWindow() 
{
    return Window::hwndWindow;
}

int Window::GetClientWidth()
{
    return Window::ClientDimensions.x;
}

int Window::GetClientHeight()
{
    return Window::ClientDimensions.y;
}

GraphFunction Window::GetGraphFunctionConfig()
{
    return Window::GraphFunctionConfig;
}

GraphOptions Window::GetGraphOptionsConfig()
{
    return Window::GraphOptionsConfig;
}




//Private
LRESULT _stdcall Window::Win32MessageHandler(HWND hwndWindow, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch (uiMessage)
    {

    case WM_GETMINMAXINFO:
    {
        PMINMAXINFO pMinMaxInfo = reinterpret_cast<PMINMAXINFO>(lParam);
        assert(pMinMaxInfo != nullptr);

        pMinMaxInfo->ptMinTrackSize.x = 400;
        pMinMaxInfo->ptMinTrackSize.y = 400;

        return 0;
    }

    case WM_SIZE:
    {
        Window::ClientDimensions.x = LOWORD(lParam);
        Window::ClientDimensions.y = HIWORD(lParam);

        return 0;
    }

    case WM_EXITSIZEMOVE:
    {
        try
        {
            if (!Window::pExitSizeMove(Window::ClientDimensions.x, Window::ClientDimensions.y))
            {
                throw Exception_ClientResizing();
            }
        }
        catch (GenericException& Exception)
        {
            UNREFERENCED_PARAMETER(Exception); 
            WndProcException = std::current_exception();
        }

        return 0;
    }

    case WM_SETFOCUS:
    {
        try
        {
            RAWINPUTDEVICE RidInfo  = {};
            RidInfo.hwndTarget      = hwndWindow;
            RidInfo.dwFlags         = NULL;
            RidInfo.usUsage         = 0x02;
            RidInfo.usUsagePage     = 0x01;
            if (!RegisterRawInputDevices(&RidInfo, 1, sizeof(RAWINPUTDEVICE)))
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_RegisterRawInputDevices);
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
                throw Exception_RawInputDevices();
            }
        }
        catch (GenericException& Exception)
        {
            UNREFERENCED_PARAMETER(Exception);
            WndProcException = std::current_exception();
        }

        return 0;
    }

    case WM_KILLFOCUS:
    {
        try
        {
            RAWINPUTDEVICE RidInfo  = {};
            RidInfo.hwndTarget      = NULL;
            RidInfo.dwFlags         = RIDEV_REMOVE;
            RidInfo.usUsage         = 0x02;
            RidInfo.usUsagePage     = 0x01;
            if (!RegisterRawInputDevices(&RidInfo, 1, sizeof(RAWINPUTDEVICE)))
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_RegisterRawInputDevices);
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
                throw Exception_RawInputDevices();
            }
        }
        catch (GenericException& Exception)
        {
            UNREFERENCED_PARAMETER(Exception);
            WndProcException = std::current_exception();
        }

        return 0;
    }

    case WM_INPUT:
    {
        PRAWINPUT pRawInput = nullptr; 
        try
        {
            UINT uiRidInfoSize = NULL;
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &uiRidInfoSize, sizeof(RAWINPUTHEADER)))
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_GetRawInputData);
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
                throw Exception_RawInputDevices();
            }
            pRawInput = reinterpret_cast<PRAWINPUT>(malloc(uiRidInfoSize));
            if (!pRawInput)
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_New); 
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
                throw Exception_RawInputDevices();
            }
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, pRawInput, &uiRidInfoSize, sizeof(RAWINPUTHEADER)) == -1)
            {
                AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_GetRawInputData);
                AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
                throw Exception_RawInputDevices();
            }

            if (pRawInput->header.dwType == RIM_TYPEMOUSE)
            {
                float fSenseLength = 100.0f;
                Window::pUpdateCameraOrientation(atanf(static_cast<float>(pRawInput->data.mouse.lLastY) / fSenseLength), atanf(static_cast<float>(pRawInput->data.mouse.lLastX) / fSenseLength), 0.0f, false);
            }

            free(pRawInput);
        }
        catch (GenericException& Exception)
        {
            if (pRawInput)
            {
                free(pRawInput);
            }

            UNREFERENCED_PARAMETER(Exception);
            WndProcException = std::current_exception();
        }


        return 0;
    }

    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }



    case WM_COMMAND:
    {
        if (!lParam)
        {
            switch (LOWORD(wParam))
            {
            
            case IDM_GRAPH_FUNCTION:
            {
                try
                {
                    if (static_cast<bool>(DialogBoxParam(reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hwndWindow, GWLP_HINSTANCE)), MAKEINTRESOURCE(IDD_GRAPH_FUNCTION), hwndWindow, Window::GraphFunctionDlgMessageHandler, reinterpret_cast<LPARAM>(&(Window::GraphFunctionConfig)))))
                    {

                    }
                }
                catch (GenericException& Exception)
                {
                    UNREFERENCED_PARAMETER(Exception);
                    WndProcException = std::current_exception();
                }

                break;
            }

            case IDM_GRAPH_OPTIONS:
            {
                try
                {
                    if (static_cast<bool>(DialogBoxParam(reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hwndWindow, GWLP_HINSTANCE)), MAKEINTRESOURCE(IDD_GRAPH_OPTIONS), hwndWindow, Window::GraphOptionsDlgMessageHandler, reinterpret_cast<LPARAM>(&(Window::GraphOptionsConfig)))))
                    {
                        if (!Window::pUpdateFragments())
                        {
                            throw Exception_D3D11FragmentUpdate();
                        }
                        if (!Window::pUpdateCameraSettings())
                        {
                            throw Exception_D3D11CameraUpdate();
                        }
                    }
                }
                catch (GenericException& Exception)
                {
                    UNREFERENCED_PARAMETER(Exception);
                    WndProcException = std::current_exception();
                }

                break;
            }

            default:
            {
                return (DefWindowProc(hwndWindow, uiMessage, wParam, lParam));
            }

            }
        }

        return 0;
    }


    default:
    {
        return (DefWindowProc(hwndWindow, uiMessage, wParam, lParam));
    }

    }
}

INT_PTR _stdcall Window::GraphFunctionDlgMessageHandler(HWND hwndDlg, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    static constexpr int            FunctionTypeRadioButtons[2] = { IDC_RADIO_SINGLEVARIABLE, IDC_RADIO_TWOVARIABLE };
    static GraphFunction::Function  CurrentFunctionType;
    static GraphFunction*           pGraphFunctionConfig;

    switch (uiMessage)
    {
    
    case WM_INITDIALOG:
    {
        pGraphFunctionConfig = reinterpret_cast<GraphFunction*>(lParam);
        assert(pGraphFunctionConfig != nullptr);


        //Set function descriptors
        CurrentFunctionType = pGraphFunctionConfig->FunctionType;
        if (!CheckRadioButton(hwndDlg, IDC_RADIO_SINGLEVARIABLE, IDC_RADIO_TWOVARIABLE, FunctionTypeRadioButtons[pGraphFunctionConfig->FunctionType]))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CheckRadioButtons);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            throw Exception_GraphFunction();
        }
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_DISPLAYFUNCTION), BM_SETCHECK, (pGraphFunctionConfig->bShowFunction) ? BST_CHECKED : BST_UNCHECKED, NULL);


        //Verify function input
        SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_INPUTFUNCTION), pGraphFunctionConfig->ptszInfixFunction);
        if (Parser::IsValidParseTarget(pGraphFunctionConfig->ptszInfixFunction))
        {
            ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDFUNCTIONTYPE), SW_HIDE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_GF_BUTTON_SAVE), TRUE);
        }
        else
        {
            ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDFUNCTIONTYPE), SW_SHOW);
            EnableWindow(GetDlgItem(hwndDlg, IDC_GF_BUTTON_SAVE), FALSE);
        }


        return TRUE;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {

        case IDC_RADIO_SINGLEVARIABLE:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                CurrentFunctionType = GraphFunction::Function::SingleVariable;
            }

            break;
        }

        case IDC_RADIO_TWOVARIABLE:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                CurrentFunctionType = GraphFunction::Function::TwoVariable;
            }
            break;
        }

        case IDC_GF_BUTTON_CANCEL:
        case IDC_GF_BUTTON_SAVE:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                if (LOWORD(wParam) == IDC_GF_BUTTON_SAVE)
                {
                    int     iEditFunctionLength = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_EDIT_INPUTFUNCTION));
                    PTSTR   ptszFunction        = new TCHAR[static_cast<ULONGLONG>(iEditFunctionLength) + 1]{};
                    if (!ptszFunction)
                    {
                        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_New);
                        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
                        throw Exception_GraphFunction();
                    }

                    GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_INPUTFUNCTION), ptszFunction, iEditFunctionLength + 1);

                    assert(pGraphFunctionConfig->ptszInfixFunction != nullptr);
                    if (!pGraphFunctionConfig->bFirstInitialization)
                    {
                        delete[] pGraphFunctionConfig->ptszInfixFunction;
                    }
                    else
                    {
                        pGraphFunctionConfig->bFirstInitialization = false;
                    }
                    pGraphFunctionConfig->ptszInfixFunction = ptszFunction;
                    pGraphFunctionConfig->FunctionType      = CurrentFunctionType;
                    pGraphFunctionConfig->bShowFunction     = (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_DISPLAYFUNCTION), BM_GETCHECK, NULL, NULL) == BST_CHECKED) ? true : false;
                }

                EndDialog(hwndDlg, (LOWORD(wParam) == IDC_GF_BUTTON_SAVE) ? 1 : 0); 
            }

            break;
        }

        case IDC_EDIT_INPUTFUNCTION:
        {
            switch (HIWORD(wParam))
            {

            case EN_CHANGE:
            {
                int     iEditFunctionLength = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_EDIT_INPUTFUNCTION));
                PTSTR   ptszFunction        = new TCHAR[static_cast<ULONGLONG>(iEditFunctionLength) + 1];
                if (!ptszFunction)
                {
                    AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_New);
                    AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
                    throw Exception_GraphFunction();
                }
                GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_INPUTFUNCTION), ptszFunction, iEditFunctionLength + 1);
                
                if (Parser::IsValidParseTarget(ptszFunction))
                {
                    ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDFUNCTIONTYPE), SW_HIDE);
                    EnableWindow(GetDlgItem(hwndDlg, IDC_GF_BUTTON_SAVE), TRUE);
                }
                else
                {
                    ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDFUNCTIONTYPE), SW_SHOW);
                    EnableWindow(GetDlgItem(hwndDlg, IDC_GF_BUTTON_SAVE), FALSE); 
                }

                assert(ptszFunction != nullptr);
                delete[] ptszFunction;

                break;
            }

            default:
            {
                return FALSE;
            }

            }

            break;
        }

        default:
        {
            return FALSE;
        }

        }

        return TRUE;
    }

    case WM_CTLCOLORSTATIC:
    {
        if (reinterpret_cast<HWND>(lParam) == GetDlgItem(hwndDlg, IDC_STATIC_INVALIDFUNCTIONTYPE))
        {
            SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
            SetTextColor(reinterpret_cast<HDC>(wParam), RGB(255, 0, 0));
            return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_MENU));
        }
        else
        {
            return FALSE;
        }
    }

    default:
    {
        return FALSE;
    }

    }
}

INT_PTR _stdcall Window::GraphOptionsDlgMessageHandler(HWND hwndDlg, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    static              GraphOptions*   pGraphOptionsConfig;
    static constexpr    UINT            uiNumDigitsRangeAxis = 20;

    static bool bInvalidXRange, bInvalidYRange, bInvalidZRange;

    switch (uiMessage)
    {
    
    case WM_INITDIALOG:
    {
       pGraphOptionsConfig = reinterpret_cast<GraphOptions*>(lParam);
       assert(pGraphOptionsConfig != nullptr);

       //Setup range settings
       TCHAR ptszRangeAxis[uiNumDigitsRangeAxis]{};
       _itot_s(pGraphOptionsConfig->uiRangeXAxis, ptszRangeAxis, _countof(ptszRangeAxis), 10);
       SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RANGE_XAXIS), ptszRangeAxis);
       if (!Parser::IsValidAxisRange(ptszRangeAxis))
       {
           ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDRANGE), SW_SHOW);
           EnableWindow(GetDlgItem(hwndDlg, IDC_GO_BUTTON_SAVE), FALSE);
       }
       ZeroMemory(ptszRangeAxis, _countof(ptszRangeAxis) * sizeof(TCHAR));


       _itot_s(pGraphOptionsConfig->uiRangeYAxis, ptszRangeAxis, _countof(ptszRangeAxis), 10);
       SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RANGE_YAXIS), ptszRangeAxis);
       if (!Parser::IsValidAxisRange(ptszRangeAxis))
       {
           ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDRANGE), SW_SHOW); 
           EnableWindow(GetDlgItem(hwndDlg, IDC_GO_BUTTON_SAVE), FALSE);
       }
       ZeroMemory(ptszRangeAxis, _countof(ptszRangeAxis) * sizeof(TCHAR));


       _itot_s(pGraphOptionsConfig->uiRangeZAxis, ptszRangeAxis, _countof(ptszRangeAxis), 10);
       SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RANGE_ZAXIS), ptszRangeAxis);
       if (!Parser::IsValidAxisRange(ptszRangeAxis))
       {
           ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDRANGE), SW_SHOW);
           EnableWindow(GetDlgItem(hwndDlg, IDC_GO_BUTTON_SAVE), FALSE);
       }
       ZeroMemory(ptszRangeAxis, _countof(ptszRangeAxis) * sizeof(TCHAR));



       //Setup camera settings
       SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_CAMERA_FREECAM), BM_SETCHECK, (pGraphOptionsConfig->bFreecamMovement) ? BST_CHECKED : BST_UNCHECKED, NULL);

       LPARAM lpSpeedRange = MAKELPARAM(1, 5);
       SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_CAMERA_SPEED), TBM_SETRANGE, FALSE, lpSpeedRange);
       SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_CAMERA_SPEED), TBM_SETPOS, TRUE, static_cast<LPARAM>(pGraphOptionsConfig->uiCameraSpeed));

       LPARAM lpSensRange = MAKELPARAM(1, 5);
       SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_CAMERA_SENSITIVITY), TBM_SETRANGE, FALSE, lpSensRange);
       SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_CAMERA_SENSITIVITY), TBM_SETPOS, TRUE, static_cast<LPARAM>(pGraphOptionsConfig->uiCameraSensitivity)); 



       //Setup visibility settings
       SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWCARTESIANAXIS), BM_SETCHECK, (pGraphOptionsConfig->bShowCartesianAxis) ? BST_CHECKED : BST_UNCHECKED, NULL);
       SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWCARTESIANGRID), BM_SETCHECK, (pGraphOptionsConfig->bShowCartesianGrid) ? BST_CHECKED : BST_UNCHECKED, NULL);

       return TRUE;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {

        case IDC_GO_BUTTON_CANCEL:
        case IDC_GO_BUTTON_SAVE:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                if (LOWORD(wParam) == IDC_GO_BUTTON_SAVE)
                {
                    TCHAR ptszRangeAxis[uiNumDigitsRangeAxis]{};

                    GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RANGE_XAXIS), ptszRangeAxis, _countof(ptszRangeAxis));
                    pGraphOptionsConfig->uiRangeXAxis = static_cast<UINT>(_ttoi(ptszRangeAxis));
                    ZeroMemory(ptszRangeAxis, _countof(ptszRangeAxis) * sizeof(TCHAR));

                    GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RANGE_YAXIS), ptszRangeAxis, _countof(ptszRangeAxis));
                    pGraphOptionsConfig->uiRangeYAxis = static_cast<UINT>(_ttoi(ptszRangeAxis));
                    ZeroMemory(ptszRangeAxis, _countof(ptszRangeAxis) * sizeof(TCHAR));

                    GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RANGE_ZAXIS), ptszRangeAxis, _countof(ptszRangeAxis));
                    pGraphOptionsConfig->uiRangeZAxis = static_cast<UINT>(_ttoi(ptszRangeAxis));
                    ZeroMemory(ptszRangeAxis, _countof(ptszRangeAxis) * sizeof(TCHAR));

                    pGraphOptionsConfig->bFreecamMovement = static_cast<bool>((SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_CAMERA_FREECAM), BM_GETCHECK, NULL, NULL)));
                    pGraphOptionsConfig->uiCameraSpeed = static_cast<UINT>(SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_CAMERA_SPEED), TBM_GETPOS, NULL, NULL));
                    pGraphOptionsConfig->uiCameraSensitivity = static_cast<UINT>(SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_CAMERA_SENSITIVITY), TBM_GETPOS, NULL, NULL));

                    pGraphOptionsConfig->bShowCartesianAxis = (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWCARTESIANAXIS), BM_GETCHECK, NULL, NULL));
                    pGraphOptionsConfig->bShowCartesianGrid = (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWCARTESIANGRID), BM_GETCHECK, NULL, NULL));
                }

                EndDialog(hwndDlg, (LOWORD(wParam) == IDC_GO_BUTTON_SAVE) ? 1 : 0);
            }
            break;
        }

        case IDC_EDIT_RANGE_XAXIS:
        case IDC_EDIT_RANGE_YAXIS:
        case IDC_EDIT_RANGE_ZAXIS:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                TCHAR ptszRangeAxis[uiNumDigitsRangeAxis]{};
                GetWindowText(GetDlgItem(hwndDlg, LOWORD(wParam)), ptszRangeAxis, _countof(ptszRangeAxis));

                if (Parser::IsValidAxisRange(ptszRangeAxis))
                {
                    switch (LOWORD(wParam))
                    {

                    case IDC_EDIT_RANGE_XAXIS:
                    {
                        bInvalidXRange = false;
                        break;
                    }
                        
                    case IDC_EDIT_RANGE_YAXIS:
                    {
                        bInvalidYRange = false;
                        break;
                    }

                    case IDC_EDIT_RANGE_ZAXIS:
                    {
                        bInvalidZRange = false;
                        break;
                    }

                    }

                    if (!bInvalidXRange && !bInvalidYRange && !bInvalidZRange)
                    {
                        ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDRANGE), SW_HIDE);
                        EnableWindow(GetDlgItem(hwndDlg, IDC_GO_BUTTON_SAVE), TRUE);
                    }
                }
                else
                {
                    switch (LOWORD(wParam)) 
                    {

                    case IDC_EDIT_RANGE_XAXIS: 
                    {
                        bInvalidXRange = true; 
                        break;
                    }

                    case IDC_EDIT_RANGE_YAXIS: 
                    {
                        bInvalidYRange = true;
                        break;
                    }

                    case IDC_EDIT_RANGE_ZAXIS: 
                    {
                        bInvalidZRange = true;
                        break;
                    }

                    }

                    ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_INVALIDRANGE), SW_SHOW);
                    EnableWindow(GetDlgItem(hwndDlg, IDC_GO_BUTTON_SAVE), FALSE);
                }
            }

            break;
        }

        default:
        {
            return FALSE;
        }

        }

        return TRUE;
    }


    case WM_CTLCOLORSTATIC:
    {
        if (reinterpret_cast<HWND>(lParam) == GetDlgItem(hwndDlg, IDC_STATIC_INVALIDRANGE))
        {
            SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
            SetTextColor(reinterpret_cast<HDC>(wParam), RGB(255, 0, 0));
            return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_MENU));
        }
        else
        {
            return FALSE;
        }
    }


    default:
    {
        return FALSE;
    }

    }
}