#pragma once

#include <Windows.h>
#include <profileapi.h>



class Timer
{
public:
    static void     Start();
    static void     Pause();
    static void     Resume();
    static double   ElapsedTime();

    static void     SetPrevFrameRenderTime(double dTime);
    static double   GetPrevFrameRenderTime();

private:
    static LARGE_INTEGER liCountsPerSecond;

    static LARGE_INTEGER   liTimerStart; 
    static LARGE_INTEGER   liPauseStart;
    static LARGE_INTEGER   liTimePaused; 
    static bool            bPaused;

    static double dTimeToRenderPreviousFrame;
};