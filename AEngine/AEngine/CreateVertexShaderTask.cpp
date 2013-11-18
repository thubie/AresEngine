#include"CreateVertexShaderTask.h"



CreateVertexShaderTask::CreateVertexShaderTask(VertexShaderCreationData* pCreationData, ShaderGenerator* pShaderGenerator,ID3D11Device* pDevice)
{
    m_pCreationData = pCreationData;   
    m_pShaderGenerator = pShaderGenerator;
    m_pDevice = pDevice;
}

CreateVertexShaderTask::~CreateVertexShaderTask()
{

}

void CreateVertexShaderTask::RunTask()
{
    HRESULT hr;
    ID3DBlob* pVSBlob = nullptr;

    //Compile shader from file
    m_pShaderGenerator->CompileShaderFromFile(m_pCreationData->pShaderFile,"VS", "vs_4_0", &pVSBlob);
    
    //Create the vertexShader
    hr = m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize(),NULL, &m_pCreationData->pVertexShader);
    assert(FAILED(hr));

    //Create the input layout
    m_pDevice->CreateInputLayout(m_pCreationData->pElementDescription,m_pCreationData->numElements,
        pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pCreationData->pInputLayout);

    //Release the blob
    pVSBlob->Release();
}