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
    Task* ImportAsset(const char* pFile, std::vector<GeometryObject>* pMeshCollection, unsigned int* meshCount);
    
private:
    void DoImportAsset(TaskData* pData); //import asset task
    void DoneImportingAsset(void* task); //Callback function when done importing

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
    ID3D11DeviceContext* m_pImmediateContext;
    ID3D11Device* m_pD3dDevice;
    unsigned int m_taskidCounter;
    GeometryManager* m_pGeoManager; 
public:
    bool m_DoneImporting;
};