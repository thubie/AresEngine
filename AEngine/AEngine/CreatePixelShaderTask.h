#pragma once

#include<assert.h>
#include"VertexFormat.h"
#include"ShaderGenerator.h"
#include"ITask.h"

class CreatePixelShaderTask : ITask
{
public:
    CreatePixelShaderTask(PixelShaderCreationData* pPixelShaderData, ShaderGenerator* pShaderGenerator,ID3D11Device* pDevice);
    ~CreatePixelShaderTask();
    void RunTask();

private:
    PixelShaderCreationData*    m_pPixelShaderData;
    ShaderGenerator*            m_pShaderGenerator;
    ID3D11Device*               m_pDevice;
};
