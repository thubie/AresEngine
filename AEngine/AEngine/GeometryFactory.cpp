#include"GeometryFactory.h"


GeometryFactory::GeometryFactory()
{
    m_taskidCounter = 0;
    m_DoneImporting = false;
}

GeometryFactory::~GeometryFactory()
{

}

//Import the asset.
void GeometryFactory::DoImportAsset(TaskData* pdata)
{
    const char* pFile = (char*)pdata->parameter1;
    GeometryObject* MeshCollection = (GeometryObject*)pdata->parameter2;
    unsigned int* MeshCount = (unsigned int*) pdata->parameter3;

    Assimp::Importer localImporter;
    
    //Import and parse the file 
    const aiScene* pScene = localImporter.ReadFile(pFile,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_FixInfacingNormals |
        aiProcess_GenUVCoords);

    PosNormUV* vertices;
    unsigned int* indices;
    aiMesh* mesh;
    unsigned int numVertices;
    unsigned int numFaces;
    unsigned int numIndices;

    for(int i = 0; i < pScene->mNumMeshes; ++i)
    {
        mesh = pScene->mMeshes[i];
        GeometryObject Mesh = MeshCollection[i];
        numVertices = mesh->mNumVertices;
        numFaces = mesh->mNumFaces;
        numIndices = numFaces * 3;
        vertices = new PosNormUV[numVertices];
        indices = new unsigned int[numIndices];
        
        for(int j = 0; j < numVertices; j++)
        {
            vertices[j].pos.x = mesh->mVertices[j].x;
            vertices[j].pos.y = mesh->mVertices[j].y;
            vertices[j].pos.z = mesh->mVertices[j].z;

            vertices[j].norm.x = mesh->mNormals[j].x;
            vertices[j].norm.y = mesh->mNormals[j].y;
            vertices[j].norm.z = mesh->mNormals[j].z;

            vertices[j].uv.x = mesh->mTextureCoords[0][j].x;
            vertices[j].uv.y = mesh->mTextureCoords[0][j].y;
        } 

        int index = 0;
        for(int k = 0; k < numFaces; k++)
        {
            aiFace face = mesh->mFaces[k];
            for(int l = 0; l < 3; l++)
            {
                indices[l + index] = face.mIndices[l];
            }
            index += 3;
        }
        
        //Test code
        HRESULT hr;
        D3D11_BUFFER_DESC bd;
        ZeroMemory( &bd, sizeof(bd) );
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(PosNormUV) * numVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory( &InitData, sizeof(InitData));
        InitData.pSysMem = vertices;
        hr = m_pD3dDevice->CreateBuffer( &bd, &InitData, &Mesh.vertexBuffer );
        assert(!FAILED(hr)); 

        //HRESULT hr;
        //D3D11_BUFFER_DESC bd;
        ZeroMemory( &bd, sizeof(bd) );
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(unsigned int) * numIndices;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        //D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = indices;
        hr = m_pD3dDevice->CreateBuffer(&bd, &InitData, &Mesh.indexBuffer);   
        assert(!FAILED(hr));

        //CreateVertexBuffer(Mesh.vertexBuffer, vertices, numVertices);
        //CreateIndexBuffer(Mesh.indexBuffer, indices, numFaces * 3);
        Mesh.indicesCount = numIndices;
        
        MeshCollection[i] = Mesh;

        delete[] vertices;
        delete[] indices;
    }

}

void GeometryFactory::SetGraphicsDeviceAndContext(ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext)
{
    m_pImmediateContext = immediateContext;
    m_pD3dDevice = d3dDevice;
}

//Get the importAssetTask
Task* GeometryFactory::ImportAsset(const char* pFile, GeometryObject* pMeshCollection, unsigned int* pMeshCount)
{
    Task* task = new Task;
    TaskData* data = new TaskData;
    data->parameter1 = (void*)pFile;
    data->parameter2 = (void*)pMeshCollection;
    data->parameter3 = (void*)pMeshCount;
    data->parameter4 = nullptr;
    data->parameter5 = nullptr;

    task->thisPointer = this;
    task->pTaskData = data;
    task->callback = DoneImporting;
    task->taskFunction = ImportAssetTask;
    return task; 
}

//Callback function need to implement an 
//better option like an event or message 
//instead of an bool flag
void GeometryFactory::DoneImportingAsset(void* task)
{
    bool finished = true;
    m_DoneImporting = finished;
}

//Create the vertexBuffer
void GeometryFactory::CreateVertexBuffer(ID3D11Buffer* vertexBuffer, PosNormUV* vertices, unsigned int count)
{
    HRESULT hr;
    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(PosNormUV) * count;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr = m_pD3dDevice->CreateBuffer( &bd, &InitData, &vertexBuffer );
    assert(!FAILED(hr)); 
}

//Create the indexBuffer
void GeometryFactory::CreateIndexBuffer(ID3D11Buffer* indexBuffer, unsigned int* indices, unsigned int count)
{
    HRESULT hr;
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(unsigned int) * count;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = indices;
    hr = m_pD3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer);   
    assert(!FAILED(hr));
}