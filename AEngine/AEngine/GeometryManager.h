#pragma once 

#include<d3d11.h>
#include<D3DX11.h>
#include<vector>
#include"GeometryObject.h"
#include"GeometryFactory.h"
#include"RingBufferQueue.h"
#include"AEngine.h"

class AEngine;
class GeometryFactory;

class GeometryManager
{
public:
    GeometryManager(AEngine* pEngine);
    ~GeometryManager();
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediatieContext);
    void Shutdown();
    void SetSubmeshIndexed(unsigned int subMeshId, OUT unsigned int* indicesCount);
    Task* ImportAssetTask(const char* pFile);
    bool DoneImporting();
    void SubmitMessage();

private:
    GeometryFactory* m_pGeoFactory;
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    std::vector<GeometryObject> m_MeshCollection;
    AEngine* m_pEngine;

public:
    unsigned int m_Count;
        
};