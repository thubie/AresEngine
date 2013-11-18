#pragma once

#include"AEngine.h"
#include"GeometryFactory.h"
#include"ShaderGenerator.h"

class GraphicsSystem
{
public:
    GraphicsSystem(AEngine* pAEngine);

private:
    AEngine*            m_pAEngine;
    ShaderGenerator*    m_pShaderGenerator;

};