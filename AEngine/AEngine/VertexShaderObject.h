#pragma once
#include<d3d11.h>

struct VertexShaderObject
{
    unsigned int id;
    ID3D11VertexShader* vertexShader;
    ID3D11InputLayout* inputLayout;
};