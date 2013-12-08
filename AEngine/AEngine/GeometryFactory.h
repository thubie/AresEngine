#pragma once


#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include"Model.h"
#include"Tasks.h"
#include"VertexTypes.h"

class GeometryFactory
{
public:
    GeometryFactory();
    GeometryFactory(const GeometryFactory& other);
    ~GeometryFactory();

    void Initialize();
    void Shutdown();
    void SetGraphicsDeviceAndContext(ID3D11DeviceContext* immediateDevice,ID3D11Device* d3dDevice); //need to change this.
    Model* ImportAssetTest(const char* pFile);

    static void ImportAssetTask(TaskData* pData, void* thisPointer)
    {
        GeometryFactory* self = static_cast<GeometryFactory*>(thisPointer);
        self->ImportAsset(pData);
    }

private:
    void ImportAsset(TaskData* pData);

private:
    bool* m_usedImporters;
    ID3D11DeviceContext*    m_pImmediateContext;
    ID3D11Device*           m_pD3dDevice;

    //Testing code.
    PosNormUV** m_pMeshcollection;
    unsigned int m_meshCount;
};