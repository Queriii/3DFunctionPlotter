#include "../../Hdr/D3D11/D3D11Camera.hpp"
#include "../../Hdr/Win32/Window.hpp"

#include <algorithm>




//Static Inits
bool D3D11Camera::bFreecamMovement                      = false;

DirectX::XMFLOAT3 D3D11Camera::f3CameraWorldPosition    = { 0.0f, 0.0f, -1.0f };
DirectX::XMFLOAT3 D3D11Camera::f3CameraPitchYawRoll     = { 0.0f, 0.0f, 0.0f };

float   D3D11Camera::fCameraFov                             = 0.0f;
UINT    D3D11Camera::uiCameraSpeed                          = 0;
UINT    D3D11Camera::uiCameraSensitivity                    = 0;

DirectX::XMMATRIX D3D11Camera::ViewMatrix               = DirectX::XMMatrixIdentity();
DirectX::XMMATRIX D3D11Camera::ProjectionMatrix         = DirectX::XMMatrixIdentity();




//Public
void D3D11Camera::InitializeDefault()
{
    GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();

    D3D11Camera::bFreecamMovement = GraphOptionsConfig.bFreecamMovement;

    D3D11Camera::SetLensInformation(90.0f, true);
}

void D3D11Camera::SetLensInformation(float fFov, bool bDegrees)
{
    if (bDegrees)
    {
        assert(fFov > 0.0f && fFov <= 180.0f);
    }
    else
    {
        assert(fFov > 0.0f && fFov <= DirectX::XM_PI);
    }
    D3D11Camera::fCameraFov = (bDegrees) ? DirectX::XMConvertToRadians(fFov) : fFov;
    
    D3D11Camera::UpdateMatrices();
}

void D3D11Camera::SetCameraPosition(float fX, float fY, float fZ)
{
    D3D11Camera::f3CameraWorldPosition = DirectX::XMFLOAT3(fX, fY, fZ);
    D3D11Camera::UpdateMatrices();
}

void D3D11Camera::SetCameraOrientation(float fPitch, float fYaw, float fRoll)
{
    D3D11Camera::f3CameraPitchYawRoll = DirectX::XMFLOAT3(fPitch, fYaw, fRoll);
    D3D11Camera::UpdateMatrices();
}

void D3D11Camera::OffsetCameraPosition(float fDeltaX, float fDeltaY, float fDeltaZ)
{
    if (D3D11Camera::bFreecamMovement)
    {
        assert(D3D11Camera::uiCameraSpeed > 0);


        DirectX::XMMATRIX RotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(D3D11Camera::f3CameraPitchYawRoll.x, D3D11Camera::f3CameraPitchYawRoll.y, D3D11Camera::f3CameraPitchYawRoll.z);

        DirectX::XMVECTOR NewRight  = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(D3D11Camera::f3DefaultRight)), RotationMatrix));
        DirectX::XMVECTOR NewUp     = DirectX::XMLoadFloat3(&(D3D11Camera::f3DefaultUp));
        DirectX::XMVECTOR NewFoward = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(D3D11Camera::f3DefaultFoward)), RotationMatrix));

        DirectX::XMVECTOR PositionalOffset = DirectX::XMVectorAdd(DirectX::XMVectorAdd(DirectX::XMVectorScale(NewRight, fDeltaX * D3D11Camera::uiCameraSpeed), DirectX::XMVectorScale(NewUp, fDeltaY * D3D11Camera::uiCameraSpeed)), DirectX::XMVectorScale(NewFoward, fDeltaZ * D3D11Camera::uiCameraSpeed));
        DirectX::XMStoreFloat3(&(D3D11Camera::f3CameraWorldPosition), DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&(D3D11Camera::f3CameraWorldPosition)), PositionalOffset));

        D3D11Camera::UpdateMatrices();
    }
}

void D3D11Camera::OffsetCameraOrientation(float fDeltaPitch, float fDeltaYaw, float fDeltaRoll, bool bDegrees)
{
    if (D3D11Camera::bFreecamMovement)
    {
        assert(D3D11Camera::uiCameraSensitivity > 0.0f);


        D3D11Camera::f3CameraPitchYawRoll.x += (bDegrees) ? DirectX::XMConvertToRadians(fDeltaPitch) * D3D11Camera::uiCameraSensitivity : fDeltaPitch * D3D11Camera::uiCameraSensitivity;
        if (D3D11Camera::f3CameraPitchYawRoll.x > 0.0f)
        {
            D3D11Camera::f3CameraPitchYawRoll.x = std::min<float>(DirectX::XMConvertToRadians(89.0f), D3D11Camera::f3CameraPitchYawRoll.x);
        }
        else
        {
            D3D11Camera::f3CameraPitchYawRoll.x = std::max<float>(DirectX::XMConvertToRadians(-89.0f), D3D11Camera::f3CameraPitchYawRoll.x);
        }

        D3D11Camera::f3CameraPitchYawRoll.y += (bDegrees) ? DirectX::XMConvertToRadians(fDeltaYaw) * D3D11Camera::uiCameraSensitivity : fDeltaYaw * D3D11Camera::uiCameraSensitivity;
        D3D11Camera::f3CameraPitchYawRoll.y = fmodf(D3D11Camera::f3CameraPitchYawRoll.y, DirectX::XM_2PI);

        D3D11Camera::f3CameraPitchYawRoll.z += (bDegrees) ? DirectX::XMConvertToRadians(fDeltaRoll) * D3D11Camera::uiCameraSensitivity : fDeltaRoll * D3D11Camera::uiCameraSensitivity;
        D3D11Camera::f3CameraPitchYawRoll.z = fmodf(D3D11Camera::f3CameraPitchYawRoll.z, DirectX::XM_2PI);

        D3D11Camera::UpdateMatrices();
    }
}

bool D3D11Camera::CameraSettingsUpdated()
{
    GraphOptions GraphOptionsConfig = Window::GetGraphOptionsConfig();

    D3D11Camera::bFreecamMovement       = GraphOptionsConfig.bFreecamMovement;
    D3D11Camera::uiCameraSpeed          = GraphOptionsConfig.uiCameraSpeed;
    D3D11Camera::uiCameraSensitivity    = GraphOptionsConfig.uiCameraSensitivity;

    return true;
}

DirectX::XMMATRIX D3D11Camera::GetViewMatrix()
{
    return D3D11Camera::ViewMatrix;
}

DirectX::XMMATRIX D3D11Camera::GetProjectionMatrix()
{
    return D3D11Camera::ProjectionMatrix;
}

DirectX::XMMATRIX D3D11Camera::GetViewProjectionMatrix()
{
    return (D3D11Camera::ViewMatrix * D3D11Camera::ProjectionMatrix);
}




//Private
void D3D11Camera::UpdateMatrices()
{
    assert(D3D11Camera::fCameraFov >= 1.0f);

    DirectX::XMMATRIX RotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(D3D11Camera::f3CameraPitchYawRoll.x, D3D11Camera::f3CameraPitchYawRoll.y, D3D11Camera::f3CameraPitchYawRoll.z);

    DirectX::XMVECTOR NewFoward     = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(D3D11Camera::f3DefaultFoward)), RotationMatrix));
    DirectX::XMVECTOR FocusPosition = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&(D3D11Camera::f3CameraWorldPosition)), NewFoward);

    D3D11Camera::ViewMatrix         = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&(D3D11Camera::f3CameraWorldPosition)), FocusPosition, DirectX::XMLoadFloat3(&(D3D11Camera::f3DefaultUp)));
    D3D11Camera::ProjectionMatrix   = DirectX::XMMatrixPerspectiveFovLH(D3D11Camera::fCameraFov, static_cast<float>(Window::GetClientWidth()) / static_cast<float>(Window::GetClientHeight()), 0.1f, 500.0f);
}