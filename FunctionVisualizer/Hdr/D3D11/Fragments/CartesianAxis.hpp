#pragma once

#include <DirectXMath.h>

#include "../D3D11Fragment.hpp"




class CartesianAxis : public D3D11Fragment
{
public:
    CartesianAxis();

    void BindFragmentSpecificRenderOutput();

    bool InitializeFragment() override;
    bool DrawFragment() override;
    bool GraphOptionsUpdated() override;
    bool GraphFunctionUpdated() override;
    bool GraphStyleUpdated() override;

private:
    typedef struct Vertex
    {
        DirectX::XMFLOAT3 f3LocalPosition;      //LocalPosition
        DirectX::XMFLOAT3 f3Color;              //Color
    }Vertex;
    Vertex Vertices[6];     //0,1 = XAxis;  2,3 = YAxis;  4,5 = ZAxis       || (NEG, POS)


    typedef struct VertexTransformation
    {
        DirectX::XMMATRIX WorldMatrix;
        DirectX::XMMATRIX ViewMatrix;
        DirectX::XMMATRIX ProjectionMatrix;
    }VertexTransformation;


    Microsoft::WRL::ComPtr<ID3D11InputLayout>   cpInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpVertexBuffer;
    D3D11_PRIMITIVE_TOPOLOGY                    VertexInterpretation;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>  cpVertexShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpVSTransformationConstantBuffer;

    D3D11_VIEWPORT ClientViewport;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> cpPixelShader;
};