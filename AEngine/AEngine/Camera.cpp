#include"Camera.h"

Camera::Camera()
{
    m_pCamPosition = nullptr;
    m_pCamTarget = nullptr;
    m_pUpDirection = nullptr;
    m_pProjectionMatrix = nullptr;
    m_pViewMatrix = nullptr;
    m_CamYaw = 0.0f;
    m_CamPitch = 0.0f;
    m_MoveLeftRight = 0.0f;
    m_MoveBackForward = 0.0f;
}

Camera::~Camera()
{
    //Free memory because we used malloc
    if(m_pViewMatrix != nullptr)
        _aligned_free(m_pViewMatrix);
    m_pViewMatrix = nullptr;

    if(m_pProjectionMatrix != nullptr)
        _aligned_free(m_pProjectionMatrix);
    m_pProjectionMatrix = nullptr;

    if(m_pUpDirection != nullptr)
        _aligned_free(m_pUpDirection);
    m_pUpDirection = nullptr;

    if(m_pCamTarget != nullptr)
        _aligned_free(m_pCamTarget);
    m_pCamTarget = nullptr;

    if(m_pCamPosition != nullptr)
        _aligned_free(m_pCamPosition);
    m_pCamPosition = nullptr;

    if(m_pCamRotationMatrix != nullptr)
        _aligned_free(m_pCamRotationMatrix);
    m_pCamRotationMatrix = nullptr;
}

void Camera::InitCamera(XMFLOAT3& camPos,XMFLOAT3& camTarget, XMFLOAT3& upDir)
{
    //_aligned_malloc alligns memory on x byte boundaries.
    m_pCamRotationMatrix = (XMMATRIX*) _aligned_malloc(sizeof(XMMATRIX), 16);
    m_pDefaultForward = (XMVECTOR*) _aligned_malloc(sizeof(XMVECTOR), 16);
    m_pDefaultRight = (XMVECTOR*) _aligned_malloc(sizeof(XMVECTOR), 16);
    m_pCamForward = (XMVECTOR*) _aligned_malloc(sizeof(XMVECTOR), 16);
    m_pCamRight = (XMVECTOR*) _aligned_malloc(sizeof(XMVECTOR), 16);

    *m_pDefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    *m_pDefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

    m_pCamPosition = (XMVECTOR*) _aligned_malloc(sizeof(XMVECTOR), 16);
    m_pCamTarget= (XMVECTOR*) _aligned_malloc(sizeof(XMFLOAT3), 16);
    m_pUpDirection = (XMVECTOR*) _aligned_malloc(sizeof(XMFLOAT3), 16);

    *m_pCamPosition = XMLoadFloat3(&camPos);
    *m_pCamTarget = XMLoadFloat3(&camTarget);
    *m_pUpDirection = XMLoadFloat3(&upDir);

    m_pViewMatrix = (XMMATRIX*) _aligned_malloc(sizeof(XMMATRIX), 16);
    m_pProjectionMatrix  = (XMMATRIX*) _aligned_malloc(sizeof(XMMATRIX), 16);

    *m_pProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1600 / (float)900, 0.01f, 10000.0f );
}

//set the camera position
void Camera::SetCameraPosition(XMFLOAT3* position)
{
    *m_pCamPosition = XMLoadFloat3(position);
}

//Set the camera direction
void Camera::SetCameraDirection(XMFLOAT3* direction)
{
    *m_pCamTarget = XMLoadFloat3(direction);
}

//return view matrix
XMMATRIX* Camera::GetViewMatrix()
{
    return m_pViewMatrix;
}

//return projection matrix
XMMATRIX* Camera::GetProjectionMatrix()
{
    return m_pProjectionMatrix;
}

//Update the ViewMatrix
void Camera::UpdateViewMatrix(XMVECTOR moveAndDirection)
{
    XMMATRIX RotateYTempMatrix;
    XMVECTOR CamForward;
    XMVECTOR CamRight;

    m_MoveLeftRight += moveAndDirection.m128_f32[0];
    m_MoveBackForward += moveAndDirection.m128_f32[1];
    m_CamYaw += moveAndDirection.m128_f32[2];
    m_CamPitch += moveAndDirection.m128_f32[3];

    *m_pCamRotationMatrix = XMMatrixRotationRollPitchYaw(m_CamPitch, m_CamYaw, 0.0f);
    *m_pCamTarget = XMVector3TransformCoord(*m_pDefaultForward, *m_pCamRotationMatrix);
    *m_pCamTarget = XMVector3Normalize(*m_pCamTarget);

	RotateYTempMatrix = XMMatrixRotationY(m_CamPitch);
    
    CamRight = XMVector3TransformCoord(*m_pDefaultRight, RotateYTempMatrix);
    *m_pUpDirection = XMVector3TransformCoord(*m_pUpDirection, RotateYTempMatrix);
    CamForward = XMVector3TransformCoord(*m_pDefaultForward, RotateYTempMatrix);

    *m_pCamPosition += m_MoveLeftRight * CamRight;
    *m_pCamPosition += m_MoveBackForward * CamForward;

    m_MoveLeftRight = 0.0f;
    m_MoveBackForward = 0.0f;

    *m_pCamTarget = *m_pCamPosition + *m_pCamTarget;
    
    *m_pViewMatrix = XMMatrixLookAtLH(*m_pCamPosition, *m_pCamTarget, *m_pUpDirection);
}