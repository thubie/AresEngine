#include"GeometryManager.h"

GeometryManager::GeometryManager()
{
    m_pGeoFactory = nullptr;
    m_pDevice = nullptr;
    m_pImmediateContext = nullptr;
    m_pAEngine = nullptr;
    
}

GeometryManager::~GeometryManager()
{
    GeometryObject* m_pMeshCollection;
}

void GeometryManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediatieContext,AEngine* pAEngine)
{
        m_pDevice = pDevice;
        m_pImmediateContext = pImmediatieContext;
        m_pAEngine = pAEngine;
        m_pGeoFactory = new GeometryFactory;
        assert(!(m_pGeoFactory == nullptr));
        m_pGeoFactory->SetGraphicsDeviceAndContext(m_pDevice, m_pImmediateContext);
        m_pMeshCollection = new std::vector<GeometryObject>;      
}

bool GeometryManager::DoneImporting()
{
    bool done = m_pGeoFactory->m_DoneImporting;
    return done;
}

void GeometryManager::Shutdown()
{
    if(m_pMeshCollection != nullptr)
    {
        for(int i = 0; i < m_pMeshCollection->size(); ++i)
        {
            GeometryObject temp = m_pMeshCollection->at(i);
            temp.vertexBuffer->Release();
            temp.indexBuffer->Release();
        }

    }

    if(m_pGeoFactory != nullptr)
        delete m_pGeoFactory;
    m_pGeoFactory = nullptr;

    //Not the owner of these objects so set their pointers as nullptr.
    m_pDevice = nullptr;
    m_pImmediateContext = nullptr;
    m_pAEngine = nullptr;
}

void GeometryManager::SetSubmeshIndexed(unsigned int subMeshIndex, OUT unsigned int* indicesCount)
{
    UINT stride = sizeof(PosNormUV);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers( 0, 1, &m_pMeshCollection->at(subMeshIndex).vertexBuffer, &stride, &offset );
    m_pImmediateContext->IASetIndexBuffer(m_pMeshCollection->at(subMeshIndex).indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    *indicesCount = m_pMeshCollection->at(subMeshIndex).indicesCount;
}


Task* GeometryManager::ImportAssetTask(const char* pFile)
{
    return m_pGeoFactory->ImportAsset(pFile, m_pMeshCollection, &m_Count);
}