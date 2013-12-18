#pragma once

#include<Windows.h>
#include<xnamath.h>


class Camera
{
  
public:
    Camera();
    Camera(const Camera& other);
    ~Camera();

    void InitCamera(XMFLOAT3* camPos,XMFLOAT3* camDirection, XMFLOAT3* upDir);

    XMMATRIX* GetProjectionMatrix();
    XMMATRIX* GetViewMatrix();
    void UpdateViewMatrix();
    void SetCameraPosition(XMFLOAT3* position);
    void SetCameraDirection(XMFLOAT3* direction);

    void MoveCameraForward();
    void MoveCameraBackward();

private:
    XMMATRIX* m_pViewMatrix;
    XMMATRIX* m_pProjectionMatrix;
    XMFLOAT3* m_pCamPosition;
    XMFLOAT3* m_pCamDirection;
    XMFLOAT3* m_pUpDirection;
};