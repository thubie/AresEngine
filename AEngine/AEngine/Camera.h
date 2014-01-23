#pragma once

#include<Windows.h>
#include<xnamath.h>


class Camera
{
  
public:
    Camera();
    ~Camera();

    void InitCamera(XMFLOAT3& camPos, XMFLOAT3& camTarget, XMFLOAT3& upDir);
    void UpdateViewMatrix(XMVECTOR moveAndDirection);
    void SetCameraPosition(XMFLOAT3* position);
    void SetCameraDirection(XMFLOAT3* direction);
    XMMATRIX* GetProjectionMatrix();
    XMMATRIX* GetViewMatrix();

private:
    XMMATRIX* m_pViewMatrix;
    XMMATRIX* m_pProjectionMatrix;
    XMVECTOR* m_pCamPosition;
    XMVECTOR* m_pCamTarget;
    XMVECTOR* m_pUpDirection;
    
    XMVECTOR* m_pDefaultForward;
    XMVECTOR* m_pDefaultRight;
    XMVECTOR* m_pCamForward;
    XMVECTOR* m_pCamRight;

    XMMATRIX* m_pCamRotationMatrix;

    float m_MoveLeftRight;
    float m_MoveBackForward;
    float m_CamYaw;
    float m_CamPitch;
};