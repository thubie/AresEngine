#pragma once

#include<d3d11.h>
#include<d3dx11.h>
#include<d3dcompiler.h>
#include<vector>
#include<assert.h>

#include"Tasks.h"
#include"VertexShaderObject.h"
#include"PixelShaderObject.h"
#include"AEngine.h"

class AEngine;

class ShaderManager
{
public:
    ShaderManager(ID3D11Device* pD3DDevice,ID3D11DeviceContext* pImmediateContext, AEngine* p_Engine, const char* currentDir);
    ~ShaderManager();

    void SetVertexShader(unsigned int vsId);
    void SetPixelShader(unsigned int psId);

    Task* CreateVertexShaderTask();
    Task* CreatePixelShaderTask();

private:
    static void StartCreateVertexShader(TaskData* pData, void* thisPointer);
    static void StartCreatePixelShader(TaskData* pData, void* thisPointer);

    void CreateVertexShader(TaskData* data);
    void CreatePixelShader(TaskData* data); 

    static void DoneCreatingVertexShader(void* thispointer, void* task);
    static void DoneCreatingPixelShader(void* thispointer, void* task);

    void CleanUpResources();
    void CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,ID3DBlob** ppBlobOut);

private:
    ID3D11Device* m_pD3DDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    ID3D11InputLayout* m_pVertexLayout;
    std::vector<VertexShaderObject> m_VertexShaders;
    std::vector<PixelShaderObject> m_PixelShaders;
    AEngine* m_pEngine;
    char* m_pShadersPath;

//public:
//    bool finishedVertexShader;
//    bool finishedPixelShader;
};