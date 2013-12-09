#pragma once 

#include<d3d11.h>
#include<D3DX11.h>

#include"GeometryObject.h"
#include"GeometryFactory.h"
#include"AEngine.h"

class AEngine;
class GeometryFactory;

class GeometryManager
{
public:
    GeometryManager();
    ~GeometryManager();

    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediatieContext, AEngine* pAEngine);
    void Shutdown();

    void GetSubmeshIndexed(unsigned int subMeshId, ID3D11Buffer* vertexBuffer,ID3D11Buffer* indexBuffer, unsigned int* indicesCount);
    Task* ImportAssetTask(const char* pFile);

    bool DoneImporting();

private:
    GeometryFactory* m_pGeoFactory;
    
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    AEngine* m_pAEngine;

public:
    unsigned int m_count;
    GeometryObject* m_pMeshCollection;
    unsigned int* m_pIdCollection;

};