#include"GeometryManager.h"

GeometryManager::GeometryManager(AEngine* pEngine, const char* currentDir)
{
    m_pGeoFactory = nullptr;
    m_pDevice = nullptr;
    m_pImmediateContext = nullptr;
    m_pContentPath = new char[1024];
    strcpy_s(m_pContentPath, 1024, currentDir);
    strcat_s(m_pContentPath, 1024, "\\Content\\dude.dae");
    m_pEngine = pEngine;
}

GeometryManager::~GeometryManager()
{
    
}

void GeometryManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediatieContext)
{
        m_pDevice = pDevice;
        m_pDevice->AddRef();

        m_pImmediateContext = pImmediatieContext;
        m_pImmediateContext->AddRef();

        m_pGeoFactory = new GeometryFactory(this);
        assert(!(m_pGeoFactory == nullptr));
        m_pGeoFactory->SetGraphicsDeviceAndContext(m_pDevice, m_pImmediateContext);   
}


void GeometryManager::SubmitMessage()
{
    Message doneImporting;
    doneImporting.MessageType = IMPORT_GEOMETRY_DONE;
    m_pEngine->SubmitMessage(doneImporting);

}


void GeometryManager::Shutdown()
{

    for(unsigned int i = 0; i < m_MeshCollection.size(); ++i)
    {
        GeometryObject temp = m_MeshCollection.at(i);
        temp.vertexBuffer->Release();
        temp.indexBuffer->Release();
    }

    if(m_pGeoFactory != nullptr)
    {
        delete m_pGeoFactory;
        m_pGeoFactory = nullptr;
    }

    
    if(m_pDevice != nullptr)
    {
        m_pDevice->Release();
        m_pDevice = nullptr;
    }

    if(m_pImmediateContext != nullptr)
    {
        m_pImmediateContext->Release();
        m_pImmediateContext = nullptr;
    }

}

void GeometryManager::SetSubmeshIndexed(unsigned int subMeshIndex, OUT unsigned int* indicesCount)
{
    UINT stride = sizeof(PosNormalTexSkinned);
    UINT offset = 0;
    GeometryObject geometryObj = m_MeshCollection.at(subMeshIndex);
    m_pImmediateContext->IASetVertexBuffers( 0, 1, &geometryObj.vertexBuffer, &stride, &offset );
    m_pImmediateContext->IASetIndexBuffer(geometryObj.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    *indicesCount = geometryObj.indicesCount;
}

Task* GeometryManager::ImportAssetTask()
{
    return m_pGeoFactory->ImportAsset(m_pContentPath, m_MeshCollection, &m_Count);
}