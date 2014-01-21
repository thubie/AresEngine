#include"ShaderManager.h"

ShaderManager:: ShaderManager(ID3D11Device* pD3DDevice,ID3D11DeviceContext* pImmediateContext, AEngine* pEngine)
 {
     m_pEngine = pEngine;

     m_pD3DDevice = pD3DDevice;
     m_pD3DDevice->AddRef();

     m_pImmediateContext = pImmediateContext;
     m_pImmediateContext->AddRef();

     m_pVertexShaders = new std::vector<VertexShaderObject>;
     m_pPixelShaders = new std::vector<PixelShaderObject>;
     finishedVertexShader = false;
     finishedPixelShader = false;
 }
 
 ShaderManager::~ShaderManager()
 {
     CleanUpResources();
 }

void ShaderManager::CleanUpResources()
{
     if(m_pD3DDevice != nullptr)
     {
         m_pD3DDevice->Release();
         m_pD3DDevice = nullptr;
     }

     if(m_pImmediateContext != nullptr)
     {
         m_pImmediateContext->Release();
         m_pImmediateContext = nullptr;
     }

     for(int i = 0; i < m_pVertexShaders->size(); i++)
     {
         m_pVertexShaders->at(i).vertexShader->Release();
         m_pVertexShaders->at(i).inputLayout->Release();
     }

     for(int i = 0; i < m_pPixelShaders->size(); i++)
     {
         m_pPixelShaders->at(i).pixelShader->Release();
     }
}

void ShaderManager::SetVertexShader(unsigned int vertexShaderId)
{
    VertexShaderObject vertexShader;
    vertexShader = m_pVertexShaders->at(vertexShaderId);

    m_pImmediateContext->IASetInputLayout(vertexShader.inputLayout);
    m_pImmediateContext->VSSetShader(vertexShader.vertexShader, NULL,0);
}

void ShaderManager::SetPixelShader(unsigned int pixelShaderId)
{
    PixelShaderObject pixelShader;
    pixelShader = m_pPixelShaders->at(pixelShaderId);
    m_pImmediateContext->PSSetShader(pixelShader.pixelShader, NULL, 0);
}

Task* ShaderManager::CreateVertexShaderTask(const char* pVertexPath)
{
    Task* task = new Task;
    TaskData* data = new TaskData;
    data->parameter1 = (void*)m_pVertexShaders;
    data->parameter2 = (void*)pVertexPath;
    data->parameter3 = nullptr;
    data->parameter4 = nullptr;
    data->parameter5 = nullptr;

    task->thisPointer = this;
    task->pTaskData = data;
    task->callback = DoneCreatingVertexShader;
    task->taskFunction = StartCreateVertexShader;
    return task; 
}

Task* ShaderManager::CreatePixelShaderTask(const char* pPixelPath)
{
    Task* task = new Task;
    TaskData* data = new TaskData;
    data->parameter1 = (void*)m_pPixelShaders;
    data->parameter2 = (void*)pPixelPath;
    data->parameter3 = nullptr;
    data->parameter4 = nullptr;
    data->parameter5 = nullptr;

    task->thisPointer = this;
    task->pTaskData = data;
    task->callback = DoneCreatingPixelShader;
    task->taskFunction = StartCreatePixelShader;
    return task; 
}

void ShaderManager::StartCreateVertexShader(TaskData* pData, void* thisPointer)
{
    ShaderManager* self = static_cast<ShaderManager*>(thisPointer);
    self->CreateVertexShader(pData);
}

void ShaderManager::StartCreatePixelShader(TaskData* pData, void* thisPointer)
{
    ShaderManager* self = static_cast<ShaderManager*>(thisPointer);
    self->CreatePixelShader(pData);
}

void ShaderManager::CreateVertexShader(TaskData* data)
{
    std::vector<VertexShaderObject>* pVertexShaders; 
    pVertexShaders = static_cast<std::vector<VertexShaderObject>*>(data->parameter1);
    const char* path = (char*)data->parameter2;
    const int length = 1024;
    size_t convertedChar;
    WCHAR wcharPath[length];
    mbstowcs_s(&convertedChar,wcharPath,path,length);
    HRESULT hr;
    ID3DBlob* pVSBlob = NULL;
    VertexShaderObject vertexShaderObj;

    //Compile the vertex shader
    CompileShaderFromFile(wcharPath, "VS", "vs_5_0", &pVSBlob);
   
    //Create the vertex shader
    hr = m_pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),NULL, &vertexShaderObj.vertexShader);
    assert(!FAILED(hr));

    //Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
        {"POSITION"   , 0, DXGI_FORMAT_R32G32B32_FLOAT     , 0, 0                           , D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL"     , 0, DXGI_FORMAT_R32G32B32_FLOAT     , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD"   , 0, DXGI_FORMAT_R32G32_FLOAT        , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WEIGHTS"    , 0, DXGI_FORMAT_R32G32B32A32_FLOAT  , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    unsigned int numElements = ARRAYSIZE(layout);

    //Create the input layout
    hr = m_pD3DDevice->CreateInputLayout(layout,numElements,pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),&vertexShaderObj.inputLayout);
    assert(!FAILED(hr));
    pVSBlob->Release();

    vertexShaderObj.id = 1; //for now hardcoded.

    pVertexShaders->push_back(vertexShaderObj);
    bool finish = true;
}

void ShaderManager::CreatePixelShader(TaskData* data)
{
    std::vector<PixelShaderObject>* pPixelShaders;
    pPixelShaders = static_cast<std::vector<PixelShaderObject>*>(data->parameter1);
    PixelShaderObject pixelShaderObj;
    HRESULT hr;
    const char* path = (char*)data->parameter2;
    const int length = 1024;
    size_t convertedChar;
    WCHAR wcharPath[length];
    mbstowcs_s(&convertedChar,wcharPath,path,length);

     //Compile pixel shader
    ID3DBlob* pPSBlob = NULL;
    CompileShaderFromFile(wcharPath, "PS","ps_5_0", &pPSBlob);

    //Create the Pixel shader
    hr = m_pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize(), NULL,&pixelShaderObj.pixelShader);
    assert(!FAILED(hr));
    pPSBlob->Release();
    pixelShaderObj.id = 1; //HardCoded for now
    pPixelShaders->push_back(pixelShaderObj);
    bool finish = true;
}

void ShaderManager::DoneCreatingVertexShader(void* thispointer, void* task)
{
    ShaderManager* self = static_cast<ShaderManager*>(thispointer);
    Message doneImporting;
    doneImporting.MessageType = IMPORT_VERTEXSHADER_DONE;
    self->m_pEngine->SubmitMessage(doneImporting);
}

void ShaderManager::DoneCreatingPixelShader(void* thispointer, void* task)
{
    ShaderManager* self = static_cast<ShaderManager*>(thispointer);
    Message doneImporting;
    doneImporting.MessageType = IMPORT_PIXELSHADER_DONE;
    self->m_pEngine->SubmitMessage(doneImporting);
}

void ShaderManager::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut)
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
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
    }
    if(pErrorBlob)
        pErrorBlob->Release();

}