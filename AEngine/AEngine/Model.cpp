#include"Model.h"

Model::Model()
{
    m_pVertexShader = nullptr;
    m_pPixelShader = nullptr;
    m_pVertexLayout = nullptr;
    m_pVertexBuffer = nullptr;
    m_pIndexBuffer = nullptr;
    m_pConstantBuffer = nullptr;
    m_pTestConstantBuffer = nullptr;
    m_pImmediateContext = nullptr;;
    m_pD3dDevice = nullptr;
}
 
Model::Model(const Model& other)
{}

Model::~Model()
{}

void Model::SetGraphicsDeviceAndContext(ID3D11DeviceContext* immediateContext,ID3D11Device* d3dDevice)
{
    m_pImmediateContext = immediateContext;
    m_pD3dDevice = d3dDevice;
}

void Model::Render(Camera* pCamera)
{
    static float t = 0.0f;
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if( dwTimeStart == 0 )
        dwTimeStart = dwTimeCur;
    t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    
    //1st cube rotate around the origin
    *m_WorldMatrix = XMMatrixRotationY(t);
    m_pTestConstantBuffer->m_World = XMMatrixTranspose(*m_WorldMatrix);
    m_pTestConstantBuffer->m_View = XMMatrixTranspose(*(pCamera->GetViewMatrix()));
    m_pTestConstantBuffer->m_Projection = XMMatrixTranspose(*(pCamera->GetProjectionMatrix()));
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL,m_pTestConstantBuffer, 0, 0);

    // Set primitive topology. This needs to go to RenderSystem
    UINT stride = sizeof(PosNormUV);
    UINT offset = 0;

    
    
}

void Model::SetVertexAndIndexBuffer(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer)
{
    m_pVertexBuffer = vertexBuffer;
    m_pIndexBuffer = indexBuffer; 
}

void Model::CreateVertexIndexBuffer(unsigned int vertexCount,unsigned int indexCount,PosNormUV* vertices, unsigned int* indices )
{
    HRESULT hr;
    this->m_numVertices = vertexCount;
    this->m_numIndices = indexCount;
    //Create the vertexBuffer.
    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(PosNormUV) * vertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr = m_pD3dDevice->CreateBuffer( &bd, &InitData, &m_pVertexBuffer );

    assert(!FAILED(hr)); 

    //Create the index buffer.
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(unsigned int) * indexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = m_pD3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
    
    assert(!FAILED(hr));

    //Create the constant buffer need to put this in another function...
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pD3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
    
    assert(!FAILED(hr));

    //Initialize the world matrix
    m_WorldMatrix = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX),16);
    *m_WorldMatrix = XMMatrixIdentity();

    m_pTestConstantBuffer = (ConstantBuffer*) _aligned_malloc(sizeof(ConstantBuffer), 16);

    //D3D11_RASTERIZER_DESC wireframedsc;
    //ZeroMemory(&wireframedsc,sizeof(D3D11_RASTERIZER_DESC));
    //wireframedsc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_SOLID;  D3D11_FILL_WIREFRAME;
    //wireframedsc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_BACK   D3D10_CULL_FRONT
    //wireframedsc.FrontCounterClockwise = false;
    //this->m_pD3dDevice->CreateRasterizerState(&wireframedsc,&m_wireframe);

    ////Create Textures
    //hr = D3DX11CreateShaderResourceViewFromFile(m_pD3dDevice,
    //    L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\pants.dds", NULL,NULL,&m_pTexture,NULL);
    //assert(!FAILED(hr));

    ////Create Sampler
    //D3D11_SAMPLER_DESC sampDesc;
    //ZeroMemory(&sampDesc, sizeof(sampDesc));
    //sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    //sampDesc.MaxAnisotropy = 16;
    //sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    //sampDesc.MinLOD = 0;
    //sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    //this->m_pD3dDevice->CreateSamplerState(&sampDesc,&m_pSamplerAF);

    
}

void Model::CleanUpModelData()
{
    //Clean up model data buffers and shaders
    if(m_pConstantBuffer) 
        m_pConstantBuffer->Release();
    if(m_pVertexBuffer) 
        m_pVertexBuffer->Release();
    if(m_pIndexBuffer) 
        m_pIndexBuffer->Release();
    if(m_pVertexLayout) 
        m_pVertexLayout->Release();
    if(m_pVertexShader) 
        m_pVertexShader->Release();
    if(m_pPixelShader) 
        m_pPixelShader->Release();
}

//Need to put this in shader manager!!!!!!
void Model::GenerateShaderAndLayout()
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
    hr = m_pD3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),NULL, &m_pVertexShader);
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
    hr = m_pD3dDevice->CreateInputLayout(layout,numElements,pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),&m_pVertexLayout);
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
    hr = m_pD3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize(), NULL,&m_pPixelShader);
    assert(!FAILED(hr));

    pPSBlob->Release();
    
}

HRESULT Model::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut)
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