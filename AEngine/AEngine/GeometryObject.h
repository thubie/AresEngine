#pragma once
#include<d3d11.h>

struct GeometryObject
{
    unsigned int id;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    unsigned int indicesCount; 
};