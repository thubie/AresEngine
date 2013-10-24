#pragma once

#include<windows.h>
#include<d3d11.h>
#include<d3dx11.h>


class RenderSystem
{
public:
    RenderSystem();
    RenderSystem(const RenderSystem& other);
    ~RenderSystem();
    void Initialize(HWND handleWindow);
    void Shutdown();

    void BeginRenderScene();
    void EndRenderScene();

private:
    void InitDeviceAndSwapChain();


private:
    HWND                    m_hWnd;
    D3D_DRIVER_TYPE         m_DriverType;
    D3D_FEATURE_LEVEL       m_D3DFeatureLevel;

public:   
    ID3D11Device*           m_pD3DDevice;
    ID3D11DeviceContext*    m_pImmediateContext;
    IDXGISwapChain*         m_pSwapChain;
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11Texture2D*        m_pDepthStencil;
    ID3D11DepthStencilView* m_pDepthStencilView;
};