#include"Model.h"

Model::Model(ID3D11DeviceContext* immediateContext,ID3D11Device* d3dDevice)
{
    m_pVertexShader = nullptr;
    m_pPixelShader = nullptr;
    m_pVertexLayout = nullptr;
    m_pVertexBuffer = nullptr;
    m_pIndexBuffer = nullptr;
    m_pConstantBuffer = nullptr;
    m_pTestConstantBuffer = nullptr;
    m_pImmediateContext = immediateContext;
    m_pD3dDevice = d3dDevice;
}
 
Model::Model(const Model& other)
{}

Model::~Model()
{}

void Model::Render(Camera* pCamera)
{
    static float t = 0.0f;
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if( dwTimeStart == 0 )
        dwTimeStart = dwTimeCur;
    t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    
    //1st cube rotate around the origin
    XMMATRIX WorldMatrix = XMMatrixRotationY(t);

    // 2nd Cube rotate arond origin
    XMMATRIX mSpin = XMMatrixRotationZ( -t );
    XMMATRIX mOrbit = XMMatrixRotationY( -t * 2.0f );
	XMMATRIX mTranslate = XMMatrixTranslation( -4.0f, 0.0f, 0.0f );
	XMMATRIX mScale = XMMatrixScaling( 0.3f, 0.3f, 0.3f );


    m_pTestConstantBuffer->m_World = XMMatrixTranspose(WorldMatrix);
    m_pTestConstantBuffer->m_View = XMMatrixTranspose(*(pCamera->GetViewMatrix()));
    m_pTestConstantBuffer->m_Projection = XMMatrixTranspose(*(pCamera->GetProjectionMatrix()));
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL,m_pTestConstantBuffer, 0, 0);

    // Render a the cube 1
	m_pImmediateContext->VSSetShader( m_pVertexShader, NULL, 0 );
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetShader( m_pPixelShader, NULL, 0 );
    m_pImmediateContext->DrawIndexed(36, 0, 0); // 36 vertices needed for 12 triangles in a triangle list
}

void Model::InitModel()
{

    HRESULT hr;

    ID3DBlob* pVSBlob = NULL;
    
    //Compile the vertex shader
    hr = CompileShaderFromFile(L"Test.fx", "VS", "vs_4_0", &pVSBlob);
    if(FAILED(hr))
    {
        MessageBox(NULL,L"The FX file cannot be compiled. Please run this executable from the directory that contains the FX file.",L"Error", MB_OK );
    }

    //Create the vertex shader
    hr = m_pD3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),NULL, &m_pVertexShader);
    if( FAILED( hr ) )
	{	
		pVSBlob->Release();
        //return hr;
	}

    //Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    unsigned int numElements = ARRAYSIZE(layout);

    //Create the input layout
    hr = m_pD3dDevice->CreateInputLayout(layout,numElements,pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),&m_pVertexLayout);

    pVSBlob->Release();
    //if(FAILED(hr))
        //return hr;

    //Set the input layout
    m_pImmediateContext->IASetInputLayout(m_pVertexLayout);
    
    //Compile pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile(L"Test.fx", "PS","ps_4_0", &pPSBlob);
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        //return hr;
    }

    //Create the Pixel shader
    hr = m_pD3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize(), NULL,&m_pPixelShader);
    pPSBlob->Release();
    //if( FAILED( hr ) )
    //   return hr;


    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = m_pD3dDevice->CreateBuffer( &bd, &InitData, &m_pVertexBuffer );
    //if( FAILED( hr ) )
        //return hr;

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

    //Create index buffer
    WORD indices[] =
    {
        3,1,0,  2,1,3,

        0,5,4,  1,5,0,

        3,4,7,  0,4,3,

        1,6,5,  2,6,1,

        2,7,6,  3,7,2,

        6,4,5, 7,4,6,
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = m_pD3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
    //if( FAILED( hr ) )
        //return hr;

    //Set index buffer
    m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    //Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pD3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
    //if( FAILED( hr ) )
        //return hr;

    //Initialize the world matrix
    m_WorldMatrix = XMMatrixIdentity();

    m_pTestConstantBuffer = (ConstantBuffer*) _aligned_malloc(sizeof(ConstantBuffer), 16);

}

void Model::CleanUpModel()
{
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