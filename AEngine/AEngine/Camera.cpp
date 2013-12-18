#include"Camera.h"

Camera::Camera()
{
    m_pCamPosition = nullptr;
    m_pCamDirection = nullptr;
    m_pUpDirection = nullptr;
    m_pProjectionMatrix = nullptr;
    m_pViewMatrix = nullptr;
}

Camera::Camera(const Camera& other)
{
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

    if(m_pCamDirection != nullptr)
        _aligned_free(m_pCamDirection);
    m_pCamDirection = nullptr;

    if(m_pCamPosition != nullptr)
        _aligned_free(m_pCamPosition);
    m_pCamPosition = nullptr;
}


void Camera::InitCamera(XMFLOAT3* pCamPos,XMFLOAT3* pCamDirection, XMFLOAT3* pUpDir)
{
    //Allign memory on 16byte boundaries.
    m_pCamPosition = (XMFLOAT3*) _aligned_malloc(sizeof(XMFLOAT3),16);
    *m_pCamPosition = *pCamPos;

    m_pCamDirection = (XMFLOAT3*) _aligned_malloc(sizeof(XMFLOAT3),16);
    *m_pCamDirection = *pCamDirection;

    m_pUpDirection = (XMFLOAT3*) _aligned_malloc(sizeof(XMFLOAT3),16);
    *m_pUpDirection = *pUpDir;

    m_pViewMatrix = (XMMATRIX*) _aligned_malloc(sizeof(XMMATRIX),16);

    //Calculate the projection matrix
    m_pProjectionMatrix  = (XMMATRIX*) _aligned_malloc(sizeof(XMMATRIX),16);
    *m_pProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1600 / (float)900, 0.01f, 1000.0f );
}

//set the camera position
void Camera::SetCameraPosition(XMFLOAT3* position)
{
    *m_pCamPosition = *position;
}

//Set the camera direction
void Camera::SetCameraDirection(XMFLOAT3* direction)
{
    *m_pCamDirection = *direction;
}


//test code for input

void Camera::MoveCameraForward()
{
    m_pCamPosition->z += 1;
}

void Camera::MoveCameraBackward()
{
    m_pCamPosition->z -= 1;
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
void Camera::UpdateViewMatrix()
{
    XMVECTOR camPos = XMLoadFloat3(m_pCamPosition);
    XMVECTOR camDir = XMLoadFloat3(m_pCamDirection);
    XMVECTOR upDir  = XMLoadFloat3(m_pUpDirection);
    XMMATRIX ViewTemp = XMMatrixLookAtLH(camPos, camDir, upDir);
    *m_pViewMatrix = ViewTemp;
}