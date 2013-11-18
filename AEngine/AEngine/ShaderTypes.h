#pragma once 

enum ShaderType
{
    POSITIONCOLOR,
    POSITIONTEXTURE,
};

//Data bundles for the creation types.
struct VertexShaderCreationData
{
    ID3D11VertexShader* pVertexShader; 
    ID3D11InputLayout* pInputLayout;
    D3D11_INPUT_ELEMENT_DESC* pElementDescription;
    unsigned int numElements;
    WCHAR* pShaderFile;
};

struct PixelShaderCreationData
{
    ID3D11PixelShader* pPixelShader; 
    WCHAR* pShaderFile;
};