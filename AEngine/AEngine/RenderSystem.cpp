#include"RenderSystem.h"

RenderSystem::RenderSystem()
{
    m_hWnd = nullptr;
    m_DriverType = D3D_DRIVER_TYPE_NULL;
    m_D3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    m_pD3DDevice = nullptr;
    m_pImmediateContext = nullptr;
    m_pSwapChain = nullptr;
    m_pRenderTargetView = nullptr;
    m_pDepthStencil = nullptr;
    m_pDepthStencilView = nullptr;
}

RenderSystem::RenderSystem(const RenderSystem& other)
{
    m_hWnd = other.m_hWnd;
    m_DriverType = other.m_DriverType;
    m_D3DFeatureLevel = other.m_D3DFeatureLevel;
    m_pD3DDevice = other.m_pD3DDevice;
    m_pImmediateContext = other.m_pImmediateContext;
    m_pSwapChain = other.m_pSwapChain;
    m_pRenderTargetView = other.m_pRenderTargetView;
}

RenderSystem::~RenderSystem()
{

}

void RenderSystem::Initialize(HWND handleWindow)
{
    m_hWnd = handleWindow;
    InitDeviceAndSwapChain();
}

void RenderSystem::Shutdown()
{
    if(m_pImmediateContext)
        m_pImmediateContext->ClearState();
    if( m_pDepthStencil ) 
        m_pDepthStencil->Release();
    if( m_pDepthStencilView ) 
        m_pDepthStencilView->Release();
    if(m_pRenderTargetView)
        m_pRenderTargetView->Release();
    if(m_pSwapChain)
        m_pSwapChain->Release();
    if(m_pImmediateContext)
        m_pImmediateContext->Release();
    if(m_pD3DDevice)
        m_pD3DDevice->Release();
}

void RenderSystem::InitDeviceAndSwapChain()
{
    HRESULT hr;
    RECT rc;
    GetClientRect(m_hWnd, &rc);
    unsigned int width = rc.right - rc.left;
    unsigned int height = rc.bottom - rc.top;

    unsigned createDeviceFlags = 0;
#ifdef _DEBUG
    //createDeviceFlags = 0;
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    unsigned int numDriverTypes = 3;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    unsigned int numFeatureLevels = 3;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd,sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = true;

    for(unsigned int driverTypesIndex = 0; driverTypesIndex < numDriverTypes; driverTypesIndex++)
    {
        m_DriverType = driverTypes[driverTypesIndex];
        hr = D3D11CreateDeviceAndSwapChain(NULL,m_DriverType,NULL, createDeviceFlags,featureLevels,numFeatureLevels,
            D3D11_SDK_VERSION,&sd,&m_pSwapChain,&m_pD3DDevice,&m_D3DFeatureLevel,&m_pImmediateContext);
        if(SUCCEEDED(hr))
            break;
    }
    //if(FAILED(hr))
    //    return hr;

    //Create a render target view
    ID3D11Texture2D* pBackbuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackbuffer);
    //if(FAILED(hr))
    //    return hr;
    
    hr = m_pD3DDevice->CreateRenderTargetView(pBackbuffer,NULL, &m_pRenderTargetView);
    pBackbuffer->Release();
    //if(FAILED(hr))
    //    return hr;

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView,NULL);

    //Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth,sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = m_pD3DDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
    //if(FAILED(hr))
    //    return hr;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV,sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_pD3DDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
    //if(FAILED(hr))
    //    return hr;

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    //Setup the viewport
    D3D11_VIEWPORT viewport;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    m_pImmediateContext->RSSetViewports(1 , &viewport);
}

void RenderSystem::BeginRenderScene()
{
    float clearColor[4] = {0.0f,0.125f,0.3f, 1.0f}; //Red,Green,Blue,alpha
    //Clear backbuffer and depthbuffer(to 1.0 max depth)
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderSystem::EndRenderScene()
{
    m_pSwapChain->Present(0,0);
}