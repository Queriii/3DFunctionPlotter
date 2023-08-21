#pragma once

#include <DirectXMath.h>

#include "../D3D11Fragment.hpp"
#include "../D3D11.hpp"
#include "../../Data Structures/List.hpp"



class CartesianGrid : public D3D11Fragment
{
public:
    CartesianGrid();

    void BindFragmentSpecificRenderOutput();

    bool InitializeFragment() override;
    bool DrawFragment() override;
    bool GraphOptionsUpdated() override;
    bool GraphFunctionUpdated() override;

    enum GridType
    {
        XZ,
        XY,
        ZY
    };
    void GenerateGrid(UINT uiRangeX, UINT uiRangeY, UINT uiRangeZ, CartesianGrid::GridType Type);

private:
    typedef struct Vertex
    {
        DirectX::XMFLOAT3 f3LocalPosition;      //LocalPosition                 
        DirectX::XMFLOAT3 f3Color;              //Color                         
    }Vertex;


    typedef struct VertexTransformation
    {
        DirectX::XMMATRIX WorldMatrix;          
        DirectX::XMMATRIX ViewMatrix;           
        DirectX::XMMATRIX ProjectionMatrix;     
    }VertexTransformation;

    typedef struct PixelGridType
    {
        int     iGridID;
        char    Pad[12];
    }PixelGridType;


    List<CartesianGrid::Vertex> XZGridVertices;
    List<DWORD>                 XZGridIndices;
    List<CartesianGrid::Vertex> XYGridVertices;
    List<DWORD>                 XYGridIndices;
    List<CartesianGrid::Vertex> ZYGridVertices;
    List<DWORD>                 ZYGridIndices;


    Microsoft::WRL::ComPtr<ID3D11InputLayout>   cpInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpXZGridVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpXYGridVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpZYGridVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpXZGridIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpXYGridIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpZYGridIndexBuffer;
    D3D11_PRIMITIVE_TOPOLOGY                    VertexInterpretation;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>  cpVertexShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpVSTransformationConstantBuffer;

    D3D11_VIEWPORT ClientViewport;

    Microsoft::WRL::ComPtr<ID3D11PixelShader>   cpPixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        cpPSGridTypeConstantBuffer;
};