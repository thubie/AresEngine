#pragma once
#include<d3d11.h>

struct TextureObject 
{
    unsigned int id;
    ID3D11ShaderResourceView* texture;
};