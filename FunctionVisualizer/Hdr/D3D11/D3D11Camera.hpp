#pragma once

#include <Windows.h>
#include <DirectXMath.h>



class D3D11Camera
{
public:
    static void InitializeDefault();        //Is called within d3d11 initialization in order to apply default camera settings.

    static void SetLensInformation(float fFov, bool bDegrees);  
    static void SetCameraPosition(float fX, float fY, float fZ);
    static void SetCameraOrientation(float fPitch, float fYaw, float fRoll);

    static void OffsetCameraPosition(float fDeltaX, float fDeltaY, float fDeltaZ);
    static void OffsetCameraOrientation(float fPitch, float fYaw, float fRoll, bool bDegrees);

    static bool CameraSettingsUpdated();

    static DirectX::XMMATRIX GetViewMatrix();
    static DirectX::XMMATRIX GetProjectionMatrix();
    static DirectX::XMMATRIX GetViewProjectionMatrix();

private:
    static void UpdateMatrices();


    static bool bFreecamMovement;

    static constexpr DirectX::XMFLOAT3 f3DefaultRight   = { 1.0f, 0.0f, 0.0f };
    static constexpr DirectX::XMFLOAT3 f3DefaultUp      = { 0.0f, 1.0f, 0.0f }; 
    static constexpr DirectX::XMFLOAT3 f3DefaultFoward  = { 0.0f, 0.0f, 1.0f };

    static DirectX::XMFLOAT3 f3CameraWorldPosition;
    static DirectX::XMFLOAT3 f3CameraPitchYawRoll;

    static float    fCameraFov;         //In radians
    static UINT     uiCameraSpeed;
    static UINT     uiCameraSensitivity; 

    static DirectX::XMMATRIX ViewMatrix;
    static DirectX::XMMATRIX ProjectionMatrix;
};