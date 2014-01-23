#pragma once

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<d3d11.h>
#include<d3dx11.h>
#include<vector>
#include<queue>
#include<map>
#include<string>

#include"Model.h"
#include"Tasks.h"
#include"VertexTypes.h"
#include"GeometryManager.h"
#include"GeometryObject.h"


class GeometryManager;

class GeometryFactory
{
    struct BoneData
    {
        unsigned int index;
        float weights[4];
        unsigned int boneId[4];
    };

public:
    GeometryFactory(GeometryManager* pGeoManager);
    ~GeometryFactory();
    void SetGraphicsDeviceAndContext(ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext); 
    Task* ImportAsset(const char* pFile, std::vector<GeometryObject>& pMeshCollection, unsigned int* meshCount);
    
private:
    void DoImportAsset(TaskData* pData); //import asset task
    static void ImportAssetTask(TaskData* pData, void* thisPointer);
    static void DoneImporting(void* thispointer, void* task);
    
    void GenerateVertices(PosNormalTexSkinned* vertices,unsigned int numVertices,aiMesh* mesh);
    void GenerateIndices(unsigned int* indices, unsigned int numIndices,  aiMesh* mesh);
    void GenerateBonesAndWeight(std::vector<std::string>* BonesByName, PosNormalTexSkinned* vertices, aiMesh* mesh);

    void ExtractSkeletonData(std::vector<std::string>* Bones, aiNode* Node);
    unsigned int FindBoneIndex(std::string* boneName, std::vector<std::string>* BonesByName);
    void AddBoneData(PosNormalTexSkinned*, unsigned int boneId, aiVertexWeight weight);

private:
    ID3D11DeviceContext* m_pImmediateContext;
    ID3D11Device* m_pD3dDevice;
    GeometryManager* m_pGeoManager; 
};