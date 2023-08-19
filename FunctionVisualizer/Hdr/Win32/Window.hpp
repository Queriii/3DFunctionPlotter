#pragma once

#include "../Data Structures/List.hpp"

#include <Windows.h>




typedef struct GraphFunction
{
    enum Function 
    {
        SingleVariable,
        TwoVariable
    };

    bool bFirstInitialization;      //Used for proper memory cleanup.

    GraphFunction::Function FunctionType;
    bool                    bShowFunction;
    PCTSTR                  ptszInfixFunction;
    List<PTSTR>             PostfixTokens;
}GraphFunction;

typedef struct GraphOptions
{
    UINT    uiRangeXAxis, uiRangeYAxis, uiRangeZAxis;
    bool    bFreecamMovement;
    UINT    uiCameraSpeed;
    UINT    uiCameraSensitivity; 
    bool    bShowCartesianAxis;
    bool    bShowCartesianGrid;
}GraphOptions;



typedef bool(*_ExitSizeMove)(int cxClientWidth, int cyClientHeight);
typedef bool(*_RenderFrame)();
typedef bool(*_UpdateFragments)();
typedef bool(*_UpdateCameraSettings)();
typedef void(*_UpdateCameraOrientation)(float fDeltaPitch, float fDeltaYaw, float fDeltaRoll, bool bDegrees);



class Window
{
public:
    static bool RegisterWindowClass(HINSTANCE hInstance, HICON hIcon, HCURSOR hCursor, PCTSTR pctszMenuName, PCTSTR pctszClassName);
    static bool RegisterWindow(PCTSTR pctszWindowTitle, int cxWindowWidth, int cyWindowHeight);
    static bool RegisterCallbacks(_ExitSizeMove pExitSizeMove, _RenderFrame pRenderFrame, _UpdateFragments pUpdateFragments, _UpdateCameraSettings pUpdateCameraSettings, _UpdateCameraOrientation pUpdateCameraOrientation);

    static WPARAM WindowLoop();

    static WNDCLASS         GetWindowClass();
    static HWND             GetWindow();
    static int              GetClientWidth();
    static int              GetClientHeight();
    static GraphFunction    GetGraphFunctionConfig();
    static GraphOptions     GetGraphOptionsConfig();

private:
    static WNDCLASS wcWindowClass;
    static HWND     hwndWindow;
    
    static POINT ClientDimensions;

    static _ExitSizeMove            pExitSizeMove;
    static _RenderFrame             pRenderFrame;
    static _UpdateFragments         pUpdateFragments;
    static _UpdateCameraSettings    pUpdateCameraSettings;
    static _UpdateCameraOrientation pUpdateCameraOrientation;

    static double dTimeToRenderPreviousFrame;

    static GraphFunction    GraphFunctionConfig;
    static GraphOptions     GraphOptionsConfig;

    static LRESULT __stdcall Win32MessageHandler(HWND hwndWindow, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    static INT_PTR __stdcall GraphFunctionDlgMessageHandler(HWND hwndDlg, UINT UiMessage, WPARAM wParam, LPARAM lParam);
    static INT_PTR __stdcall GraphOptionsDlgMessageHandler(HWND hwndDlg, UINT uiMessage, WPARAM wParam, LPARAM lParam);
};