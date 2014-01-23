#pragma once

#include<windows.h>
#include<assert.h>
#include<d3d11.h>
#include<d3dx11.h>
#include<d3dcompiler.h>
#include"GeometryManager.h"
#include"TextureManager.h"
#include"ShaderManager.h"
#include"AnimationManager.h"

class AEngine;
class TextureManager;
class AnimationManager;
class GeometryManager;
class ShaderManager;

class RenderSystem
{
    struct ConstantBuffer
    {
        XMMATRIX m_World;
        XMMATRIX m_View;
        XMMATRIX m_Projection;
        XMMATRIX m_FinalTransform[100];
    };

public:
    RenderSystem(AEngine* pEngine);
    RenderSystem(const RenderSystem& other);
    ~RenderSystem();
    void Initialize(HWND handleWindow);
    void Shutdown();
    void RenderScene(GeometryManager* pGeoManager,TextureManager* pTextureManager, ShaderManager* pShaderManager, Camera* pCamera, AnimationManager* pAnimationManager);
    void RegisterGameObjects(std::vector<unsigned int>& gameId);
   
private:
    void InitDeviceAndSwapChain();
    void InitResources(); //TestCode
    HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut);
    void GenerateShaderAndLayout();

private:
    HWND m_hWnd;
    D3D_DRIVER_TYPE m_DriverType;
    D3D_FEATURE_LEVEL m_D3DFeatureLevel;
    ID3D11RasterizerState* m_wireframe;
    ID3D11RasterizerState* m_solid;
    AEngine* m_pEngine;
    //TestCode
    ID3D11Buffer*               m_pConstantBuffer;
    ConstantBuffer*             m_pTestConstantBuffer;
    ID3D11SamplerState*         m_pSamplerAF;   
    XMMATRIX*                   m_WorldMatrix;
public:   
    ID3D11Device* m_pD3DDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    IDXGISwapChain* m_pSwapChain;
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11Texture2D* m_pDepthStencil;
    ID3D11DepthStencilView* m_pDepthStencilView;
    std::vector<XMFLOAT4X4> m_TestWorldTransForms;

    
};