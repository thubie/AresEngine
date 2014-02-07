#pragma once 

#include<d3d11.h>
#include<D3DX11.h>
#include<vector>
#include"GeometryObject.h"
#include"GeometryFactory.h"
#include"RingBufferQueue.h"
#include"RenderSystem.h"

class RenderSystem;
class GeometryFactory;

class GeometryManager
{
public:
    GeometryManager(RenderSystem* pRenderSystem, const char* currentDir);
    ~GeometryManager();
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediatieContext);
    void Shutdown();
    void SetSubmeshIndexed(unsigned int subMeshId, OUT unsigned int* indicesCount);
    Task* ImportAssetTask();
    void SubmitMessage();

private:
    GeometryFactory* m_pGeoFactory;
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    std::vector<GeometryObject> m_MeshCollection;
    RenderSystem* m_pRenderSystem;
    char* m_pContentPath;
public:
    unsigned int m_Count;      
};