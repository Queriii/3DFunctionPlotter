#include <d3d11.h>
#include <d3dcompiler.h>

#include "../../../Hdr/D3D11/Fragments/CartesianAxis.hpp"
#include "../../../Hdr/Exceptions/Exceptions.hpp"
#include "../../../Hdr/D3D11/D3D11.hpp"
#include "../../../Hdr/D3D11/D3D11Camera.hpp"
#include "../../../Hdr/Win32/Window.hpp"




//Cnstrct
CartesianAxis::CartesianAxis()
    : D3D11Fragment(D3D11::GetD3D11DeviceContext()), cpInputLayout(nullptr), cpVertexBuffer(nullptr), VertexInterpretation(D3D11_PRIMITIVE_TOPOLOGY_LINELIST), cpVertexShader(nullptr), cpVSTransformationConstantBuffer(nullptr), ClientViewport({}), cpPixelShader(nullptr)
{
    GraphOptions    GraphOptionsConfig  = Window::GetGraphOptionsConfig();
    GraphStyle      GraphStyleConfig    = Window::GetGraphStyleConfig();

    this->Vertices[0] = { DirectX::XMFLOAT3(-static_cast<float>(GraphOptionsConfig.uiRangeXAxis), 0.0f, 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[1] = { DirectX::XMFLOAT3(static_cast<float>(GraphOptionsConfig.uiRangeXAxis), 0.0f, 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[2] = { DirectX::XMFLOAT3(0.0f, -static_cast<float>(GraphOptionsConfig.uiRangeYAxis), 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[3] = { DirectX::XMFLOAT3(0.0f, static_cast<float>(GraphOptionsConfig.uiRangeYAxis), 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[4] = { DirectX::XMFLOAT3(0.0f, 0.0f, -static_cast<float>(GraphOptionsConfig.uiRangeZAxis)), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[5] = { DirectX::XMFLOAT3(0.0f, 0.0f, static_cast<float>(GraphOptionsConfig.uiRangeZAxis)), GraphStyleConfig.f3CartesianAxisColor };

    this->SetDrawAllowance(GraphOptionsConfig.bShowCartesianAxis);
}




//Public
void CartesianAxis::BindFragmentSpecificRenderOutput()
{
    assert(D3D11::GetSCBackBufferRTV() != nullptr && D3D11::GetSCDepthStencilBufferDSV() != nullptr);

    ID3D11RenderTargetView* pSCBackBufferRTV = D3D11::GetSCBackBufferRTV();
    D3D11::GetD3D11DeviceContext()->OMSetRenderTargets(1, &pSCBackBufferRTV, D3D11::GetSCDepthStencilBufferDSV());
}

bool CartesianAxis::InitializeFragment()
{
    Microsoft::WRL::ComPtr<ID3DBlob> cpVertexShaderBytes, cpPixelShaderBytes;
    PCTSTR ptszVertexShader = this->GetValidShaderPath(__TEXT("CartesianAxisVS.cso"));
    PCTSTR ptszPixelShader  = this->GetValidShaderPath(__TEXT("CartesianAxisPS.cso"));


    bool bStatus = true;
    do
    {
        if (FAILED(D3DReadFileToBlob(ptszVertexShader, cpVertexShaderBytes.GetAddressOf())) ||
            FAILED(D3DReadFileToBlob(ptszPixelShader, cpPixelShaderBytes.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_D3DReadFileToBlob);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_INPUT_ELEMENT_DESC CartesianAxisIL[] =
        {
            {"LocalPosition", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, NULL},
            {"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, NULL}
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateInputLayout(CartesianAxisIL, _countof(CartesianAxisIL), cpVertexShaderBytes->GetBufferPointer(), cpVertexShaderBytes->GetBufferSize(), this->cpInputLayout.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_CreateInputLayout);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_BUFFER_DESC VertexBufferDesc =
        {
            _countof(this->Vertices) * sizeof(CartesianAxis::Vertex),
            D3D11_USAGE_DYNAMIC,
            D3D11_BIND_VERTEX_BUFFER,
            D3D11_CPU_ACCESS_WRITE,
            NULL,
            NULL
        };
        D3D11_SUBRESOURCE_DATA VertexBufferSubRe =
        {
            this->Vertices,
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&VertexBufferDesc, &VertexBufferSubRe, this->cpVertexBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }



        if (FAILED(D3D11::GetD3D11Device()->CreateVertexShader(cpVertexShaderBytes->GetBufferPointer(), cpVertexShaderBytes->GetBufferSize(), nullptr, this->cpVertexShader.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_CreateVertexShader); 
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }

        D3D11_BUFFER_DESC VSTransformationConstantBufferDesc =
        {
            sizeof(CartesianAxis::VertexTransformation),
            D3D11_USAGE_DYNAMIC,
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_CPU_ACCESS_WRITE,
            NULL,
            NULL
        };
        if (FAILED(D3D11::GetD3D11Device()->CreateBuffer(&VSTransformationConstantBufferDesc, nullptr, this->cpVSTransformationConstantBuffer.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_CreateBuffer);
            AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
            bStatus = false;
            break;
        }



        if (FAILED(D3D11::GetD3D11Device()->CreatePixelShader(cpPixelShaderBytes->GetBufferPointer(), cpPixelShaderBytes->GetBufferSize(), nullptr, this->cpPixelShader.GetAddressOf())))
        {
            AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::_CreatePixelShader); 
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

bool CartesianAxis::DrawFragment()
{
    D3D11::GetD3D11DeviceContext()->IASetInputLayout(this->cpInputLayout.Get());
    UINT uiStrides[] = { sizeof(CartesianAxis::Vertex) }; UINT uiOffsets[] = { 0 };
    D3D11::GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, this->cpVertexBuffer.GetAddressOf(), uiStrides, uiOffsets);
    D3D11::GetD3D11DeviceContext()->IASetPrimitiveTopology(this->VertexInterpretation);



    D3D11::GetD3D11DeviceContext()->VSSetShader(this->cpVertexShader.Get(), nullptr, NULL);

    D3D11_MAPPED_SUBRESOURCE MappedVSTransformationConstantBuffer;
    D3D11::GetD3D11DeviceContext()->Map(this->cpVSTransformationConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedVSTransformationConstantBuffer);
    CartesianAxis::VertexTransformation VertexTransformationConstantBuffer =
    {
        DirectX::XMMatrixIdentity(),
        D3D11Camera::GetViewMatrix(),
        D3D11Camera::GetProjectionMatrix()
    };
    memcpy_s(MappedVSTransformationConstantBuffer.pData, sizeof(CartesianAxis::VertexTransformation), &VertexTransformationConstantBuffer, sizeof(VertexTransformationConstantBuffer));
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



    this->BindFragmentSpecificRenderOutput();
    D3D11::GetD3D11DeviceContext()->Draw(_countof(this->Vertices), 0);



    return true;
}

bool CartesianAxis::GraphOptionsUpdated() 
{
    GraphOptions    GraphOptionsConfig  = Window::GetGraphOptionsConfig();
    GraphStyle      GraphStyleConfig    = Window::GetGraphStyleConfig();

    this->Vertices[0] = { DirectX::XMFLOAT3(-static_cast<float>(GraphOptionsConfig.uiRangeXAxis), 0.0f, 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[1] = { DirectX::XMFLOAT3(static_cast<float>(GraphOptionsConfig.uiRangeXAxis), 0.0f, 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[2] = { DirectX::XMFLOAT3(0.0f, -static_cast<float>(GraphOptionsConfig.uiRangeYAxis), 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[3] = { DirectX::XMFLOAT3(0.0f, static_cast<float>(GraphOptionsConfig.uiRangeYAxis), 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[4] = { DirectX::XMFLOAT3(0.0f, 0.0f, -static_cast<float>(GraphOptionsConfig.uiRangeZAxis)), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[5] = { DirectX::XMFLOAT3(0.0f, 0.0f, static_cast<float>(GraphOptionsConfig.uiRangeZAxis)), GraphStyleConfig.f3CartesianAxisColor };



    D3D11_MAPPED_SUBRESOURCE MappedVertexBuffer;
    if (FAILED(D3D11::GetD3D11DeviceContext()->Map(this->cpVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedVertexBuffer)))
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_Map);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__); 
        return false;
    }
    memcpy_s(MappedVertexBuffer.pData, _countof(this->Vertices) * sizeof(CartesianAxis::Vertex), this->Vertices, _countof(this->Vertices) * sizeof(CartesianAxis::Vertex));
    D3D11::GetD3D11DeviceContext()->Unmap(this->cpVertexBuffer.Get(), 0);



    this->SetDrawAllowance(GraphOptionsConfig.bShowCartesianAxis);



    return true;
}

bool CartesianAxis::GraphFunctionUpdated()
{
    return true;
}

bool CartesianAxis::GraphStyleUpdated()
{
    GraphOptions    GraphOptionsConfig = Window::GetGraphOptionsConfig();
    GraphStyle      GraphStyleConfig = Window::GetGraphStyleConfig();

    this->Vertices[0] = { DirectX::XMFLOAT3(-static_cast<float>(GraphOptionsConfig.uiRangeXAxis), 0.0f, 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[1] = { DirectX::XMFLOAT3(static_cast<float>(GraphOptionsConfig.uiRangeXAxis), 0.0f, 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[2] = { DirectX::XMFLOAT3(0.0f, -static_cast<float>(GraphOptionsConfig.uiRangeYAxis), 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[3] = { DirectX::XMFLOAT3(0.0f, static_cast<float>(GraphOptionsConfig.uiRangeYAxis), 0.0f), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[4] = { DirectX::XMFLOAT3(0.0f, 0.0f, -static_cast<float>(GraphOptionsConfig.uiRangeZAxis)), GraphStyleConfig.f3CartesianAxisColor };
    this->Vertices[5] = { DirectX::XMFLOAT3(0.0f, 0.0f, static_cast<float>(GraphOptionsConfig.uiRangeZAxis)), GraphStyleConfig.f3CartesianAxisColor };

    D3D11_MAPPED_SUBRESOURCE MappedVertexBuffer; 
    if (FAILED(D3D11::GetD3D11DeviceContext()->Map(this->cpVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &MappedVertexBuffer))) 
    {
        AdditionalExceptionInformation::SetLastErrorCode(AdditionalExceptionInformation::AdditionalExceptionInformationIndices::_Map);
        AdditionalExceptionInformation::SetErrorLocation(__FILE__, __LINE__);
        return false;
    }
    memcpy_s(MappedVertexBuffer.pData, _countof(this->Vertices) * sizeof(CartesianAxis::Vertex), this->Vertices, _countof(this->Vertices) * sizeof(CartesianAxis::Vertex));
    D3D11::GetD3D11DeviceContext()->Unmap(this->cpVertexBuffer.Get(), 0);

    return true;
}