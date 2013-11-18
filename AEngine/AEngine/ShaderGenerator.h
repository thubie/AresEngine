#pragma once

#include<d3d11.h>
#include<D3DX11.h>
#include<d3dcompiler.h>
#include"GraphicsSystem.h"
#include"ShaderTypes.h"


//ShaderType;

class ShaderGenerator
{
public:
    ShaderGenerator(GraphicsSystem* pGraphicsSystem);
    ShaderGenerator(const ShaderGenerator& other);
    ~ShaderGenerator();

    void CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut);
    

private: 
    void PosColorShader(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);
    
private:
    GraphicsSystem*   m_pGraphicsSystem;

    //Need to encapsulate this later but use this as test.
    ID3D11VertexShader*         m_pTestVertex;
    ID3D11PixelShader*          m_pTestPixel;
    D3D11_INPUT_ELEMENT_DESC*   m_pTestLayout;

    VertexShaderCreationData*   m_pTestVertexCreateData;
    PixelShaderCreationData*    m_pTestPixelCreateData;
};