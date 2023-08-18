#include "../../Hdr/Win32/Timer.hpp"
#include "../../Hdr/Exceptions/Exceptions.hpp"




//Static Inits
LARGE_INTEGER Timer::liCountsPerSecond  = {};

LARGE_INTEGER   Timer::liTimerStart     = {};
LARGE_INTEGER   Timer::liPauseStart     = {};
LARGE_INTEGER   Timer::liTimePaused     = {};
bool            Timer::bPaused          = true;

double Timer::dTimeToRenderPreviousFrame = 0.0;




//Public
void Timer::Start()
{
    if (!QueryPerformanceFrequency(&(Timer::liCountsPerSecond)))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_QueryPerformanceFrequency);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        throw Exception_D3D11Timer();
    }

    if (!QueryPerformanceCounter(&(Timer::liTimerStart)))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_QueryPerformanceCounter);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        throw Exception_D3D11Timer();
    }
    Timer::liTimePaused = {};
    Timer::liPauseStart = {};
    Timer::bPaused      = false;
}

void Timer::Pause()
{
    if (Timer::bPaused)
    {
        return;
    }

    if (!QueryPerformanceCounter(&(Timer::liPauseStart)))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_QueryPerformanceCounter);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        throw Exception_D3D11Timer();
    }
    Timer::bPaused = true;
}

void Timer::Resume()
{
    if (!Timer::bPaused)
    {
        return;
    }

    LARGE_INTEGER liUnpauseTime = {};
    if (!QueryPerformanceCounter(&liUnpauseTime))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_QueryPerformanceCounter);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        throw Exception_D3D11Timer();
    }
    Timer::liTimePaused.QuadPart    += liUnpauseTime.QuadPart - Timer::liPauseStart.QuadPart;
    Timer::liPauseStart             = {};
    Timer::bPaused                  = false;
}

double Timer::ElapsedTime()
{
    if (Timer::bPaused)
    {
        return (static_cast<double>(((Timer::liTimePaused.QuadPart - Timer::liTimerStart.QuadPart) - Timer::liTimePaused.QuadPart)) / static_cast<double>(Timer::liCountsPerSecond.QuadPart));
    }
    else
    {
        LARGE_INTEGER liCurrentTime = {};
        if (!QueryPerformanceCounter(&liCurrentTime))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_QueryPerformanceCounter);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            throw Exception_D3D11Timer();
        }
        return (static_cast<double>(((liCurrentTime.QuadPart - Timer::liTimerStart.QuadPart) - Timer::liTimePaused.QuadPart)) / static_cast<double>(Timer::liCountsPerSecond.QuadPart));
    }
}

void Timer::SetPrevFrameRenderTime(double dTime)
{
    Timer::dTimeToRenderPreviousFrame = dTime;
}

double Timer::GetPrevFrameRenderTime()
{
    return (Timer::dTimeToRenderPreviousFrame); 
}