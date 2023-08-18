#include "../../Hdr/Exceptions/Exceptions.hpp"

#include <Windows.h>
#include <cassert>
#include <stdio.h>




//Static Inits
AdditionalExceptionInformation::AdditionalExceptionInformationIndices AdditionalExceptionInformation::LastErrorCode = AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_None;
const char*     AdditionalExceptionInformation::pcszTranslationUnit = "Unknown";
unsigned int    AdditionalExceptionInformation::uiLineNumber        = 0;




//Public
const char* GenericException::what() const noexcept
{
    return ("GenericException (Undefined)");
}



const char* Exception_Win32Initialization::what() const noexcept
{
    return ("Exception_Win32Initialization");
}

const char* Exception_D3D11Initialization::what() const noexcept
{
    return ("Exception_D3D11Initialization");
}

const char* Exception_D3D11FragmentRegistration::what() const noexcept
{
    return ("Exception_D3D11FragmentRegistration");
}

const char* Exception_D3D11FragmentUpdate::what() const noexcept
{
    return ("Exception_D3D11FragmentUpdate");
}

const char* Exception_D3D11CameraUpdate::what() const noexcept 
{
    return ("Exception_D3D11CameraUpdate");
}

const char* Exception_D3D11RenderFrame::what() const noexcept 
{
    return ("Exception_D3D11RenderFrame");
}

const char* Exception_D3D11Timer::what() const noexcept
{
    return ("Exception_D3D11Timer");
}

const char* Exception_ClientResizing::what() const noexcept
{
    return ("Exception_ClientResizing");
}

const char* Exception_RawInputDevices::what() const noexcept
{
    return ("Exception_RawInputDevices");
}

const char* Exception_GraphOptions::what() const noexcept 
{
    return ("Exception_GraphOptions");
}

const char* Exception_GraphFunction::what() const noexcept
{
    return ("Exception_GraphFunction");
}




AdditionalExceptionInformation::AdditionalExceptionInformationIndices AdditionalExceptionInformation::GetLastErrorCode()
{
    return AdditionalExceptionInformation::LastErrorCode;
}

const char* AdditionalExceptionInformation::GetLastErrorDescr()
{
    return AdditionalExceptionInformation::AdditionalExceptionInformationTable[AdditionalExceptionInformation::LastErrorCode];
}

void AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices Code)
{
    AdditionalExceptionInformation::LastErrorCode = Code;
}

void AdditionalExceptionInformation::SetErrorLocation(const char* pcszTranslationUnit, unsigned int uiLineNumber)
{
    AdditionalExceptionInformation::pcszTranslationUnit = pcszTranslationUnit;
    AdditionalExceptionInformation::uiLineNumber        = uiLineNumber; 
}

void    FormatMessageBox(PCSTR pcszFormatting, ...);
char*   ExtractTranslationUnit(const char* pcszTranslationUnitPath);
void AdditionalExceptionInformation::Handler(GenericException& Exception)
{
    char* pszTranslationUnit = ExtractTranslationUnit(AdditionalExceptionInformation::pcszTranslationUnit);
    assert(pszTranslationUnit != nullptr); 

    FormatMessageBox("Exception >:(\nWhat - %s\nWhere - %s (%i)\nAdditional Info - %s", Exception.what(), pszTranslationUnit, AdditionalExceptionInformation::uiLineNumber, AdditionalExceptionInformation::GetLastErrorDescr());

    delete[] pszTranslationUnit;
}




//Misc
void FormatMessageBox(PCSTR pcszFormatting, ...)
{
    assert(pcszFormatting != nullptr);

    va_list pArgs       = nullptr;
    PSTR    pszMessage  = nullptr;

    va_start(pArgs, pcszFormatting);

    do
    {
        int iMessageLength = _vscprintf(pcszFormatting, pArgs) + 1;
        if (iMessageLength == 1)
        {
            break;
        }

        pszMessage = new CHAR[iMessageLength]{};
        if (!pszMessage)
        {
            break;
        }

        vsprintf_s(pszMessage, iMessageLength, pcszFormatting, pArgs);

        MessageBoxA(NULL, pszMessage, ":(", MB_OK);
    } while (false);

    va_end(pArgs);
}

char* ExtractTranslationUnit(const char* pcszTranslationUnitPath)
{
    assert(pcszTranslationUnitPath != nullptr);

    int i = static_cast<int>(strlen(pcszTranslationUnitPath));
    for (; i >= 0; i--)
    {
        if (pcszTranslationUnitPath[i] == '\\')
        {
            break;
        }
    }

    char* pcszTranslationUnit = new char[strlen(pcszTranslationUnitPath) - i]{};
    if (!pcszTranslationUnit) 
    {
        return nullptr;
    }
    for (int j = i + 1, k = 0; j < strlen(pcszTranslationUnitPath); j++, k++)
    {
        pcszTranslationUnit[k] = pcszTranslationUnitPath[j];
    }
    pcszTranslationUnit[strlen(pcszTranslationUnit) - i - 1] = '\0';

    return pcszTranslationUnit;
}