#include"GeometryFactory.h"


GeometryFactory::GeometryFactory()
{
  
}

GeometryFactory::~GeometryFactory()
{}

void GeometryFactory::Initialize()
{
    
}

void GeometryFactory::Shutdown()
{

}

void GeometryFactory::ImportAsset(TaskData* pdata)
{

}

void GeometryFactory::SetGraphicsDeviceAndContext(ID3D11DeviceContext* immediateContext,ID3D11Device* d3dDevice)
{
    m_pImmediateContext = immediateContext;
    m_pD3dDevice = d3dDevice;
}

Model* GeometryFactory::ImportAssetTest(const char* pFile)
{   
    Model* tempModel = new Model();
    tempModel->SetGraphicsDeviceAndContext(m_pImmediateContext,m_pD3dDevice);
    tempModel->GenerateShaderAndLayout();

    Assimp::Importer localImporter;
    
    //Import and parse the file 
    const aiScene* pScene = localImporter.ReadFile(pFile,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_FixInfacingNormals |
        aiProcess_GenUVCoords 
        //aiProcess_FlipUVs
        );

    //Get the only mesh
    aiMesh* mesh = pScene->mMeshes[0]; 
    unsigned int numVertices = mesh->mNumVertices;
    unsigned int numFaces  = mesh->mNumFaces;
    
    //XMFLOAT3* vertices = new XMFLOAT3[numVertices];
    PosNormUV* vertices = new PosNormUV[numVertices];
    unsigned int* indices = new unsigned int[numFaces * 3];
    float u = 0; 
    float v = 0;


    for(int i = 0; i < numVertices; i++)
    {
        vertices[i].pos.x = mesh->mVertices[i].x;
        vertices[i].pos.y = mesh->mVertices[i].y;
        vertices[i].pos.z = mesh->mVertices[i].z;

        vertices[i].norm.x = mesh->mNormals[i].x;
        vertices[i].norm.y = mesh->mNormals[i].y;
        vertices[i].norm.z = mesh->mNormals[i].z;

        vertices[i].uv.x = mesh->mTextureCoords[0][i].x;
        vertices[i].uv.y = mesh->mTextureCoords[0][i].y;
    }

    int index = 0;
    for(int i = 0; i < numFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for(int j = 0; j < 3; j++)
        {
            indices[j + index] = face.mIndices[j];
        }
        index += 3;
    }
    tempModel->CreateVertexIndexBuffer(numVertices, numFaces * 3, vertices, indices);

    delete[] vertices;
    delete[] indices;
    return tempModel;
}