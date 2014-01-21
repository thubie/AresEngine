#include"GeometryFactory.h"


GeometryFactory::GeometryFactory(GeometryManager* pGeoManager)
{
    m_pGeoManager = pGeoManager;
    m_DoneImporting = false;
}

GeometryFactory::~GeometryFactory()
{

}

void GeometryFactory::SetGraphicsDeviceAndContext(ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext)
{
    m_pImmediateContext = immediateContext;
    m_pImmediateContext->AddRef();
    m_pD3dDevice = d3dDevice;
    m_pD3dDevice->AddRef();
}

//Import the asset.
void GeometryFactory::DoImportAsset(TaskData* pdata)
{
    const char* pFile = (char*)pdata->parameter1;
    std::vector<GeometryObject>* SubMeshCollection = (std::vector<GeometryObject>*)pdata->parameter2;
    Assimp::Importer localImporter;
    
    //Import and parse the file 
    const aiScene* pScene = localImporter.ReadFile(pFile,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_FixInfacingNormals |
        aiProcess_GenUVCoords |
        aiProcess_SplitByBoneCount );

    PosNormalTexSkinned* vertices;
    unsigned int* indices;
    aiMesh* mesh;
    unsigned int numVertices;
    unsigned int numFaces;
    unsigned int numIndices;

    //int boneIndex = 0;
    std::vector<std::string>* BonesByName = new std::vector<std::string>();
    BoneData* boneDataCollection;
    BonesByName->reserve(100);

    aiNode* rootNode = pScene->mRootNode->mChildren[0];
    ExtractSkeletonData(BonesByName, rootNode);

    for(int i = 0; i < pScene->mNumMeshes; ++i)
    {
        mesh = pScene->mMeshes[i];
        GeometryObject geoMesh;
        numVertices = mesh->mNumVertices;
        numFaces = mesh->mNumFaces;
        numIndices = numFaces * 3;
        vertices = new PosNormalTexSkinned[numVertices];
        indices = new unsigned int[numIndices];
        boneDataCollection = new BoneData[numVertices];

        ZeroMemory(vertices,sizeof(PosNormalTexSkinned) * numVertices);
        ZeroMemory(indices,sizeof(unsigned int) * numVertices);
        ZeroMemory(boneDataCollection,sizeof(BoneData) * numVertices);

        GenerateVertices(vertices, numVertices, mesh);
        GenerateIndices(indices, numFaces, mesh);
        GenerateBonesAndWeight(BonesByName, vertices, mesh);
        
        HRESULT hr;
        D3D11_BUFFER_DESC bd;
        ZeroMemory( &bd, sizeof(bd) );
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(PosNormalTexSkinned) * numVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory( &InitData, sizeof(InitData));
        InitData.pSysMem = vertices;
        hr = m_pD3dDevice->CreateBuffer( &bd, &InitData, &geoMesh.vertexBuffer );
        assert(!FAILED(hr)); 

        ZeroMemory( &bd, sizeof(bd) );
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(unsigned int) * numIndices;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = indices;
        hr = m_pD3dDevice->CreateBuffer(&bd, &InitData, &geoMesh.indexBuffer);   
        assert(!FAILED(hr));
        
        geoMesh.id = i;
        geoMesh.indicesCount = numIndices;
        SubMeshCollection->push_back(geoMesh);

        
        delete[] boneDataCollection;
        delete[] vertices;
        delete[] indices;
        
    }
    BonesByName->clear();
}

//Extract Skeleton data 
void GeometryFactory::ExtractSkeletonData(std::vector<std::string>* skeletonData, aiNode* node)
{
    std::string boneName(node->mName.C_Str()); 
    skeletonData->push_back(boneName);
    for(int i = 0; i < node->mNumChildren; ++i)
    {
        ExtractSkeletonData(skeletonData, node->mChildren[i]);
    }
}

//find the bone index 
unsigned int GeometryFactory::FindBoneIndex(std::string* boneName, std::vector<std::string>* BonesByName)
{
    unsigned int numBones = BonesByName->size();
    std::string boneNameToFind = *boneName;
    std::string curBoneName;
    for(int i = 0; i < numBones; ++i)
    {
        curBoneName = BonesByName->at(i);
        if(curBoneName == boneNameToFind)
        {
            return i;
        }
    }
    return 0;
}

//Generate the vertices.
void GeometryFactory::GenerateVertices(PosNormalTexSkinned* vertices,unsigned int numVertices, aiMesh* mesh)
{
    //Get the position, normal and texcoord information
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
}

//Generate the indices.
void GeometryFactory::GenerateIndices(unsigned int* indices, unsigned int numFaces, aiMesh* mesh)
{
    unsigned int index = 0;
    for(int k = 0; k < numFaces; k++)
    {
        aiFace face = mesh->mFaces[k];
        for(int l = 0; l < 3; l++)
        {
            indices[l + index] = face.mIndices[l];
        }
        index += 3;
    }
}

//Generate the Bones and Weights
void GeometryFactory::GenerateBonesAndWeight(std::vector<std::string>* BonesByName, PosNormalTexSkinned* vertices, aiMesh* mesh)
{
    /*Set bones and weights*/
    std::string boneName;
        
    unsigned int numBones = mesh->mNumBones;
    unsigned int numWeight = 0;
    aiBone* bone;
    int index = 0;
    aiVertexWeight weight;
        
    for(int j = 0; j < numBones; ++j)
    {
        bone = mesh->mBones[j];
        boneName = bone->mName.C_Str();
        numWeight = bone->mNumWeights;
        index = FindBoneIndex(&boneName,BonesByName);

        for(int k = 0; k < numWeight; ++k)
        {
            weight =  bone->mWeights[k];
            AddBoneData(vertices, index, weight);
        }
    }
}

//Add Bone data to vertex
void GeometryFactory::AddBoneData(PosNormalTexSkinned* vertices, unsigned int boneId, aiVertexWeight weight)
{
    PosNormalTexSkinned temp = vertices[weight.mVertexId];
    for(int i = 0; i < 4; ++i)
    {
        if(temp.boneIndices[i] == 0)
        {
            temp.boneIndices[i] = boneId;
            temp.weights[i] = weight.mWeight;
            vertices[weight.mVertexId] = temp;
            return;
        }
    }
}

//Get the importAssetTask
Task* GeometryFactory::ImportAsset(const char* pFile, std::vector<GeometryObject>& pMeshCollection, unsigned int* pMeshCount)
{
    Task* task = new Task;
    TaskData* data = new TaskData;
    data->parameter1 = (void*)pFile;
    data->parameter2 = (void*)&pMeshCollection;
    data->parameter3 = (void*)pMeshCount;
    data->parameter4 = nullptr;
    data->parameter5 = nullptr;

    task->thisPointer = this;
    task->pTaskData = data;
    task->callback = DoneImporting;
    task->taskFunction = ImportAssetTask;
    return task; 
}

void GeometryFactory::ImportAssetTask(TaskData* pData, void* thisPointer)
{
    GeometryFactory* self = static_cast<GeometryFactory*>(thisPointer);
    self->DoImportAsset(pData);
}

void GeometryFactory::DoneImporting(void* thispointer, void* task)
{
    GeometryFactory* self = static_cast<GeometryFactory*>(thispointer);
    self->m_pGeoManager->SubmitMessage();
    self->m_DoneImporting = true;
    
}

//Callback function 
void GeometryFactory::DoneImportingAsset(void* task)
{
    //need to implement an 
    //better option like an event or message 
    //instead of an bool flag
    
}

