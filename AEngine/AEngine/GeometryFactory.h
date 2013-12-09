#pragma once


#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<d3d11.h>
#include<d3dx11.h>

#include"Model.h"
#include"Tasks.h"
#include"VertexTypes.h"
#include"GeometryManager.h"
#include"GeometryObject.h"


class GeometryManager;

class GeometryFactory
{
public:
    GeometryFactory();
    ~GeometryFactory();
    void SetGraphicsDeviceAndContext(ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext); 
    Task* ImportAsset(const char* pFile, GeometryObject* pMeshCollection, unsigned int* meshCount);
    
    static void ImportAssetTask(TaskData* pData, void* thisPointer)
    {
        GeometryFactory* self = static_cast<GeometryFactory*>(thisPointer);
        self->DoImportAsset(pData);
    }

    static void DoneImporting(void* thispointer, void* task)
    {
        GeometryFactory* self = static_cast<GeometryFactory*>(thispointer);
        self->DoneImportingAsset(task);
    }

private:
    void DoImportAsset(TaskData* pData); //import asset task
    void DoneImportingAsset(void* task); //Callback function when done importing
    
    void CreateVertexBuffer(ID3D11Buffer* vertexBuffer, PosNormUV* vertices, unsigned int count);
    void CreateIndexBuffer(ID3D11Buffer* indexBuffer, unsigned int* indices, unsigned int count);

private:
    ID3D11DeviceContext* m_pImmediateContext;
    ID3D11Device* m_pD3dDevice;
    unsigned int m_taskidCounter;
    GeometryManager* m_pGeoManager; 
public:
    bool m_DoneImporting;
};