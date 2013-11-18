#include"CreatePixelShaderTask.h"

CreatePixelShaderTask::CreatePixelShaderTask(PixelShaderCreationData* pPixelShaderData, ShaderGenerator* pShaderGenerator,ID3D11Device* pDevice)
{
    m_pPixelShaderData  = pPixelShaderData;
    m_pShaderGenerator  = pShaderGenerator;
    m_pDevice           = pDevice;
}

CreatePixelShaderTask::~CreatePixelShaderTask()
{

}

void CreatePixelShaderTask::RunTask()
{
    ID3DBlob* pPSBlob = nullptr;
    HRESULT hr;

    //Compile pixelshader
    m_pShaderGenerator->CompileShaderFromFile(m_pPixelShaderData->pShaderFile,"PS", "ps_4_0", &pPSBlob);

    //Create pixel shader
    hr = m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize(), 
        NULL, &m_pPixelShaderData->pPixelShader);

    //Release the D3DBlob
    pPSBlob->Release();
}