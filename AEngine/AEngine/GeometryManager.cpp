#include"GeometryManager.h"

GeometryManager::GeometryManager()
{
    m_pGeoFactory = nullptr;
    m_pDevice = nullptr;
    m_pImmediateContext = nullptr;
    m_pAEngine = nullptr;
    m_pMeshCollection = nullptr;
}

GeometryManager::~GeometryManager()
{
    GeometryObject* m_pMeshCollection;
}

void GeometryManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediatieContext,AEngine* pAEngine)
{
    if(m_pDevice == nullptr)
        m_pDevice = pDevice;

    if(m_pImmediateContext = nullptr)
        m_pImmediateContext = pImmediatieContext;

    if(m_pAEngine == nullptr)
        m_pAEngine = pAEngine;

    if(m_pGeoFactory == nullptr)
        m_pGeoFactory = new GeometryFactory;
    m_pGeoFactory->SetGraphicsDeviceAndContext(m_pDevice, m_pImmediateContext);

    if(m_pMeshCollection == nullptr)
        m_pMeshCollection = new GeometryObject[16]();
}

bool GeometryManager::DoneImporting()
{
    bool done = m_pGeoFactory->m_DoneImporting;
    return done;
}

void GeometryManager::Shutdown()
{
    if(m_pMeshCollection != nullptr)
        delete[] m_pMeshCollection;
    m_pMeshCollection = nullptr;

    if(m_pGeoFactory != nullptr)
        delete m_pGeoFactory;
    m_pGeoFactory = nullptr;

    //Not the owner of these objects so set their pointers as nullptr.
    m_pDevice = nullptr;
    m_pImmediateContext = nullptr;
    m_pAEngine = nullptr;
}

void GeometryManager::GetSubmeshIndexed(unsigned int subMeshIndex, ID3D11Buffer* vertexBuffer,ID3D11Buffer* indexBuffer, unsigned int* indicesCount)
{
    GeometryObject temp = m_pMeshCollection[subMeshIndex];
    vertexBuffer = temp.vertexBuffer;
    indexBuffer = temp.indexBuffer;
    *indicesCount = temp.indicesCount;
}

Task* GeometryManager::ImportAssetTask(const char* pFile)
{
    return m_pGeoFactory->ImportAsset(pFile, m_pMeshCollection, &m_count);
}