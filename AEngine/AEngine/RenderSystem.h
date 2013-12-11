#pragma once

#include<windows.h>
#include<assert.h>
#include<d3d11.h>
#include<d3dx11.h>
#include<d3dcompiler.h>
#include"GeometryManager.h"

class AEngine;
class GeometryManager;
class TextureManager;

class RenderSystem
{
    struct ConstantBuffer
    {
        XMMATRIX m_World;
        XMMATRIX m_View;
        XMMATRIX m_Projection;
    };

public:
    RenderSystem();
    RenderSystem(const RenderSystem& other);
    ~RenderSystem();
    void Initialize(HWND handleWindow);
    void Shutdown();

    //Testing purpose
    void BeginRenderScene();
    void RenderScene(GeometryManager* pGeoManager,TextureManager* pTextureManager,Camera* pCamera);
    void EndRenderScene();

private:
    void InitDeviceAndSwapChain();
    void InitResources(); //TestCode
    HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut);
    void GenerateShaderAndLayout();

private:
    HWND m_hWnd;
    D3D_DRIVER_TYPE m_DriverType;
    D3D_FEATURE_LEVEL m_D3DFeatureLevel;


public:   
    ID3D11Device* m_pD3DDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    IDXGISwapChain* m_pSwapChain;
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11Texture2D* m_pDepthStencil;
    ID3D11DepthStencilView* m_pDepthStencilView;

    //TestCode
    ID3D11Buffer*               m_pConstantBuffer;
    ConstantBuffer*             m_pTestConstantBuffer;
    ID3D11SamplerState*         m_pSamplerAF;
    ID3D11ShaderResourceView**  m_pTextures; 

    ID3D11VertexShader*         m_pVertexShader;
    ID3D11PixelShader*          m_pPixelShader;
    ID3D11InputLayout*          m_pVertexLayout;
    ID3D11RasterizerState*      m_wireframe;

    XMMATRIX*                   m_WorldMatrix;
};