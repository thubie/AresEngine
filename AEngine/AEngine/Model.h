#pragma once

#include<Windows.h>
#include<xnamath.h>
#include<d3d11.h>
#include<d3dx11.h>
#include<d3dcompiler.h>
#include"Camera.h"
#include<assert.h>
#include"VertexTypes.h"

class Model
{
    struct ConstantBuffer
    {
        XMMATRIX m_World;
        XMMATRIX m_View;
        XMMATRIX m_Projection;
    };

public:
    Model();
    Model(const Model& other); 
    ~Model();

    void Render(Camera* pCamera);
    void SetVertexAndIndexBuffer(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer);
    void CreateVertexIndexBuffer(unsigned int VertexCount,unsigned int IndexCount,PosNormUV* vertices,unsigned int* indices);
    void GenerateShaderAndLayout();
    void CleanUpModelData();
    void SetGraphicsDeviceAndContext(ID3D11DeviceContext* immediateDevice,ID3D11Device* d3dDevice);

private:
    HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut);

private:
    ID3D11VertexShader*         m_pVertexShader;
    ID3D11PixelShader*          m_pPixelShader;
    ID3D11InputLayout*          m_pVertexLayout;
    ID3D11DeviceContext*        m_pImmediateContext;
    ID3D11RasterizerState*      m_wireframe; //need to put this by rendering code
    ID3D11Device*               m_pD3dDevice;
    ID3D11Buffer*               m_pVertexBuffer;
    ID3D11Buffer*               m_pIndexBuffer;
    ID3D11Buffer*               m_pConstantBuffer;
    ConstantBuffer*             m_pTestConstantBuffer;
    ID3D11SamplerState*         m_pSamplerAF;
    ID3D11ShaderResourceView*   m_pTexture; 

    //TODO: Put this in an camera class get it from there....
    XMMATRIX*                   m_WorldMatrix;
    unsigned int                m_numVertices;
    unsigned int                m_numIndices;
};