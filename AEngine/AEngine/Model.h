#pragma once

#include<Windows.h>
#include<xnamath.h>
#include<d3d11.h>
#include<D3DX11.h>
#include <d3dcompiler.h>


class Model
{
    struct SimpleVertex
    {
        XMFLOAT3 m_Pos;
        XMFLOAT4 Color;
    };

    struct ConstantBuffer
    {
        XMMATRIX m_World;
        XMMATRIX m_View;
        XMMATRIX m_Projection;
    };

public:
    Model(ID3D11DeviceContext* immediateDevice,ID3D11Device* d3dDevice);
    Model(const Model& other); 
    ~Model();

    void render();
    void InitModel();
    void CleanUpModel();
private:
    HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, 
        LPCSTR szShaderModel,ID3DBlob** ppBlobOut);

private:
    ID3D11VertexShader*     m_pVertexShader;
    ID3D11PixelShader*      m_pPixelShader;
    ID3D11InputLayout*      m_pVertexLayout;
    ID3D11DeviceContext*    m_pImmediateContext;
    ID3D11Device*           m_pD3dDevice;
    ID3D11Buffer*           m_pVertexBuffer;
    ID3D11Buffer*           m_pIndexBuffer;
    ID3D11Buffer*           m_pConstantBuffer;
    //TODO: Put this in an camera class get it from there....
    XMMATRIX                m_WorldMatrix;
    XMMATRIX                m_ViewMatrix;
    XMMATRIX                m_ProjectionMatrix;
    //Second cube.
    XMMATRIX                m_WorldMatrix2;
    XMMATRIX                m_ViewMatrix2;
    XMMATRIX                m_ProjectionMatrix2;
};