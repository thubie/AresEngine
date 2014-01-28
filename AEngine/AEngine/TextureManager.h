#pragma once

#include<d3d11.h>
#include<d3dx11.h>
#include<vector>
#include<assert.h>

#include"RenderSystem.h"
#include"TextureObject.h"
#include"Tasks.h"

class RenderSystem;

class TextureManager
{
public:
    TextureManager(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, RenderSystem* pRenderSystem, const char* currentDir);
    ~TextureManager();

    void SetTexture(unsigned int ID);
    Task* ImportTextures();

private:
    void CleanUpResources();
    void DoImportTask(TaskData* data);
    void DoneImportingTask(void* task);

    static void DoImportTextureTask(TaskData* pData, void* thisPointer);
    static void DoneImporting(void* thispointer, void* task);

private:
    std::vector<TextureObject> m_TextureCollection;
    ID3D11SamplerState* m_pSamplerAF;
    ID3D11Device* m_pD3DDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    RenderSystem* m_pRenderSystem;
    char* m_ContentPath;

};