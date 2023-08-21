#pragma once

#include <d3d11.h>
#include <wrl/client.h>



class D3D11Fragment
{
public:
    D3D11Fragment(ID3D11DeviceContext* pDC);
    D3D11Fragment(ID3D11DeviceContext* pDC, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV);

    void BindFragmentSpecificRenderOutput();
    void SetDrawAllowance(bool bDraw);

    bool ShouldDraw() const;

    TCHAR* GetValidShaderPath(PCTSTR pctszShaderName);

    virtual bool InitializeFragment()   = 0;        //Is called upon when fragment is registered in d3d11, otherwise don't call (post construction)...
    virtual bool DrawFragment()         = 0;
    virtual bool GraphOptionsUpdated()  = 0;
    virtual bool GraphFunctionUpdated() = 0;

protected:
    void SetRenderSpecificOutput(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV);

private:
    ID3D11DeviceContext*    pDC;
    ID3D11RenderTargetView* pRTV;       //These can be null in the case of rendering directly to the swap chain back & depth buffer.
    ID3D11DepthStencilView* pDSV;       //These can be null in the case of rendering directly to the swap chain back & depth buffer.

    bool bDrawFragment;
};