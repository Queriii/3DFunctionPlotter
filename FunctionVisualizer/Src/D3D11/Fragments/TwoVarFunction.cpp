#include <d3d11.h>
#include <d3dcompiler.h>

#include "../../../Hdr/D3D11/Fragments/TwoVarFunction.hpp"
#include "../../../Hdr/D3D11/D3D11Camera.hpp"
#include "../../../Hdr/Exceptions/Exceptions.hpp"
#include "../../../Hdr/Win32/Window.hpp"
#include "../../../Hdr/Data Structures/Stack.hpp"
#include "../../../Hdr/Data Structures/Parser.hpp"




//Cnstrct
TwoVarFunction::TwoVarFunction()
    : D3D11Fragment(D3D11::GetD3D11DeviceContext()), cpInputLayout(nullptr), cpVertexBuffer(nullptr), cpIndexBuffer(nullptr), VertexInterpretation(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST), cpVertexShader(nullptr), cpVSTransformationConstantBuffer(nullptr), ClientViewport({}), cpPixelShader(nullptr), cpPSRangeInfoConstantBuffer(nullptr)
{
    GraphOptions    GraphOptionsConfig      = Window::GetGraphOptionsConfig();
    GraphFunction   GraphFunctionConfig     = Window::GetGraphFunctionConfig();
    assert(GraphFunctionConfig.ptszInfixFunction != nullptr);

    Parser::InfixToPostfix(GraphFunctionConfig.ptszInfixFunction, this->PostfixTokens);

    this->CreatePlane(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, this->PlaneVertices, this->PlaneIndices);
}



//Public
void TwoVarFunction::BindFragmentSpecificRenderOutput()
{
    assert(D3D11::GetSCBackBufferRTV() != nullptr && D3D11::GetSCDepthStencilBufferDSV() != nullptr);

    ID3D11RenderTargetView* pSCBackBufferRTV = D3D11::GetSCBackBufferRTV();
    D3D11::GetD3D11DeviceContext()->OMSetRenderTargets(1, &pSCBackBufferRTV, D3D11::GetSCDepthStencilBufferDSV());
}

bool TwoVarFunction::InitializeFragment()
{
    Microsoft::WRL::ComPtr<ID3DBlob> cpVertexShaderBytes, cpHullShaderBytes, cpDomainShaderBytes, cpPixelShaderBytes;
    PCTSTR pctszVertexShader    = this->GetValidShaderPath(__TEXT("TwoVarFunctionVS.cso"));
    PCTSTR pctszHullShader      = this->GetValidShaderPath(__TEXT("TwoVarFunctionHS.cso"));
    PCTSTR pctszDomainShader    = this->GetValidShaderPath(__TEXT("TwoVarFunctionDS.cso"));
    PCTSTR pctszPixelShader     = this->GetValidShaderPath(__TEXT("TwoVarFunctionPS.cso"));
    

    bool bStatus = true;

    do
    {
        if (FAILED(D3DReadFileToBlob(pctszVertexShader, cpVertexShaderBytes.GetAddressOf())) ||
            FAILED(D3DReadFileToBlob(pctszHullShader, cpHullShaderBytes.GetAddressOf())) ||
            FAILED(D3DReadFileToBlob(pctszDomainShader, cpDomainShaderBytes.GetAddressOf())) ||
            FAILED(D3DReadFileToBlob(pctszPixelShader, cpPixelShaderBytes.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_D3DReadFileToBlob);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_INPUT_ELEMENT_DESC TwoVarFunctionIL[] =
        {
            {"LocalPosition", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, NULL},
            {"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, NULL}
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateInputLayout(TwoVarFunctionIL, _countof(TwoVarFunctionIL), cpVertexShaderBytes->GetBufferPointer(), cpVertexShaderBytes->GetBufferSize(), this->cpInputLayout.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_CreateInputLayout); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
            bStatus = false; 
            break; 
        }

        D3D11_BUFFER_DESC PlaneVertexBufferDesc =
        {
            static_cast<UINT>(this->PlaneVertices.Size() * sizeof(TwoVarFunction::Vertex)), 
            D3D11_USAGE_IMMUTABLE, 
            D3D11_BIND_VERTEX_BUFFER, 
            NULL, 
            NULL, 
            NULL 
        };
        D3D11_SUBRESOURCE_DATA PlaneVertexBufferSubRe =
        {
            &(this->PlaneVertices[0]), 
            NULL, 
            NULL 
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneVertexBufferDesc, &PlaneVertexBufferSubRe, this->cpVertexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
            bStatus = false; 
            break; 
        }

        D3D11_BUFFER_DESC PlaneIndexBufferDesc =
        {
            static_cast<UINT>(this->PlaneIndices.Size() * sizeof(DWORD)),
            D3D11_USAGE_IMMUTABLE, 
            D3D11_BIND_INDEX_BUFFER, 
            NULL, 
            NULL, 
            NULL 
        };
        D3D11_SUBRESOURCE_DATA PlaneIndexBufferSubRe =
        {
            &(this->PlaneIndices[0]), 
            NULL, 
            NULL 
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneIndexBufferDesc, &PlaneIndexBufferSubRe, this->cpIndexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
            bStatus = false; 
            break; 
        }



        if (FAILED(D3D11::GetD3D11Device()->CreateVertexShader(cpVertexShaderBytes->GetBufferPointer(), cpVertexShaderBytes->GetBufferSize(), nullptr, this->cpVertexShader.GetAddressOf()))) 
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateVertexShader); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
            bStatus = false; 
            break; 
        }

        D3D11_BUFFER_DESC VSTransformationConstantBufferDesc =
        {
            sizeof(TwoVarFunction::VertexTransformation),
            D3D11_USAGE_DYNAMIC,
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_CPU_ACCESS_WRITE,
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&VSTransformationConstantBufferDesc, nullptr, this->cpVSTransformationConstantBuffer.GetAddressOf()))) 
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
            bStatus = false; 
            break; 
        }



        GraphStyle GraphStyleConfig = Window::GetGraphStyleConfig();
        D3D11_RASTERIZER_DESC RasterizerStateDesc = 
        {
            (GraphStyleConfig.bWireframeFunction) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID, 
            D3D11_CULL_NONE,
            FALSE,
            NULL,
            0.0f,
            0.0f,
            TRUE,
            FALSE,
            FALSE,
            FALSE
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateRasterizerState(&RasterizerStateDesc, this->cpRasterizerState.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateRasterizerState); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }



        if (FAILED(D3D11::GetD3D11Device()->CreatePixelShader(cpPixelShaderBytes->GetBufferPointer(), cpPixelShaderBytes->GetBufferSize(), nullptr, this->cpPixelShader.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreatePixelShader);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
            bStatus = false; 
            break; 
        }

        D3D11_BUFFER_DESC PSRangeInfoConstantBufferDesc =
        {
            sizeof(TwoVarFunction::RangeInfo),
            D3D11_USAGE_DYNAMIC,
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_CPU_ACCESS_WRITE,
            NULL,
            NULL
        };
        GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();
        TwoVarFunction::RangeInfo RangeInfoConstantBuffer =
        {
            static_cast<float>(GraphOptionsConfig.uiRangeYAxis),
            {}
        };
        D3D11_SUBRESOURCE_DATA PSRangeInfoConstantBufferSubRe =
        {
            &RangeInfoConstantBuffer, 
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PSRangeInfoConstantBufferDesc, &PSRangeInfoConstantBufferSubRe, this->cpPSRangeInfoConstantBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
            bStatus = false; 
            break; 
        }



        D3D11_BLEND_DESC BlendStateDesc =
        {
            FALSE,
            FALSE,
            {
                TRUE,
                D3D11_BLEND_SRC_ALPHA, 
                D3D11_BLEND_INV_SRC_ALPHA, 
                D3D11_BLEND_OP_ADD,
                D3D11_BLEND_ZERO, 
                D3D11_BLEND_ONE, 
                D3D11_BLEND_OP_ADD,
                D3D11_COLOR_WRITE_ENABLE_ALL 
            }
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBlendState(&BlendStateDesc, this->cpBlendState.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBlendState);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

    } while (false);

    if (pctszVertexShader)
    {
        delete[] pctszVertexShader; 
    }
    if (pctszHullShader)
    {
        delete[] pctszHullShader;
    }
    if (pctszDomainShader)
    {
        delete[] pctszDomainShader; 
    }
    if (pctszPixelShader)
    {
        delete[] pctszPixelShader;
    }

    return bStatus;
}

bool TwoVarFunction::DrawFragment()
{
    D3D11::GetD3D11DeviceContext()->IASetInputLayout(this->cpInputLayout.Get());
    UINT uiStrides[] = { sizeof(TwoVarFunction::Vertex) }; UINT uiOffsets[] = { 0 };
    D3D11::GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, this->cpVertexBuffer.GetAddressOf(), uiStrides, uiOffsets);
    D3D11::GetD3D11DeviceContext()->IASetIndexBuffer(this->cpIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, NULL);
    D3D11::GetD3D11DeviceContext()->IASetPrimitiveTopology(this->VertexInterpretation);



    D3D11::GetD3D11DeviceContext()->VSSetShader(this->cpVertexShader.Get(), nullptr, NULL);

    D3D11_MAPPED_SUBRESOURCE MappedVSVertexTransformationConstantBuffer;
    D3D11::GetD3D11DeviceContext()->Map(this->cpVSTransformationConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedVSVertexTransformationConstantBuffer);

    TwoVarFunction::VertexTransformation VSVertexTransformationConstantBuffer =
    {
        DirectX::XMMatrixIdentity(),
        D3D11Camera::GetViewMatrix(),
        D3D11Camera::GetProjectionMatrix()
    };
    memcpy_s(MappedVSVertexTransformationConstantBuffer.pData, sizeof(TwoVarFunction::VertexTransformation), &VSVertexTransformationConstantBuffer, sizeof(VSVertexTransformationConstantBuffer));

    D3D11::GetD3D11DeviceContext()->Unmap(this->cpVSTransformationConstantBuffer.Get(), 0);
    D3D11::GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, this->cpVSTransformationConstantBuffer.GetAddressOf());



    D3D11::GetD3D11DeviceContext()->RSSetState(this->cpRasterizerState.Get());

    this->ClientViewport =
    {
        0.0f,
        0.0f,
        static_cast<float>(Window::GetClientWidth()),
        static_cast<float>(Window::GetClientHeight()),
        0.0f,
        1.0f
    };
    D3D11::GetD3D11DeviceContext()->RSSetViewports(1, &(this->ClientViewport));



    D3D11::GetD3D11DeviceContext()->PSSetShader(this->cpPixelShader.Get(), nullptr, NULL);
    D3D11::GetD3D11DeviceContext()->PSSetConstantBuffers(0, 1, this->cpPSRangeInfoConstantBuffer.GetAddressOf());

    D3D11::GetD3D11DeviceContext()->OMSetBlendState(this->cpBlendState.Get(), {}, 0xFFFFFFFF);
    this->BindFragmentSpecificRenderOutput();

    D3D11::GetD3D11DeviceContext()->DrawIndexed(this->PlaneIndices.Size(), 0, 0);


    D3D11::GetD3D11DeviceContext()->OMSetBlendState(nullptr, {}, 0xFFFFFFFF);

    return true;
}

bool TwoVarFunction::GraphOptionsUpdated()
{
    GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();

    this->CreatePlane(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, this->PlaneVertices, this->PlaneIndices);

    this->cpVertexBuffer.Reset();
    this->cpIndexBuffer.Reset();



    D3D11_BUFFER_DESC PlaneVertexBufferDesc = 
    {
        static_cast<UINT>(this->PlaneVertices.Size() * sizeof(TwoVarFunction::Vertex)), 
        D3D11_USAGE_IMMUTABLE, 
        D3D11_BIND_VERTEX_BUFFER, 
        NULL, 
        NULL, 
        NULL 
    };
    D3D11_SUBRESOURCE_DATA PlaneVertexBufferSubRe = 
    {
        &(this->PlaneVertices[0]), 
        NULL, 
        NULL 
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneVertexBufferDesc, &PlaneVertexBufferSubRe, this->cpVertexBuffer.GetAddressOf()))) 
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer); 
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
        return false;
    }

    D3D11_BUFFER_DESC PlaneIndexBufferDesc = 
    {
        static_cast<UINT>(this->PlaneIndices.Size() * sizeof(DWORD)), 
        D3D11_USAGE_IMMUTABLE, 
        D3D11_BIND_INDEX_BUFFER, 
        NULL, 
        NULL, 
        NULL 
    };
    D3D11_SUBRESOURCE_DATA PlaneIndexBufferSubRe = 
    {
        &(this->PlaneIndices[0]), 
        NULL, 
        NULL 
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneIndexBufferDesc, &PlaneIndexBufferSubRe, this->cpIndexBuffer.GetAddressOf()))) 
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer); 
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
        return false;
    }



    TwoVarFunction::RangeInfo RangeInfoConstantBuffer = 
    {
        static_cast<float>(GraphOptionsConfig.uiRangeYAxis), 
        {}
    };

    D3D11_MAPPED_SUBRESOURCE MappedRangeInfoConstantBuffer;
    D3D11::GetD3D11DeviceContext()->Map(this->cpPSRangeInfoConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedRangeInfoConstantBuffer);
    memcpy_s(MappedRangeInfoConstantBuffer.pData, sizeof(TwoVarFunction::RangeInfo), &RangeInfoConstantBuffer, sizeof(RangeInfoConstantBuffer));
    D3D11::GetD3D11DeviceContext()->Unmap(this->cpPSRangeInfoConstantBuffer.Get(), 0);

    return true;
}

bool TwoVarFunction::GraphFunctionUpdated()
{
    GraphFunction   GraphFunctionConfig = Window::GetGraphFunctionConfig();
    GraphOptions    GraphOptionsConfig  = Window::GetGraphOptionsConfig();


    this->SetDrawAllowance(GraphFunctionConfig.bShowFunction);
    
    for (unsigned int i = 0; i < this->PostfixTokens.Size(); i++)
    {
        delete[] this->PostfixTokens[i];
    }
    this->PostfixTokens.RemoveAll();

    Parser::InfixToPostfix(GraphFunctionConfig.ptszInfixFunction, this->PostfixTokens);



    this->CreatePlane(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, this->PlaneVertices, this->PlaneIndices);

    this->cpVertexBuffer.Reset(); 
    this->cpIndexBuffer.Reset();

    D3D11_BUFFER_DESC PlaneVertexBufferDesc = 
    {
        static_cast<UINT>(this->PlaneVertices.Size() * sizeof(TwoVarFunction::Vertex)), 
        D3D11_USAGE_IMMUTABLE, 
        D3D11_BIND_VERTEX_BUFFER, 
        NULL, 
        NULL, 
        NULL 
    };
    D3D11_SUBRESOURCE_DATA PlaneVertexBufferSubRe = 
    {
        &(this->PlaneVertices[0]), 
        NULL, 
        NULL 
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneVertexBufferDesc, &PlaneVertexBufferSubRe, this->cpVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC PlaneIndexBufferDesc =
    {
        static_cast<UINT>(this->PlaneIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA PlaneIndexBufferSubRe =
    {
        &(this->PlaneIndices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneIndexBufferDesc, &PlaneIndexBufferSubRe, this->cpIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    return true;
}

bool TwoVarFunction::GraphStyleUpdated()
{
    GraphOptions    GraphOptionsConfig  = Window::GetGraphOptionsConfig();
    GraphStyle      GraphStyleConfig    = Window::GetGraphStyleConfig();



    this->CreatePlane(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, this->PlaneVertices, this->PlaneIndices);

    this->cpVertexBuffer.Reset();
    this->cpIndexBuffer.Reset();

    D3D11_BUFFER_DESC PlaneVertexBufferDesc =
    {
        static_cast<UINT>(this->PlaneVertices.Size() * sizeof(TwoVarFunction::Vertex)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_VERTEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA PlaneVertexBufferSubRe =
    {
        &(this->PlaneVertices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneVertexBufferDesc, &PlaneVertexBufferSubRe, this->cpVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC PlaneIndexBufferDesc =
    {
        static_cast<UINT>(this->PlaneIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA PlaneIndexBufferSubRe =
    {
        &(this->PlaneIndices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PlaneIndexBufferDesc, &PlaneIndexBufferSubRe, this->cpIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    this->cpRasterizerState.Reset();
    D3D11_RASTERIZER_DESC RasterizerStateDesc =
    {
        (GraphStyleConfig.bWireframeFunction) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID,
        D3D11_CULL_NONE,
        FALSE,
        NULL,
        0.0f,
        0.0f,
        TRUE,
        FALSE,
        FALSE,
        FALSE
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateRasterizerState(&RasterizerStateDesc, this->cpRasterizerState.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateRasterizerState);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    return true;
}

void TwoVarFunction::CreatePlane(UINT uiRangeX, UINT uiRangeY, UINT uiRangeZ, List<TwoVarFunction::Vertex>& PlaneVertices, List<DWORD>& PlaneIndices)
{
    assert(uiRangeX != NULL && uiRangeY != NULL && uiRangeZ != NULL);

    PlaneVertices.RemoveAll();
    PlaneIndices.RemoveAll();

    GraphFunction   GraphFunctionConfig = Window::GetGraphFunctionConfig();
    GraphStyle      GraphStyleConfig    = Window::GetGraphStyleConfig();
    
    for (int i = -static_cast<int>(uiRangeZ); i <= static_cast<int>(uiRangeZ); i++)
    {
        if (i == static_cast<int>(uiRangeZ))
        {
            for (int j = -static_cast<int>(uiRangeX); j <= static_cast<int>(uiRangeX); j++)
            {
                if (j == static_cast<int>(uiRangeX))
                {
                    TwoVarFunction::Vertex Current; 
                    Current.f3LocalPosition = DirectX::XMFLOAT3(static_cast<float>(j), TwoVarFunction::EvaluateFunction(static_cast<float>(j), static_cast<float>(i)), static_cast<float>(i)); 
                    Current.f4Color         = DirectX::XMFLOAT4(GraphStyleConfig.f3GraphedFunctionColor.x, GraphStyleConfig.f3GraphedFunctionColor.y, GraphStyleConfig.f3GraphedFunctionColor.z, (GraphStyleConfig.bTransparentFunction) ? 0.5f : 1.0f);

                    this->PlaneVertices.Append(Current); 
                }
                else
                {
                    for (int subX = 0; subX < static_cast<int>(GraphFunctionConfig.uiLevelOfDetail); subX++)
                    {
                        TwoVarFunction::Vertex Current; 
                        Current.f3LocalPosition = DirectX::XMFLOAT3(static_cast<float>(j) + static_cast<float>(subX) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail), TwoVarFunction::EvaluateFunction(static_cast<float>(j) + static_cast<float>(subX) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail), static_cast<float>(i)), static_cast<float>(i));
                        Current.f4Color         = DirectX::XMFLOAT4(GraphStyleConfig.f3GraphedFunctionColor.x, GraphStyleConfig.f3GraphedFunctionColor.y, GraphStyleConfig.f3GraphedFunctionColor.z, (GraphStyleConfig.bTransparentFunction) ? 0.5f : 1.0f);

                        this->PlaneVertices.Append(Current); 
                    }
                }
            }
        }
        else
        {
            for (int subZ = 0; subZ < static_cast<int>(GraphFunctionConfig.uiLevelOfDetail); subZ++) 
            { 
                for (int j = -static_cast<int>(uiRangeX); j <= static_cast<int>(uiRangeX); j++) 
                {
                    if (j == static_cast<int>(uiRangeX))
                    {
                        TwoVarFunction::Vertex Current; 
                        Current.f3LocalPosition = DirectX::XMFLOAT3(static_cast<float>(j), TwoVarFunction::EvaluateFunction(static_cast<float>(j), static_cast<float>(i) + static_cast<float>(subZ) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail)), static_cast<float>(i) + static_cast<float>(subZ) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail));
                        Current.f4Color         = DirectX::XMFLOAT4(GraphStyleConfig.f3GraphedFunctionColor.x, GraphStyleConfig.f3GraphedFunctionColor.y, GraphStyleConfig.f3GraphedFunctionColor.z, (GraphStyleConfig.bTransparentFunction) ? 0.5f : 1.0f);

                        this->PlaneVertices.Append(Current); 
                    }
                    else
                    {
                        for (int subX = 0; subX < static_cast<int>(GraphFunctionConfig.uiLevelOfDetail); subX++) 
                        {
                            TwoVarFunction::Vertex Current; 
                            Current.f3LocalPosition = DirectX::XMFLOAT3(static_cast<float>(j) + static_cast<float>(subX) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail), TwoVarFunction::EvaluateFunction(static_cast<float>(j) + static_cast<float>(subX) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail), static_cast<float>(i) + static_cast<float>(subZ) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail)), static_cast<float>(i) + static_cast<float>(subZ) / static_cast<float>(GraphFunctionConfig.uiLevelOfDetail)); 
                            Current.f4Color = DirectX::XMFLOAT4(GraphStyleConfig.f3GraphedFunctionColor.x, GraphStyleConfig.f3GraphedFunctionColor.y, GraphStyleConfig.f3GraphedFunctionColor.z, (GraphStyleConfig.bTransparentFunction) ? 0.5f : 1.0f);

                            this->PlaneVertices.Append(Current);
                        }
                    }
                }
            }
        }
    }

    unsigned int uiOrigNumVerticesPerRow    = (2 * uiRangeX) + 1;
    unsigned int uiOrigNumBoxesPerRow       = uiOrigNumVerticesPerRow - 1;
    unsigned int uiNumVerticesPerRow        = (uiOrigNumBoxesPerRow * GraphFunctionConfig.uiLevelOfDetail) + 1;

    unsigned int uiOrigNumVerticesPerCol    = (2 * uiRangeZ) + 1;
    unsigned int uiOrigNumBoxesPerCol       = uiOrigNumVerticesPerCol - 1;
    unsigned int uiNumVerticesPerCol        = (uiOrigNumBoxesPerCol * GraphFunctionConfig.uiLevelOfDetail) + 1;

    for (int i = 0; i < static_cast<int>(uiNumVerticesPerCol - 1); i++)
    {
        for (int j = 0; j < static_cast<int>(uiNumVerticesPerRow - 1); j++)
        {
            this->PlaneIndices.Append((i * uiNumVerticesPerRow) + j);
            this->PlaneIndices.Append(((i + 1) * uiNumVerticesPerRow) + j);
            this->PlaneIndices.Append(((i + 1) * uiNumVerticesPerRow) + j + 1);

            this->PlaneIndices.Append((i * uiNumVerticesPerRow) + j);
            this->PlaneIndices.Append(((i + 1) * uiNumVerticesPerRow) + j + 1);
            this->PlaneIndices.Append((i * uiNumVerticesPerRow) + j + 1);
        }
    }
}


float TwoVarFunction::EvaluateFunction(float fX, float fZ)
{
    Stack<PTSTR> OperandStack;
    for (UINT i = 0; i < this->PostfixTokens.Size(); i++)
    {
        if (Parser::IsFunction(this->PostfixTokens[i]))
        {
            assert(OperandStack.Size() > 0);

            PTSTR ptszNumber = OperandStack.Pop();
            switch (this->PostfixTokens[i][0])
            {

            case __TEXT('$'):
            {
                float   fRes    = sinf(static_cast<float>(_ttof(ptszNumber)));
                int     iSize   = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{};
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes);
                
                OperandStack.Push(ptszRes);
                break;
            }

            case __TEXT('#'):
            {
                float   fRes    = cosf(static_cast<float>(_ttof(ptszNumber)));
                int     iSize   = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{};
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes);

                OperandStack.Push(ptszRes);
                break;
            }

            case __TEXT('@'):
            {
                float   fRes    = tanf(static_cast<float>(_ttof(ptszNumber)));
                int     iSize   = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{}; 
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes); 

                OperandStack.Push(ptszRes); 
                break; 
            }

            default:
            {
                assert(1 == 0);
            }

            }
        }
        else if (Parser::IsOperator(this->PostfixTokens[i]))
        {
            switch (this->PostfixTokens[i][0])
            {

            case __TEXT('+'):
            {
                assert(OperandStack.Size() > 1);

                PTSTR   ptszNumber1   = OperandStack.Pop();
                PTSTR   ptszNumber2   = OperandStack.Pop();
                float   fRes          = static_cast<float>(_ttof(ptszNumber2)) + static_cast<float>(_ttof(ptszNumber1));
                int     iSize         = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0); 

                PTSTR ptszRes = new TCHAR[iSize + 1]{}; 
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes); 

                OperandStack.Push(ptszRes); 

                delete[] ptszNumber1;
                delete[] ptszNumber2;

                break;
            }

            case __TEXT('-'):
            {
                assert(OperandStack.Size() > 1);

                PTSTR   ptszNumber1 = OperandStack.Pop();
                PTSTR   ptszNumber2 = OperandStack.Pop();
                float   fRes        = static_cast<float>(_ttof(ptszNumber2)) - static_cast<float>(_ttof(ptszNumber1)); 
                int     iSize       = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{};
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes);

                OperandStack.Push(ptszRes);

                delete[] ptszNumber1;
                delete[] ptszNumber2;

                break;
            }

            case __TEXT('*'):
            {
                assert(OperandStack.Size() > 1);

                PTSTR   ptszNumber1 = OperandStack.Pop();
                PTSTR   ptszNumber2 = OperandStack.Pop();
                float   fRes        = static_cast<float>(_ttof(ptszNumber2)) * static_cast<float>(_ttof(ptszNumber1));
                int     iSize       = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{};
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes);

                OperandStack.Push(ptszRes);

                delete[] ptszNumber1;
                delete[] ptszNumber2;

                break;
            }

            case __TEXT('/'):
            {
                assert(OperandStack.Size() > 1);

                PTSTR   ptszNumber1 = OperandStack.Pop();
                PTSTR   ptszNumber2 = OperandStack.Pop();
                float   fRes        = static_cast<float>(_ttof(ptszNumber2)) / static_cast<float>(_ttof(ptszNumber1));
                int     iSize       = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{};
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes);

                OperandStack.Push(ptszRes);

                delete[] ptszNumber1;
                delete[] ptszNumber2;

                break;
            }

            case __TEXT('^'):
            {
                assert(OperandStack.Size() > 1);

                PTSTR   ptszNumber1 = OperandStack.Pop(); 
                PTSTR   ptszNumber2 = OperandStack.Pop(); 
                float   fRes        = powf(static_cast<float>(_ttof(ptszNumber2)), static_cast<float>(_ttof(ptszNumber1)));
                int     iSize       = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{}; 
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes); 

                OperandStack.Push(ptszRes); 

                delete[] ptszNumber1;
                delete[] ptszNumber2;

                break;
            }

            }
        }
        else if (Parser::IsVariable(this->PostfixTokens[i]))
        {
            switch (this->PostfixTokens[i][0])
            {
            
            case __TEXT('x'):
            {
                float fRes  = fX;
                int   iSize = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0);

                PTSTR ptszRes = new TCHAR[iSize + 1]{};
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes);

                OperandStack.Push(ptszRes);

                break;
            }

            case __TEXT('z'):
            {
                float fRes      = fZ;
                int   iSize     = _sntprintf(nullptr, NULL, __TEXT("%f"), fRes);
                assert(iSize > 0); 

                PTSTR ptszRes = new TCHAR[iSize + 1]{}; 
                _stprintf_s(ptszRes, iSize + 1, __TEXT("%f"), fRes); 

                OperandStack.Push(ptszRes); 

                break;
            }

            }
        }
        else if (Parser::IsNumber(this->PostfixTokens[i]))
        {
            size_t ullLength    = _tcslen(this->PostfixTokens[i]);
            PTSTR ptszRes       = new TCHAR[ullLength + 1]{};
            _tcscpy_s(ptszRes, ullLength + 1, this->PostfixTokens[i]);

            OperandStack.Push(ptszRes);
        }
        else
        {
            assert(1 == 0);
        }
    }

    assert(OperandStack.Size() == 1);

    float fFinalRes = static_cast<float>(_ttof(OperandStack.Peek()));
    delete[] OperandStack.Pop();

    return fFinalRes;
}