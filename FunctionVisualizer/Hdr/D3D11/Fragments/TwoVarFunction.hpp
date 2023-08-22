#pragma once

#include <DirectXMath.h>

#include "../D3D11Fragment.hpp"
#include "../D3D11.hpp"



class TwoVarFunction : public D3D11Fragment
{
public:
    TwoVarFunction();    


    void BindFragmentSpecificRenderOutput();

    bool InitializeFragment() override;
    bool DrawFragment() override;
    bool GraphOptionsUpdated() override;
    bool GraphFunctionUpdated() override;
    bool GraphStyleUpdated() override;

private:
    typedef struct Vertex
    {
        DirectX::XMFLOAT3 f3LocalPosition;          //LocalPosition
        DirectX::XMFLOAT4 f4Color;                  //Color
    }Vertex;

    typedef struct VertexTransformation
    {
        DirectX::XMMATRIX WorldMatrix;
        DirectX::XMMATRIX ViewMatrix;
        DirectX::XMMATRIX ProjectionMatrix;
    }VertexTransformation;

    typedef struct RangeInfo
    {
        float   fRangeY;
        char    Pad0[12];
    }RangeInfo;


    void    CreatePlane(UINT uiRangeX, UINT uiRangeY, UINT uiRangeZ, List<TwoVarFunction::Vertex>& PlaneVertices, List<DWORD>& PlaneIndices); 
    float   EvaluateFunction(float fX, float fZ);


    List<TwoVarFunction::Vertex>    PlaneVertices;     //Defined for XZ
    List<DWORD>                     PlaneIndices;

    List<PTSTR> PostfixTokens;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>   cpInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpIndexBuffer;
    D3D11_PRIMITIVE_TOPOLOGY                    VertexInterpretation;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>  cpVertexShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpVSTransformationConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   cpRasterizerState;
    D3D11_VIEWPORT                                  ClientViewport;

    Microsoft::WRL::ComPtr<ID3D11PixelShader>   cpPixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpPSRangeInfoConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11BlendState> cpBlendState; 
};