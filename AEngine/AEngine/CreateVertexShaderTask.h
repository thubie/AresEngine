#pragma once

#include<assert.h>
#include"VertexFormat.h"
#include"ShaderGenerator.h"
#include"ITask.h"

class CreateVertexShaderTask : ITask
{
public:
    CreateVertexShaderTask(VertexShaderCreationData* pVertexShaderData, ShaderGenerator* pShaderGenerator,ID3D11Device* pDevice);
    ~CreateVertexShaderTask();
    void RunTask();

private:
    ShaderGenerator*            m_pShaderGenerator;
    VertexShaderCreationData*   m_pCreationData;
    ID3D11Device*               m_pDevice
};