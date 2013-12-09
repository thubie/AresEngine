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
    InitResources();
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

//Need to refactor this later again mostly test code for now
void RenderSystem::InitResources()
{
    //Create Textures
    HRESULT hr;
    m_pTextures = new ID3D11ShaderResourceView*[4];
    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\head.dds", NULL,NULL,&m_pTextures[0],NULL);
    assert(!FAILED(hr));

    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\jacket.dds", NULL,NULL,&m_pTextures[1],NULL);
    assert(!FAILED(hr));

    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\pants.dds", NULL,NULL,&m_pTextures[2],NULL);
    assert(!FAILED(hr));

    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\upBodyC.dds", NULL,NULL,&m_pTextures[3],NULL);
    assert(!FAILED(hr));

    //Generate shaders and layout
    GenerateShaderAndLayout();

    //Create Renderstate
    D3D11_RASTERIZER_DESC wireframedsc;
    ZeroMemory(&wireframedsc,sizeof(D3D11_RASTERIZER_DESC));
    wireframedsc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_SOLID;  D3D11_FILL_WIREFRAME;
    wireframedsc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_BACK   D3D10_CULL_FRONT
    wireframedsc.FrontCounterClockwise = false;
    this->m_pD3DDevice->CreateRasterizerState(&wireframedsc,&m_wireframe);

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
    m_WorldMatrix = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX),16);
    *m_WorldMatrix = XMMatrixIdentity();
    m_pTestConstantBuffer = (ConstantBuffer*) _aligned_malloc(sizeof(ConstantBuffer), 16);

}

void RenderSystem::BeginRenderScene()
{
    float clearColor[4] = {0.0f,0.125f,0.3f, 1.0f}; //Red,Green,Blue,alpha
    //Clear backbuffer and depthbuffer(to 1.0 max depth)
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void RenderSystem::RenderScene(GeometryObject* meshes, unsigned int Count, Camera* pCamera)
{
    GeometryObject* testMeshes = meshes;
    unsigned int meshCount = Count;
    //test rendering run

    static float t = 0.0f;
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if( dwTimeStart == 0 )
        dwTimeStart = dwTimeCur;
    t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    
    //rotate the model around the origin
    *m_WorldMatrix = XMMatrixRotationY(t);
    m_pTestConstantBuffer->m_World = XMMatrixTranspose(*m_WorldMatrix);
    m_pTestConstantBuffer->m_View = XMMatrixTranspose(*(pCamera->GetViewMatrix()));
    m_pTestConstantBuffer->m_Projection = XMMatrixTranspose(*(pCamera->GetProjectionMatrix()));
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL,m_pTestConstantBuffer, 0, 0);

    for(int i = 0; i < 4/*meshCount*/; ++i)
    {
        UINT stride = sizeof(PosNormUV);
        UINT offset = 0;

        m_pImmediateContext->IASetVertexBuffers( 0, 1, &testMeshes[i].vertexBuffer, &stride, &offset );
        m_pImmediateContext->IASetIndexBuffer(testMeshes[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);    
        m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
	    m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	
        m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0 );
        m_pImmediateContext->PSSetShaderResources(0,1,&m_pTextures[i]);
        m_pImmediateContext->PSSetSamplers(0,1,&m_pSamplerAF);

        m_pImmediateContext->RSSetState(m_wireframe);
        m_pImmediateContext->DrawIndexed(testMeshes[i].indicesCount, 0, 0);
    }
}

void RenderSystem::EndRenderScene()
{
    m_pSwapChain->Present(0,0);
}

void RenderSystem::GenerateShaderAndLayout()
{
    HRESULT hr;

    ID3DBlob* pVSBlob = NULL;
    
    //Compile the vertex shader
    hr = CompileShaderFromFile(L"StaticMesh.fx", "VS", "vs_4_0", &pVSBlob);
    if(FAILED(hr))
    {
        MessageBox(NULL,L"The FX file cannot be compiled. Please run this executable from the directory that contains the FX file.",L"Error", MB_OK );
    }

    //Create the vertex shader
    hr = m_pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),NULL, &m_pVertexShader);
    if( FAILED( hr ) )	
		pVSBlob->Release();

    //Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    unsigned int numElements = ARRAYSIZE(layout);

    //Create the input layout
    hr = m_pD3DDevice->CreateInputLayout(layout,numElements,pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),&m_pVertexLayout);
    assert(!FAILED(hr));

    pVSBlob->Release();

    //Set the input layout
    m_pImmediateContext->IASetInputLayout(m_pVertexLayout);
    
    //Compile pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile(L"StaticMesh.fx", "PS","ps_4_0", &pPSBlob);
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
    }

    //Create the Pixel shader
    hr = m_pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize(), NULL,&m_pPixelShader);
    assert(!FAILED(hr));

    pPSBlob->Release();
}

HRESULT RenderSystem::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile(szFileName,NULL,NULL,szEntryPoint,szShaderModel,
        dwShaderFlags,0,NULL,ppBlobOut,&pErrorBlob,NULL);
    if(FAILED(hr))
    {
        if(pErrorBlob != NULL)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        if(pErrorBlob) 
            pErrorBlob->Release();
        return hr;
    }
    if(pErrorBlob)
        pErrorBlob->Release();

    return S_OK;
}

