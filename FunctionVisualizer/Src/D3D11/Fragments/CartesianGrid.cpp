#include "../../../Hdr/D3D11/Fragments/CartesianGrid.hpp"
#include "../../../Hdr/Win32/Window.hpp"
#include "../../../Hdr/D3D11/D3D11Camera.hpp"
#include "../../../Hdr/Exceptions/Exceptions.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>




//Cnstrct
CartesianGrid::CartesianGrid()
    : D3D11Fragment(D3D11::GetD3D11DeviceContext()), cpInputLayout(nullptr), cpXZGridVertexBuffer(nullptr), cpXYGridVertexBuffer(nullptr), cpZYGridVertexBuffer(nullptr), cpXZGridIndexBuffer(nullptr), cpXYGridIndexBuffer(nullptr), cpZYGridIndexBuffer(nullptr), VertexInterpretation(D3D11_PRIMITIVE_TOPOLOGY_LINELIST), cpVertexShader(nullptr), cpVSTransformationConstantBuffer(nullptr), ClientViewport({}), cpPixelShader(nullptr)
{
    GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();

    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::GridType::XZ);
    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::GridType::XY);
    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::GridType::ZY);

    this->SetDrawAllowance(GraphOptionsConfig.bShowCartesianGrid);
}




//Public
void CartesianGrid::BindFragmentSpecificRenderOutput()
{
    assert(D3D11::GetSCBackBufferRTV() != nullptr && D3D11::GetSCDepthStencilBufferDSV() != nullptr);

    ID3D11RenderTargetView* pSCBackBufferRTV = D3D11::GetSCBackBufferRTV();
    D3D11::GetD3D11DeviceContext()->OMSetRenderTargets(1, &pSCBackBufferRTV, D3D11::GetSCDepthStencilBufferDSV());
}

bool CartesianGrid::InitializeFragment()
{
    Microsoft::WRL::ComPtr<ID3DBlob> cpVertexShaderBytes, cpPixelShaderBytes;
    PCTSTR ptszVertexShader = this->GetValidShaderPath(__TEXT("CartesianGridVS.cso"));
    PCTSTR ptszPixelShader  = this->GetValidShaderPath(__TEXT("CartesianGridPS.cso"));


    bool bStatus = true;
    do
    {
        if (FAILED(D3DReadFileToBlob(ptszVertexShader, cpVertexShaderBytes.GetAddressOf())) ||
            FAILED(D3DReadFileToBlob(ptszPixelShader, cpPixelShaderBytes.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_D3DReadFileToBlob);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_INPUT_ELEMENT_DESC CartesianGridIL[] =
        {
            {"LocalPosition", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, NULL},
            {"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, NULL},
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateInputLayout(CartesianGridIL, _countof(CartesianGridIL), cpVertexShaderBytes->GetBufferPointer(), cpVertexShaderBytes->GetBufferSize(), this->cpInputLayout.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateInputLayout); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }


        //XZ
        D3D11_BUFFER_DESC XZGridVertexBufferDesc =
        {
            static_cast<UINT>(this->XZGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
            D3D11_USAGE_IMMUTABLE,
            D3D11_BIND_VERTEX_BUFFER,
            NULL,
            NULL,
            NULL
        };
        D3D11_SUBRESOURCE_DATA XZGridVertexBufferSubRe =
        {
            &(this->XZGridVertices[0]),
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XZGridVertexBufferDesc, &XZGridVertexBufferSubRe, this->cpXZGridVertexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_BUFFER_DESC XZGridIndexBufferDesc =
        {
            static_cast<UINT>(this->XZGridIndices.Size() * sizeof(DWORD)),
            D3D11_USAGE_IMMUTABLE,
            D3D11_BIND_INDEX_BUFFER,
            NULL,
            NULL,
            NULL
        };
        D3D11_SUBRESOURCE_DATA XZGridIndexBufferSubRe =
        {
            &(this->XZGridIndices[0]),
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XZGridIndexBufferDesc, &XZGridIndexBufferSubRe, this->cpXZGridIndexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }


        //XY
        D3D11_BUFFER_DESC XYGridVertexBufferDesc =
        {
            static_cast<UINT>(this->XYGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
            D3D11_USAGE_IMMUTABLE,
            D3D11_BIND_VERTEX_BUFFER,
            NULL,
            NULL,
            NULL
        };
        D3D11_SUBRESOURCE_DATA XYGridVertexBufferSubRe =
        {
            &(this->XYGridVertices[0]),
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XYGridVertexBufferDesc, &XYGridVertexBufferSubRe, this->cpXYGridVertexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_BUFFER_DESC XYGridIndexBufferDesc =
        {
            static_cast<UINT>(this->XYGridIndices.Size() * sizeof(DWORD)),
            D3D11_USAGE_IMMUTABLE,
            D3D11_BIND_INDEX_BUFFER,
            NULL,
            NULL,
            NULL
        };
        D3D11_SUBRESOURCE_DATA XYGridIndexBufferSubRe =
        {
            &(this->XYGridIndices[0]),
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XYGridIndexBufferDesc, &XYGridIndexBufferSubRe, this->cpXYGridIndexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }


        //ZY
        D3D11_BUFFER_DESC ZYGridVertexBufferDesc =
        {
            static_cast<UINT>(this->ZYGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
            D3D11_USAGE_IMMUTABLE,
            D3D11_BIND_VERTEX_BUFFER,
            NULL,
            NULL,
            NULL
        };
        D3D11_SUBRESOURCE_DATA ZYGridVertexBufferSubRe =
        {
            &(this->ZYGridVertices[0]),
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&ZYGridVertexBufferDesc, &ZYGridVertexBufferSubRe, this->cpZYGridVertexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_BUFFER_DESC ZYGridIndexBufferDesc =
        {
            static_cast<UINT>(this->ZYGridIndices.Size() * sizeof(DWORD)),
            D3D11_USAGE_IMMUTABLE,
            D3D11_BIND_INDEX_BUFFER,
            NULL,
            NULL,
            NULL
        };
        D3D11_SUBRESOURCE_DATA ZYGridIndexBufferSubRe =
        {
            &(this->ZYGridIndices[0]),
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&ZYGridIndexBufferDesc, &ZYGridIndexBufferSubRe, this->cpZYGridIndexBuffer.GetAddressOf())))
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
            sizeof(CartesianGrid::VertexTransformation),
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



        if (FAILED(D3D11::GetD3D11Device()->CreatePixelShader(cpPixelShaderBytes->GetBufferPointer(), cpPixelShaderBytes->GetBufferSize(), nullptr, this->cpPixelShader.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreatePixelShader);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_BUFFER_DESC PSGridTypeConstantBufferDesc =
        {
            sizeof(CartesianGrid::PixelGridType),
            D3D11_USAGE_DYNAMIC,
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_CPU_ACCESS_WRITE,
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&PSGridTypeConstantBufferDesc, nullptr, this->cpPSGridTypeConstantBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

    } while (false);

    if (ptszVertexShader)
    {
        delete[] ptszVertexShader;
    }
    if (ptszPixelShader)
    {
        delete[] ptszPixelShader;
    }


    return bStatus;
}

bool CartesianGrid::DrawFragment()
{
    GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();



    D3D11::GetD3D11DeviceContext()->IASetInputLayout(this->cpInputLayout.Get());
    UINT uiStrides[] = { sizeof(CartesianGrid::Vertex) }; UINT uiOffsets[] = { 0 };
    D3D11::GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, this->cpXZGridVertexBuffer.GetAddressOf(), uiStrides, uiOffsets);
    D3D11::GetD3D11DeviceContext()->IASetIndexBuffer(this->cpXZGridIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, NULL);
    D3D11::GetD3D11DeviceContext()->IASetPrimitiveTopology(this->VertexInterpretation);



    D3D11::GetD3D11DeviceContext()->VSSetShader(this->cpVertexShader.Get(), nullptr, NULL);
    
    D3D11_MAPPED_SUBRESOURCE MappedVSVertexTransformationConstantBuffer;
    D3D11::GetD3D11DeviceContext()->Map(this->cpVSTransformationConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedVSVertexTransformationConstantBuffer);

    CartesianGrid::VertexTransformation VSVertexTransformationConstantBuffer =
    {
        DirectX::XMMatrixIdentity(),
        D3D11Camera::GetViewMatrix(),
        D3D11Camera::GetProjectionMatrix()
    };
    memcpy_s(MappedVSVertexTransformationConstantBuffer.pData, sizeof(CartesianGrid::VertexTransformation), &VSVertexTransformationConstantBuffer, sizeof(VSVertexTransformationConstantBuffer));

    D3D11::GetD3D11DeviceContext()->Unmap(this->cpVSTransformationConstantBuffer.Get(), 0);
    D3D11::GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, this->cpVSTransformationConstantBuffer.GetAddressOf());



    D3D11::GetD3D11DeviceContext()->RSSetState(nullptr);

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

    D3D11_MAPPED_SUBRESOURCE MappedPSGridTypeConstantBuffer;
    D3D11::GetD3D11DeviceContext()->Map(this->cpPSGridTypeConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedPSGridTypeConstantBuffer);

    CartesianGrid::PixelGridType PSGridTypeConstantBuffer =
    {
        (GraphOptionsConfig.bShowCartesianAxis) ? 0 : 3    
    };
    memcpy_s(MappedPSGridTypeConstantBuffer.pData, sizeof(CartesianGrid::PixelGridType), &PSGridTypeConstantBuffer, sizeof(PSGridTypeConstantBuffer));

    D3D11::GetD3D11DeviceContext()->Unmap(this->cpPSGridTypeConstantBuffer.Get(), 0); 
    D3D11::GetD3D11DeviceContext()->PSSetConstantBuffers(0, 1, this->cpPSGridTypeConstantBuffer.GetAddressOf());



    this->BindFragmentSpecificRenderOutput();

    D3D11::GetD3D11DeviceContext()->DrawIndexed(static_cast<UINT>(this->XZGridIndices.Size()), 0, 0);



    D3D11::GetD3D11DeviceContext()->Map(this->cpPSGridTypeConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedPSGridTypeConstantBuffer);

    PSGridTypeConstantBuffer =
    {
        (GraphOptionsConfig.bShowCartesianAxis) ? 1 : 3
    };
    memcpy_s(MappedPSGridTypeConstantBuffer.pData, sizeof(CartesianGrid::PixelGridType), &PSGridTypeConstantBuffer, sizeof(PSGridTypeConstantBuffer));

    D3D11::GetD3D11DeviceContext()->Unmap(this->cpPSGridTypeConstantBuffer.Get(), 0);
    D3D11::GetD3D11DeviceContext()->PSSetConstantBuffers(0, 1, this->cpPSGridTypeConstantBuffer.GetAddressOf());

    D3D11::GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, this->cpXYGridVertexBuffer.GetAddressOf(), uiStrides, uiOffsets);
    D3D11::GetD3D11DeviceContext()->IASetIndexBuffer(this->cpXYGridIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, NULL);
    D3D11::GetD3D11DeviceContext()->DrawIndexed(static_cast<UINT>(this->XYGridIndices.Size()), 0, 0);



    D3D11::GetD3D11DeviceContext()->Map(this->cpPSGridTypeConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedPSGridTypeConstantBuffer);

    PSGridTypeConstantBuffer =
    {
        (GraphOptionsConfig.bShowCartesianAxis) ? 2 : 3
    };
    memcpy_s(MappedPSGridTypeConstantBuffer.pData, sizeof(CartesianGrid::PixelGridType), &PSGridTypeConstantBuffer, sizeof(PSGridTypeConstantBuffer));

    D3D11::GetD3D11DeviceContext()->Unmap(this->cpPSGridTypeConstantBuffer.Get(), 0);
    D3D11::GetD3D11DeviceContext()->PSSetConstantBuffers(0, 1, this->cpPSGridTypeConstantBuffer.GetAddressOf());

    D3D11::GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, this->cpZYGridVertexBuffer.GetAddressOf(), uiStrides, uiOffsets);
    D3D11::GetD3D11DeviceContext()->IASetIndexBuffer(this->cpZYGridIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, NULL);
    D3D11::GetD3D11DeviceContext()->DrawIndexed(static_cast<UINT>(this->ZYGridIndices.Size()), 0, 0);



    return true;
}

bool CartesianGrid::GraphOptionsUpdated()
{
    GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();

    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::XZ);
    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::XY);
    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::ZY);
      
    this->cpXZGridVertexBuffer.Reset();
    this->cpXZGridIndexBuffer.Reset();
    this->cpXYGridVertexBuffer.Reset();
    this->cpXYGridIndexBuffer.Reset();
    this->cpZYGridVertexBuffer.Reset();
    this->cpZYGridIndexBuffer.Reset();



    //XZ
    D3D11_BUFFER_DESC XZGridVertexBufferDesc =
    {
        static_cast<UINT>(this->XZGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_VERTEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XZGridVertexBufferSubRe =
    {
        &(this->XZGridVertices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XZGridVertexBufferDesc, &XZGridVertexBufferSubRe, this->cpXZGridVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC XZGridIndexBufferDesc =
    {
        static_cast<UINT>(this->XZGridIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XZGridIndexBufferSubRe =
    {
        &(this->XZGridIndices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XZGridIndexBufferDesc, &XZGridIndexBufferSubRe, this->cpXZGridIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }


    //XY
    D3D11_BUFFER_DESC XYGridVertexBufferDesc =
    {
        static_cast<UINT>(this->XYGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_VERTEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XYGridVertexBufferSubRe =
    {
        &(this->XYGridVertices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XYGridVertexBufferDesc, &XYGridVertexBufferSubRe, this->cpXYGridVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC XYGridIndexBufferDesc =
    {
        static_cast<UINT>(this->XYGridIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XYGridIndexBufferSubRe =
    {
        &(this->XYGridIndices[0]), 
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XYGridIndexBufferDesc, &XYGridIndexBufferSubRe, this->cpXYGridIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }


    //ZY
    D3D11_BUFFER_DESC ZYGridVertexBufferDesc =
    {
        static_cast<UINT>(this->ZYGridVertices.Size() * sizeof(CartesianGrid::Vertex)), 
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_VERTEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA ZYGridVertexBufferSubRe =
    {
        &(this->ZYGridVertices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&ZYGridVertexBufferDesc, &ZYGridVertexBufferSubRe, this->cpZYGridVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC ZYGridIndexBufferDesc =
    {
        static_cast<UINT>(this->ZYGridIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA ZYGridIndexBufferSubRe =
    {
        &(this->ZYGridIndices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&ZYGridIndexBufferDesc, &ZYGridIndexBufferSubRe, this->cpZYGridIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }



    this->SetDrawAllowance(GraphOptionsConfig.bShowCartesianGrid);



    return true;
}

bool CartesianGrid::GraphFunctionUpdated()
{
    return true;
}

bool CartesianGrid::GraphStyleUpdated()
{
    GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();

    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::XZ);
    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::XY);
    this->GenerateGrid(GraphOptionsConfig.uiRangeXAxis, GraphOptionsConfig.uiRangeYAxis, GraphOptionsConfig.uiRangeZAxis, CartesianGrid::ZY);

    this->cpXZGridVertexBuffer.Reset();
    this->cpXZGridIndexBuffer.Reset();
    this->cpXYGridVertexBuffer.Reset();
    this->cpXYGridIndexBuffer.Reset();
    this->cpZYGridVertexBuffer.Reset();
    this->cpZYGridIndexBuffer.Reset();



    //XZ
    D3D11_BUFFER_DESC XZGridVertexBufferDesc =
    {
        static_cast<UINT>(this->XZGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_VERTEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XZGridVertexBufferSubRe =
    {
        &(this->XZGridVertices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XZGridVertexBufferDesc, &XZGridVertexBufferSubRe, this->cpXZGridVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC XZGridIndexBufferDesc =
    {
        static_cast<UINT>(this->XZGridIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XZGridIndexBufferSubRe =
    {
        &(this->XZGridIndices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XZGridIndexBufferDesc, &XZGridIndexBufferSubRe, this->cpXZGridIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }


    //XY
    D3D11_BUFFER_DESC XYGridVertexBufferDesc =
    {
        static_cast<UINT>(this->XYGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_VERTEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XYGridVertexBufferSubRe =
    {
        &(this->XYGridVertices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XYGridVertexBufferDesc, &XYGridVertexBufferSubRe, this->cpXYGridVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC XYGridIndexBufferDesc =
    {
        static_cast<UINT>(this->XYGridIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA XYGridIndexBufferSubRe =
    {
        &(this->XYGridIndices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&XYGridIndexBufferDesc, &XYGridIndexBufferSubRe, this->cpXYGridIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }


    //ZY
    D3D11_BUFFER_DESC ZYGridVertexBufferDesc =
    {
        static_cast<UINT>(this->ZYGridVertices.Size() * sizeof(CartesianGrid::Vertex)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_VERTEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA ZYGridVertexBufferSubRe =
    {
        &(this->ZYGridVertices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&ZYGridVertexBufferDesc, &ZYGridVertexBufferSubRe, this->cpZYGridVertexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    D3D11_BUFFER_DESC ZYGridIndexBufferDesc =
    {
        static_cast<UINT>(this->ZYGridIndices.Size() * sizeof(DWORD)),
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        NULL,
        NULL,
        NULL
    };
    D3D11_SUBRESOURCE_DATA ZYGridIndexBufferSubRe =
    {
        &(this->ZYGridIndices[0]),
        NULL,
        NULL
    };
    if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&ZYGridIndexBufferDesc, &ZYGridIndexBufferSubRe, this->cpZYGridIndexBuffer.GetAddressOf())))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_CreateBuffer);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }

    return true;
}

void CartesianGrid::GenerateGrid(UINT uiRangeX, UINT uiRangeY, UINT uiRangeZ, CartesianGrid::GridType Type)
{
    assert(uiRangeX != 0 && uiRangeY != 0 && uiRangeZ != 0);

    GraphStyle GraphStyleConfig = Window::GetGraphStyleConfig();


    switch (Type)
    {
    
    case CartesianGrid::GridType::XZ:
    {
        this->XZGridVertices.RemoveAll();
        this->XZGridIndices.RemoveAll();


        //Create XZ Plane
        for (int z = -static_cast<int>(uiRangeZ); z <= static_cast<int>(uiRangeZ); z++)
        {
            for (int x = -static_cast<int>(uiRangeX); x <= static_cast<int>(uiRangeX); x++)
            {
                CartesianGrid::Vertex Current;
                Current.f3Color         = GraphStyleConfig.f3CartesianGridColor;
                Current.f3LocalPosition = DirectX::XMFLOAT3(static_cast<float>(x), 0.0f, static_cast<float>(z));

                this->XZGridVertices.Append(Current);
            }
        }

        //Create Plane Indices
        UINT uiVerticesPerXRow = (2 * uiRangeX) + 1;
        for (int z = -static_cast<int>(uiRangeZ), Cur = 0; z < static_cast<int>(uiRangeZ); z++)
        {
            for (int x = -static_cast<int>(uiRangeX); x < static_cast<int>(uiRangeX); x++)
            {
                this->XZGridIndices.Append(static_cast<DWORD>(Cur));
                this->XZGridIndices.Append(static_cast<DWORD>(Cur + 1));

                this->XZGridIndices.Append(static_cast<DWORD>(Cur + 1));
                this->XZGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow + 1));

                this->XZGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow + 1));
                this->XZGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow));

                this->XZGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow));
                this->XZGridIndices.Append(static_cast<DWORD>(Cur));

                Cur++;
            }
            Cur++;
        }
        break;
    }

    case CartesianGrid::GridType::XY:
    {
        this->XYGridVertices.RemoveAll(); 
        this->XYGridIndices.RemoveAll();


        //Create XZ Plane
        for (int y = -static_cast<int>(uiRangeY); y <= static_cast<int>(uiRangeY); y++)
        {
            for (int x = -static_cast<int>(uiRangeX); x <= static_cast<int>(uiRangeX); x++)
            {
                CartesianGrid::Vertex Current;
                Current.f3Color         = GraphStyleConfig.f3CartesianGridColor;
                Current.f3LocalPosition = DirectX::XMFLOAT3(static_cast<float>(x), static_cast<float>(y), 0.0f);

                this->XYGridVertices.Append(Current); 
            }
        }

        //Create Plane Indices
        UINT uiVerticesPerXRow = (2 * uiRangeX) + 1;
        for (int y = -static_cast<int>(uiRangeY), Cur = 0; y < static_cast<int>(uiRangeY); y++)
        {
            for (int x = -static_cast<int>(uiRangeX); x < static_cast<int>(uiRangeX); x++)
            {
                this->XYGridIndices.Append(static_cast<DWORD>(Cur));
                this->XYGridIndices.Append(static_cast<DWORD>(Cur + 1)); 

                this->XYGridIndices.Append(static_cast<DWORD>(Cur + 1)); 
                this->XYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow + 1));

                this->XYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow + 1)); 
                this->XYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow));

                this->XYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerXRow)); 
                this->XYGridIndices.Append(static_cast<DWORD>(Cur));

                Cur++;
            }
            Cur++;
        }
        break;
    }

    case CartesianGrid::GridType::ZY:
    {
        this->ZYGridVertices.RemoveAll();
        this->ZYGridIndices.RemoveAll();  


        //Create XZ Plane
        for (int y = -static_cast<int>(uiRangeY); y <= static_cast<int>(uiRangeY); y++)
        {
            for (int z = -static_cast<int>(uiRangeZ); z <= static_cast<int>(uiRangeZ); z++)
            {
                CartesianGrid::Vertex Current; 
                Current.f3Color         = GraphStyleConfig.f3CartesianGridColor;
                Current.f3LocalPosition = DirectX::XMFLOAT3(0.0f, static_cast<float>(y), static_cast<float>(z));

                this->ZYGridVertices.Append(Current); 
            }
        }

        //Create Plane Indices
        UINT uiVerticesPerZRow = (2 * uiRangeZ) + 1;
        for (int y = -static_cast<int>(uiRangeY), Cur = 0; y < static_cast<int>(uiRangeY); y++)
        {
            for (int z = -static_cast<int>(uiRangeZ); z < static_cast<int>(uiRangeZ); z++)
            {
                this->ZYGridIndices.Append(static_cast<DWORD>(Cur)); 
                this->ZYGridIndices.Append(static_cast<DWORD>(Cur + 1)); 

                this->ZYGridIndices.Append(static_cast<DWORD>(Cur + 1));
                this->ZYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerZRow + 1)); 

                this->ZYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerZRow + 1)); 
                this->ZYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerZRow)); 

                this->ZYGridIndices.Append(static_cast<DWORD>(Cur + uiVerticesPerZRow));
                this->ZYGridIndices.Append(static_cast<DWORD>(Cur)); 

                Cur++;
            }
            Cur++;
        }
        break;
    }

    }
    
}