#include "../../Hdr/D3D11/D3D11Fragment.hpp"
#include "../../Hdr/Win32/Window.hpp"

#include <cassert>
#include <tchar.h>




//Cnstrct
D3D11Fragment::D3D11Fragment(ID3D11DeviceContext* pDC)
    : pDC(pDC), pRTV(nullptr), pDSV(nullptr), bDrawFragment(false)
{
    assert(pDC != nullptr);
}

D3D11Fragment::D3D11Fragment(ID3D11DeviceContext* pDC, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV)
    : pDC(pDC), pRTV(pRTV), pDSV(pDSV), bDrawFragment(false)
{
    assert(pDC != nullptr && pRTV != nullptr && pDSV != nullptr);
}




//Public
void D3D11Fragment::BindFragmentSpecificRenderOutput()
{
    assert(this->pDC != nullptr && this->pRTV != nullptr);
    this->pDC->OMSetRenderTargets(1, &(this->pRTV), this->pDSV);
}

void D3D11Fragment::SetDrawAllowance(bool bDraw)
{
    this->bDrawFragment = bDraw;
}

bool D3D11Fragment::ShouldDraw() const
{
    return this->bDrawFragment;
}

TCHAR* D3D11Fragment::GetValidShaderPath(PCTSTR pctszShaderName)
{
    assert(pctszShaderName != nullptr);

    TCHAR* ptszShaderPath = new TCHAR[MAX_PATH]{};
    if (!ptszShaderPath)
    {
        return nullptr;
    }

    if (!GetModuleFileName(nullptr, ptszShaderPath, MAX_PATH))
    {
        return nullptr;
    }

    for (int i = static_cast<int>(_tcslen(ptszShaderPath)); i >= 0; i--)
    {
        if (ptszShaderPath[i] == __TEXT('\\'))
        {
            break;
        }
        else
        {
            ptszShaderPath[i] = __TEXT('\0');
        }
    }
    _tcscat_s(ptszShaderPath, MAX_PATH, pctszShaderName);

    return ptszShaderPath;
}




//Protected
void D3D11Fragment::SetRenderSpecificOutput(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV)
{
    assert(pRTV != nullptr && pDSV != nullptr);
    this->pRTV = pRTV;
    this->pDSV = pDSV;
}