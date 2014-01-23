#include"AnimationManager.h"

AnimationManager::AnimationManager(AEngine* pEngine, const char* currentDir)
{
    m_pImporter = new Assimp::Importer;
    animationIndex = 0;
    m_pAnimationPath = new char[1024];
    strcpy_s(m_pAnimationPath, 1024, currentDir);
    strcat_s(m_pAnimationPath, 1024, "\\Content\\dude.dae");
    m_pEngine = pEngine;
}

AnimationManager::~AnimationManager()
{
   /* m_GameObjectIds.clear();    
    m_TimeScalers.clear();
    m_AnimationTimes.clear();   
    m_AnimationTaskData.clear();
    m_OffsetMatricesAssimp.clear();
    m_FinalTransforms.clear();*/
}

//Creates the animation tasks and returns the number of animation tasks created.
unsigned int AnimationManager::CreateAnimationTasks()
{
    unsigned int numCreatedTask = 0;
    unsigned int numTaskToCreate = m_GameObjectIds.size();
    unsigned int gameObjectID = 0;
    unsigned int startIndex = 0;
    unsigned int endIndex = 0;

    for(unsigned int i = 0; i < numTaskToCreate; ++i)
    { 
        startIndex = i * 8;
        endIndex = startIndex + 7;

        gameObjectID = m_GameObjectIds[i];
        m_AnimationTaskData[gameObjectID].parameter1 = (void*)i; //start Index
        m_AnimationTaskData[gameObjectID].parameter2 = nullptr; //End index
        m_AnimationTaskData[gameObjectID].parameter3 = nullptr; 
        m_AnimationTaskData[gameObjectID].parameter4 = nullptr; 
        m_AnimationTaskData[gameObjectID].parameter5 = nullptr; 

        m_AnimationTask[gameObjectID].pTaskData = &m_AnimationTaskData[gameObjectID];
        m_AnimationTask[gameObjectID].thisPointer = this;
        m_AnimationTask[gameObjectID].taskFunction = AnimationUpdate;
        m_AnimationTask[gameObjectID].callback = DoneUpdatingAnimation;
        m_AnimationTask[gameObjectID].taskId = 0;
        
        numCreatedTask++;
    }
    return numCreatedTask;
}

//Register game objects
void AnimationManager::RegisterGameObjects(std::vector<unsigned int>& gameIds)
{
    unsigned int numGameObjects = gameIds.size();
    unsigned int gameId = 0;
    TaskData taskData;
    Task task;

    for(unsigned int i = 0; i < numGameObjects; ++i)
    {
        gameId = gameIds[i];
        m_GameObjectIds.push_back(gameId);
        m_AnimationTimes.push_back(0.0f);
        m_TimeScalers.push_back(1.0f);

        SkeletonCBufferData CBuffer;
        m_FinalTransforms.push_back(CBuffer);

        m_AnimationTaskData.push_back(taskData);
        m_AnimationTask.push_back(task);
    }
}

//Callback function when done updating animation
void AnimationManager::DoneUpdatingAnimation(void* thisPointer, void* task)
{
    AnimationManager* self = static_cast<AnimationManager*>(thisPointer);
    LONG openTasks = _InterlockedDecrement(&self->m_OpenAnimationTasks);
    if(openTasks == 0)
    {
        Message doneAnimation;
        doneAnimation.MessageType = ANIMATIONS_DONE;
        self->m_pEngine->SubmitMessage(doneAnimation);
    }
}

//Starts the update animation state
void AnimationManager::AnimationUpdate(TaskData* pData, void* thisPointer)
{
    AnimationManager* self = static_cast<AnimationManager*>(thisPointer);
    self->DoAnimationUpdate(pData);
}

//Update all gameobject animation timer
void AnimationManager::UpdateAnimationTime(float elapsedTime)
{
    unsigned int numGameObjects = m_GameObjectIds.size();
    for(unsigned int i = 0; i < numGameObjects; ++i)
    {
        m_AnimationTimes.at(i) += elapsedTime * m_TimeScalers.at(i);
    }
    m_OpenAnimationTasks = numGameObjects;
}

//Update animation state
void AnimationManager::DoAnimationUpdate(TaskData* pData)
{
    //initializes to an id matrix
    aiMatrix4x4 idMatrix;
    unsigned int index = (unsigned int)pData->parameter1;
    unsigned int gameObjectId = m_GameObjectIds[index];
    float animTime;
    float timeInTicks;
    //Get the ticks per second if the ticks per second isn't zero.
    //Set ticks per second to the ticks per second of the animation 
    //else set it at 25 ticks per second
    float tickPerSecond = m_pScene->mAnimations[animationIndex]->mTicksPerSecond != 0 ?
        m_pScene->mAnimations[animationIndex]->mTicksPerSecond : 25.0f;

    animTime = m_AnimationTimes[gameObjectId];
    timeInTicks = animTime * tickPerSecond;
    animTime = fmod(timeInTicks, m_pScene->mAnimations[animationIndex]->mDuration);
    ProcessAnimation(animTime, m_pScene->mRootNode->mChildren[0], idMatrix, gameObjectId); 
}

//Find the node within the animation channel collection
aiNodeAnim* AnimationManager::FindNodeAnim(const aiAnimation* animation,const aiNode* node)
{
    unsigned int numChannels = animation->mNumChannels;
    aiNodeAnim* result = nullptr;
    std::string currentNodeName;

    for(unsigned int i = 0; i < numChannels; ++i)
    {
        if(animation->mChannels[i]->mNodeName == node->mName)
        {
            result = animation->mChannels[i];
        }
    }
    return result;
}

//Update animation information.
void AnimationManager::ProcessAnimation(float animTime, const aiNode* node, aiMatrix4x4& parentTransform, unsigned int gameobjectID)
{
    std::string NodeName(node->mName.data);
    unsigned int index = FindBoneIndex(node); 
    aiMatrix4x4 nodeTransformation = node->mTransformation;
    const aiAnimation* pAnimation = m_pScene->mAnimations[animationIndex];
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, node);

    if(pNodeAnim)
    {
        //Interpolate rotation and generate rotation transformation matrix
        aiQuaternion rotationAssimp;
        aiVector3D positionAssimp(0,0,0);
        aiVector3D scalingAssimp(0,0,0);

        InterpolateRotation(rotationAssimp,animTime,pNodeAnim);
        InterpolatePosition(positionAssimp,animTime,pNodeAnim);
        InterpolateScaling(scalingAssimp,animTime,pNodeAnim);

        aiMatrix4x4 nodeTransAssimp;
        aiMatrix4x4 translation;
        aiMatrix4x4 scaling;
        aiMatrix4x4 rotation;

        nodeTransAssimp = aiMatrix4x4(rotationAssimp.GetMatrix());
        rotation = aiMatrix4x4(rotationAssimp.GetMatrix());
        aiMatrix4x4::Translation(positionAssimp,translation);
        aiMatrix4x4::Scaling(scalingAssimp,scaling);
        
        nodeTransformation = translation *  rotation * scaling;
    }

    aiMatrix4x4 globalTransformation =  parentTransform * nodeTransformation;
    aiMatrix4x4 offsetMatrix = m_OffsetMatricesAssimp[index];
    aiMatrix4x4 result =  globalTransformation * offsetMatrix;
    result.Transpose(); 
    XMMATRIX resultXM = XMLoadFloat4x4((const XMFLOAT4X4*)&result.a1);
    XMStoreFloat4x4(&m_FinalTransforms.at(gameobjectID).skeletonData[index],resultXM);

    for(unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessAnimation(animTime, node->mChildren[i], globalTransformation, gameobjectID);
    }
}

//Interpolate scaling.
void AnimationManager::InterpolateScaling(aiVector3D& result,float animTime, const aiNodeAnim* pNodeAnim)
{ 
    if(pNodeAnim->mNumScalingKeys == 1)
    {
        result = pNodeAnim->mScalingKeys[0].mValue;       
        return;
    }

    unsigned int scalingKey = FindScaling(animTime,pNodeAnim);
    unsigned int nextScalingKey = scalingKey + 1;
    float deltaTime = pNodeAnim->mScalingKeys[nextScalingKey].mTime - pNodeAnim->mScalingKeys[scalingKey].mTime;
    float factor = (animTime -(float)pNodeAnim->mScalingKeys[scalingKey].mTime) / deltaTime;
    const aiVector3D& start = pNodeAnim->mScalingKeys[scalingKey].mValue;
    const aiVector3D& end = pNodeAnim->mScalingKeys[nextScalingKey].mValue;
    aiVector3D delta = end - start;
    result = start + (factor * delta);
}

//Interpolate Position.
void AnimationManager::InterpolatePosition(aiVector3D& result,float animTime, const aiNodeAnim* pNodeAnim)
{
    if(pNodeAnim->mNumPositionKeys == 1)
    {
        result = pNodeAnim->mPositionKeys[0].mValue;       
        return;
    }

    unsigned int positionKey = FindPosition(animTime,pNodeAnim);
    unsigned int nextPositionKey = positionKey + 1;
    float deltaTime = pNodeAnim->mPositionKeys[nextPositionKey].mTime - pNodeAnim->mPositionKeys[positionKey].mTime;
    float factor = (animTime -(float)pNodeAnim->mPositionKeys[positionKey].mTime) / deltaTime; 

    const aiVector3D& start = pNodeAnim->mPositionKeys[positionKey].mValue;
    const aiVector3D& end = pNodeAnim->mPositionKeys[nextPositionKey].mValue;
    aiVector3D delta = end - start;
    result = start + (factor * delta);
}

//Slerp quaternions.
void AnimationManager::InterpolateRotation(aiQuaternion& result, float AnimTime, const aiNodeAnim* pNodeAnim)
{
    if(pNodeAnim->mNumRotationKeys == 1)
    {
        result = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    unsigned int rotationIndex = FindRotation(AnimTime, pNodeAnim);
    unsigned int nextRotationIndex = rotationIndex + 1;
    float startTime = (float)pNodeAnim->mRotationKeys[rotationIndex].mTime;
    float endTime = (float)pNodeAnim->mRotationKeys[nextRotationIndex].mTime;
    float deltaTime =  endTime - startTime; 
    float factor = (AnimTime - startTime) / deltaTime;
 
    const aiQuaternion& startRotationQ = pNodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& endRotationQ = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;

    aiQuaternion::Interpolate(result,startRotationQ,endRotationQ,factor);
    result.Normalize();
}

//Find scaling index.
unsigned int AnimationManager::FindScaling(float animTime, const aiNodeAnim* pNodeAnim)
{
    for(unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; ++i)
    {
        if(animTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

//Find rotation index.
unsigned int AnimationManager::FindRotation(float animTime, const aiNodeAnim* pNodeAnim)
{
    for(unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; ++i)
    {
        if(animTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

//Find position index.
unsigned int AnimationManager::FindPosition(float animTime, const aiNodeAnim* pNodeAnim)
{
    for(unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; ++i)
    {
        if(animTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

//returns the current backbuffer.
std::vector<SkeletonCBufferData>* AnimationManager:: GetFinalTransforms()
{
    return &m_FinalTransforms;
}

//Create the import task for the workerthreads.
Task* AnimationManager::ImportTask()
{
    Task* task = new Task;

    TaskData* data = new TaskData;
    data->parameter1 = nullptr;
    data->parameter2 = nullptr;
    data->parameter3 = nullptr;
    data->parameter4 = nullptr;
    data->parameter5 = nullptr;

    task->thisPointer = this;
    task->pTaskData = data;
    task->callback = DoneImporting;
    task->taskFunction = ImportAnimation;

    return task;
}

//Starts the importing process.
void AnimationManager::ImportAnimation(TaskData* pData, void* thisPointer)
{
    AnimationManager* self = static_cast<AnimationManager*>(thisPointer);
    self->DoImportAnimation(pData);
}

//Import the animation.
void AnimationManager::DoImportAnimation(TaskData* pData)
{
    const char* pFile = m_pAnimationPath;
    m_SkeletonBones.reserve(100);
   
    //Import and parse the file 
    m_pScene = m_pImporter->ReadFile(pFile,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_FixInfacingNormals |
        aiProcess_GenUVCoords |
        aiProcess_SplitByBoneCount);
    
    aiNode* rootNode = m_pScene->mRootNode->mChildren[0];
    ExtractSkeletonData(rootNode);
    m_numBones = m_SkeletonBones.size();
    GenerateOffsetMatrixes(m_pScene); 
}

//Generates the model's offsetmatix collection 
void AnimationManager::GenerateOffsetMatrixes(const aiScene* pScene)
{
    m_OffsetMatricesAssimp.resize(m_numBones);
    unsigned int numMeshes = pScene->mNumMeshes;
    unsigned int numBones  = 0;
    unsigned int index = 0;
    aiBone* bone;
    std::string boneName;
   
    for(unsigned int i = 0; i < numMeshes; ++i)
    {
        numBones = pScene->mMeshes[i]->mNumBones;
        for(unsigned int j = 0; j < numBones; ++j)
        {
            bone = pScene->mMeshes[i]->mBones[j];
            boneName = bone->mName.C_Str();
            index = FindBoneIndex(&boneName);
            m_OffsetMatricesAssimp.at(index) = bone->mOffsetMatrix;
        }
    }
}

//Callback function when done importing
void AnimationManager::DoneImporting(void* thisPointer, void* task)
{
    AnimationManager* self = static_cast<AnimationManager*>(thisPointer);
    Message doneImporting;
    doneImporting.MessageType = IMPORT_ANIMATION_DONE;
    self->m_pEngine->SubmitMessage(doneImporting);
}

//Extracts the skeleton bone data
void  AnimationManager::ExtractSkeletonData(const aiNode* node)
{
    unsigned int numChildBones = node->mNumChildren;
    aiNode* currentNode = new aiNode;
    *currentNode = *node;
    m_SkeletonBones.push_back(*currentNode);

    for(unsigned int i = 0; i < numChildBones; ++i)
    {
        ExtractSkeletonData(node->mChildren[i]);
    }
}

//Finds the bone index
unsigned int AnimationManager::FindBoneIndex(std::string* boneName)
{
    std::string boneNameToFind = *boneName;
    std::string curBoneName;
    for(unsigned int i = 0; i < m_numBones; ++i)
    {
        curBoneName = m_SkeletonBones[i].mName.C_Str();
        if(curBoneName == boneNameToFind)
        {
            return i;
        }
    }
    return 0;
}

//Finds the bone index
unsigned int AnimationManager::FindBoneIndex(const aiNode* node)
{  
    for(unsigned int i = 0; i < m_numBones; ++i)
    {
        if(m_SkeletonBones[i].mName == node->mName)
        {
            return i;
        }
    }
    return 0;
}