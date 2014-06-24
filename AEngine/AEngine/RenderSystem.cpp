#include"RenderSystem.h"


RenderSystem::RenderSystem(AEngine* pEngine)
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
    m_pEngine = pEngine;
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

void RenderSystem::RegisterGameObjects(std::vector<unsigned int>& gameId)
{
    unsigned int numGameObjects = gameId.size();
    unsigned int numObjectsX = sqrtl(numGameObjects);
    unsigned int numObjectsZ = numGameObjects / numObjectsX;

    //clear current gameobjects
    m_TestWorldTransForms.clear();

    m_TestWorldTransForms.reserve(numGameObjects);
    float x,y,z;
    XMMATRIX currentWorld;
    y = 0;
    for(unsigned int i = 0; i < numObjectsZ; ++i)
    {
        z = i * 75;
        for(unsigned int j = 0; j < numObjectsX; ++j)
        {
            x = j * 75;
            currentWorld = XMMatrixTranslation(x, y, z);
            XMFLOAT4X4 worldTrans;
            XMStoreFloat4x4(&worldTrans, currentWorld);
            m_TestWorldTransForms.push_back(worldTrans);
        }
    }
}

void RenderSystem::Initialize(HWND handleWindow, const char* currentDir)
{
    m_hWnd = handleWindow;
    InitDeviceAndSwapChain();
    InitSubSystems(currentDir);
    InitResources();
}

void RenderSystem::InitSubSystems(const char* currentDir)
{
    m_pGeoManager = new GeometryManager(this, currentDir);
    m_pGeoManager->Initialize(m_pD3DDevice, m_pImmediateContext);
    m_pTextureManager = new TextureManager(m_pD3DDevice, m_pImmediateContext, this, currentDir);
    m_pShaderManager = new ShaderManager(m_pD3DDevice, m_pImmediateContext, this, currentDir);
}

void RenderSystem::Shutdown()
{
    if(m_pImmediateContext)
        m_pImmediateContext->ClearState();

    if( m_pDepthStencil ) 
    {
        m_pDepthStencil->Release();
        m_pDepthStencil = nullptr;
    }

    if(m_pDepthStencilView) 
    {
       m_pDepthStencilView->Release();
       m_pDepthStencilView = nullptr;
    }

    if(m_pRenderTargetView)
    {
        m_pRenderTargetView->Release();
        m_pRenderTargetView = nullptr;
    }

    if(m_pSwapChain)
    {
        m_pSwapChain->Release();
        m_pSwapChain = nullptr;
    }

    if(m_pImmediateContext)
    {
        m_pImmediateContext->Release();
        m_pImmediateContext = nullptr;
    }

    if(m_pD3DDevice)
    {
        m_pD3DDevice->Release();
        m_pD3DDevice = nullptr;
    }
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
    createDeviceFlags = 0;
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
    sd.BufferDesc.RefreshRate.Numerator = 0;//60;
    sd.BufferDesc.RefreshRate.Denominator = 0;//1;
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
    assert(!FAILED(hr)); 

    //Create a render target view
    ID3D11Texture2D* pBackbuffer = NULL;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackbuffer);
    assert(!FAILED(hr)); 

    hr = m_pD3DDevice->CreateRenderTargetView(pBackbuffer,NULL, &m_pRenderTargetView);
    pBackbuffer->Release();
    assert(!FAILED(hr));

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
    assert(!FAILED(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV,sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_pD3DDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
    assert(!FAILED(hr));

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

void RenderSystem::InitResources()
{
    ////Create Textures
    HRESULT hr;

    //Create Renderstate
    D3D11_RASTERIZER_DESC solidDesc;
    ZeroMemory(&solidDesc,sizeof(D3D11_RASTERIZER_DESC));
    solidDesc.FillMode = D3D11_FILL_SOLID; 
    solidDesc.CullMode = D3D11_CULL_BACK; 
    solidDesc.FrontCounterClockwise = false;
    this->m_pD3DDevice->CreateRasterizerState(&solidDesc, &m_solid);

    D3D11_RASTERIZER_DESC wireframeDesc;
    ZeroMemory(&wireframeDesc,sizeof(D3D11_RASTERIZER_DESC));
    wireframeDesc.FillMode = D3D11_FILL_WIREFRAME; 
    wireframeDesc.CullMode = D3D11_CULL_BACK; 
    wireframeDesc.FrontCounterClockwise = false;
    this->m_pD3DDevice->CreateRasterizerState(&wireframeDesc,&m_wireframe);

    //Create Sampler
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.MaxAnisotropy = 16;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    this->m_pD3DDevice->CreateSamplerState(&sampDesc,&m_pSamplerAF);

    //Create constant buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd,sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pD3DDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
    assert(!FAILED(hr));

    //Initialize the world matrix
    //m_WorldMatrix = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX),16);
    //*m_WorldMatrix = XMMatrixIdentity();
    m_pTestConstantBuffer = (ConstantBuffer*) _aligned_malloc(sizeof(ConstantBuffer), 16);
    m_pImmediateContext->RSSetState(m_solid);
    
}

void RenderSystem::SubmitImportingTasks()
{
    m_pEngine->SubmitImportingTask(m_pGeoManager->ImportAssetTask());
    m_pEngine->SubmitImportingTask(m_pTextureManager->ImportTextures());
    m_pEngine->SubmitImportingTask(m_pShaderManager->CreatePixelShaderTask());
    m_pEngine->SubmitImportingTask(m_pShaderManager->CreateVertexShaderTask());
}

//Submit messages to the engine
void RenderSystem::SubmitMessage(Message msg)
{
    m_pEngine->SubmitMessage(msg);
}

//Renders the Scene
void RenderSystem::RenderScene(XMMATRIX* pView, XMMATRIX* pProjection, std::vector<SkeletonCBufferData>* finalTransforms)
{
    //Clear the renderTarget
    float clearColor[4] = {0.0f,0.125f,0.3f, 1.0f}; 
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    XMMATRIX world;
    int count = m_TestWorldTransForms.size();
    unsigned int indicesCount = 0;
    XMMATRIX* finalTransformsCbuffer = m_pTestConstantBuffer->m_FinalTransform;
    std::vector<SkeletonCBufferData>* pSkeletonBufferData = finalTransforms;
    
    m_pTestConstantBuffer->m_View = XMMatrixTranspose(*pView);
    m_pTestConstantBuffer->m_Projection = XMMatrixTranspose(*pProjection);
    m_pShaderManager->SetVertexShader(0); //hardcoded for the skinning shader
    m_pShaderManager->SetPixelShader(0);
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->PSSetSamplers(0,1,&m_pSamplerAF);

    for(int i = 0; i < count ; ++i)
    {        
        world = XMLoadFloat4x4(&m_TestWorldTransForms.at(i));  
        m_pTestConstantBuffer->m_World = XMMatrixTranspose(world);
        XMFLOAT4X4* finalTransforms = pSkeletonBufferData->at(i).skeletonData;
        memcpy((void*)finalTransformsCbuffer, (void*)finalTransforms, sizeof(XMMATRIX) * 100);
        m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL,m_pTestConstantBuffer, 0, 0);
        
        for(int i = 0; i < 4; ++i)
        {
            m_pGeoManager->SetSubmeshIndexed(i, &indicesCount);
            m_pTextureManager->SetTexture(i);           
            m_pImmediateContext->DrawIndexed(indicesCount, 0, 0);
        }
    }
    m_pSwapChain->Present(0,0);
    Message doneRendering;
    doneRendering.MessageType = RENDERING_DONE;
    SubmitMessage(doneRendering);
}


