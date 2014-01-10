#include"AnimationManager.h"

AnimationManager::AnimationManager()
{
    m_pAnimations = nullptr;  
    m_pFinalTransformsCollection = new std::vector<SkeletonCBufferData*>;
    SkeletonCBufferData* test = (SkeletonCBufferData*)_aligned_malloc(sizeof(SkeletonCBufferData),16);
    m_pFinalTransformsCollection->push_back(test);
    m_pImporter = new Assimp::Importer;
    animationIndex = 0;
    Keyframe = 0;
    done = false;
}

AnimationManager::~AnimationManager()
{
    //Delete data
}

Task* AnimationManager::ImportTask(const char* pFile)
{
    Task* task = new Task;

    TaskData* data = new TaskData;
    data->parameter1 = (void*)pFile;
    data->parameter2 = (void*)m_pSkeletonBones;
    data->parameter3 = nullptr;
    data->parameter4 = nullptr;
    data->parameter5 = nullptr;

    task->thisPointer = this;
    task->pTaskData = data;
    task->callback = DoneImporting;
    task->taskFunction = ImportAnimation;

    return task;
}

void AnimationManager::ImportAnimation(TaskData* pData, void* thisPointer)
{
    AnimationManager* self = static_cast<AnimationManager*>(thisPointer);
    self->DoImportAnimation(pData);
}

void AnimationManager::DoImportAnimation(TaskData* pData)
{
    const char* pFile = (char*)pData->parameter1;
    m_pSkeletonBones = new std::vector<aiNode>;
    m_pSkeletonBones->reserve(100);
   
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
    XMMATRIX globalInverse = XMLoadFloat4x4((XMFLOAT4X4*)&rootNode->mTransformation.a1);
    XMVECTOR determinant = XMMatrixDeterminant(globalInverse);
    m_GlobalInverseTransform = XMMatrixInverse(&determinant, globalInverse);
    ExtractSkeletonData(m_pSkeletonBones, rootNode);
    m_numBones = m_pSkeletonBones->size();
    GenerateOffsetMatrixes(m_pOffsetMatrices, m_pScene);
    done = true;
}

void AnimationManager::GenerateAnimations(Animation* pAnimations, const aiScene* pScene)
{

    unsigned int numAnimations = pScene->mNumAnimations;
    unsigned int numChannels;
    unsigned int Index;
    std::string nameToFind;
    aiAnimation* pAnimation;
    m_pAnimations = new Animation[numAnimations];

    for(int i = 0; i < numAnimations; ++i)
    {
        pAnimation = pScene->mAnimations[i];
        m_pAnimations[i].animationId = i;
        m_pAnimations[i].duration = pAnimation->mDuration;
        m_pAnimations[i].ticksPerSecond = pAnimation->mTicksPerSecond;
        m_pAnimations[i].numChannels = pAnimation->mNumChannels;

        numChannels = m_pAnimations[i].numChannels;
        m_pAnimations[i].channels = new Channel[numChannels];
                
        for(int j = 0; j < numChannels; ++j)
        {
            Channel* pChannel = &m_pAnimations[i].channels[j];
            std::string nameToFind(pScene->mAnimations[i]->mChannels[j]->mNodeName.C_Str());
            Index = FindBoneIndex(&nameToFind, m_pSkeletonBones);
            pChannel->boneIndex = Index;
            pChannel->numKeyFrames = pAnimation->mChannels[j]->mNumPositionKeys;
            pChannel->keyFrames = new KeyFrame[pChannel->numKeyFrames];
            for(int k = 0; k < pChannel->numKeyFrames; ++k)
            {
                KeyFrame* pKeyFrame = &pChannel->keyFrames[k];
                pKeyFrame->time = pAnimation->mChannels[j]->mPositionKeys[k].mTime;
                pKeyFrame->position.x = pAnimation->mChannels[j]->mPositionKeys[k].mValue.x;
                pKeyFrame->position.y = pAnimation->mChannels[j]->mPositionKeys[k].mValue.y;
                pKeyFrame->position.z = pAnimation->mChannels[j]->mPositionKeys[k].mValue.z;

                //Need to change if order is wxyz instead of now the used xyzw!!!
                pKeyFrame->rotQuat.m128_f32[0] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.x;
                pKeyFrame->rotQuat.m128_f32[1] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.y;
                pKeyFrame->rotQuat.m128_f32[2] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.z;
                pKeyFrame->rotQuat.m128_f32[3] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.w;

                pKeyFrame->scale.x = pAnimation->mChannels[j]->mScalingKeys[k].mValue.x;
                pKeyFrame->scale.y = pAnimation->mChannels[j]->mScalingKeys[k].mValue.y;
                pKeyFrame->scale.z = pAnimation->mChannels[j]->mScalingKeys[k].mValue.z;
            }
        }
    }
}

void AnimationManager::GenerateSkeletonStructure(Bone* skeletonBones)
{
    unsigned numBones = m_numBones;
    m_pSkeletonStructure = new Bone[numBones];
    Bone currentBone;
    std::string parentName;
    aiNode* bone;

    bone = &m_pSkeletonBones->at(0);
    currentBone.transformation = XMLoadFloat4x4((XMFLOAT4X4*)&bone->mTransformation.a1);
    currentBone.parentIndex = -1;
    m_pSkeletonStructure[0] = currentBone;

    for(int i = 1; i < numBones; ++i)
    {
        bone = &m_pSkeletonBones->at(i);
        currentBone.transformation = XMLoadFloat4x4((XMFLOAT4X4*)&bone->mTransformation.a1);
        parentName = bone->mParent->mName.C_Str();
        currentBone.parentIndex = FindBoneIndex(&parentName, m_pSkeletonBones);
        m_pSkeletonStructure[i] = currentBone;
    }
    bool test = true;
}

void AnimationManager::GenerateOffsetMatrixes(XMMATRIX* pOffsetMatrices, const aiScene* pScene)
{
    m_pOffsetMatrices = (XMMATRIX*) _aligned_malloc(sizeof(XMMATRIX) * m_numBones, 16); //new XMMATRIX[m_numBones];
    unsigned int numMeshes = pScene->mNumMeshes;
    unsigned int numBones  = 0;
    unsigned int index = 0;
    aiBone* bone;
    std::string boneName;
    XMMATRIX offsetMatrix;

    for(int i = 0; i < numMeshes; ++i)
    {
        numBones = pScene->mMeshes[i]->mNumBones;
        for(int j = 0; j < numBones; ++j)
        {
            bone = pScene->mMeshes[i]->mBones[j];
            boneName = bone->mName.C_Str();
            index = FindBoneIndex(&boneName, m_pSkeletonBones);
            offsetMatrix = XMLoadFloat4x4((XMFLOAT4X4*)&bone->mOffsetMatrix.a1);
            m_pOffsetMatrices[index] = offsetMatrix;
        }
    }
}

void AnimationManager::DoneImporting(void* thispointer, void* task)
{

}

void  AnimationManager::ExtractSkeletonData(std::vector<aiNode>* SkeletonBones,const aiNode* node)
{
    unsigned int numChildBones = node->mNumChildren;
    aiNode* currentNode = new aiNode;
    *currentNode = *node;
    m_pSkeletonBones->push_back(*currentNode);

    for(int i = 0; i < numChildBones; ++i)
    {
        ExtractSkeletonData(SkeletonBones,node->mChildren[i]);
    }
}

unsigned int AnimationManager::FindBoneIndex(std::string* boneName, std::vector<aiNode>* SkeletonBones)
{
    unsigned int Index = 0;
    unsigned int numBones = SkeletonBones->size();
    std::string boneNameToFind = *boneName;
    std::string curBoneName;
    for(int i = 0; i < numBones; ++i)
    {
        curBoneName = SkeletonBones->at(i).mName.C_Str();
        if(curBoneName == boneNameToFind)
        {
            return i;
        }
    }
}

void AnimationManager::UpdateAnimation(TaskData* pData, void* thisPointer)
{

}

void AnimationManager::DoneUpdatingAnimation(void* thisPointer, void* task)
{

}


void AnimationManager::UpdateAnimationTest(float gameTime)
{
    XMMATRIX idMatrix = XMMatrixIdentity();

    //Get the ticks per second if the ticks per second isn't zero.
    //Set ticks per second to the ticks per second of the animation 
    //else set it at 25 ticks per second
    float tickPerSecond = m_pScene->mAnimations[animationIndex]->mTicksPerSecond != 0 ?
        m_pScene->mAnimations[animationIndex]->mTicksPerSecond : 25.0f;

    float timeInTicks = gameTime * tickPerSecond;
    float animationTime = fmod(timeInTicks, m_pScene->mAnimations[animationIndex]->mDuration);

    ProcessAnimation(animationTime, m_pScene->mRootNode->mChildren[0], idMatrix);
}

aiNodeAnim* AnimationManager::FindNodeAnim(const aiAnimation* animation, std::string nodeName)
{
    unsigned int numChannels = animation->mNumChannels;
    aiNodeAnim* result = nullptr;
    std::string currentNodeName;

    for(int i = 0; i < numChannels; ++i)
    {
        currentNodeName = animation->mChannels[i]->mNodeName.data;
        if(currentNodeName == nodeName)
        {
            result = animation->mChannels[i];
        }
    }
    return result;
}

void AnimationManager::ProcessAnimation(float animTime, const aiNode* node, XMMATRIX& parentTransform)
{
    std::string NodeName(node->mName.data);
    unsigned int index = FindBoneIndex(&NodeName,m_pSkeletonBones); 
    XMMATRIX nodeTransformation = XMLoadFloat4x4((XMFLOAT4X4*)&node->mTransformation.a1);
    const aiAnimation* pAnimation = m_pScene->mAnimations[animationIndex];
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    //pNodeAnim = nullptr;
    if(pNodeAnim)
    {
        //Interpolate rotation and generate rotation transformation matrix
        aiQuaternion rotationQ;
        XMVECTOR position;
        XMVECTOR scaling;
        XMVECTOR rotation;
       
        XMMATRIX rotationMat;
        XMMATRIX positionMat;
        XMMATRIX scalingMat;

        InterpolateRotation(rotation, animTime,pNodeAnim);
        InterpolateRotation(rotationQ,animTime,pNodeAnim);
        InterpolatePosition(position,animTime,pNodeAnim);
        InterpolateScaling(scaling,animTime,pNodeAnim);

        aiMatrix4x4 rotationAssimp;
        rotationAssimp = aiMatrix4x4(rotationQ.GetMatrix());
        rotationMat = XMLoadFloat4x4((const XMFLOAT4X4*)&rotationAssimp.a1);

        //rotationMat = XMMatrixRotationQuaternion(rotation);
        //rotationMat = XMMatrixTranspose(rotationMat);
        scalingMat = XMMatrixScalingFromVector(scaling);
        //scalingMat = XMMatrixTranspose(scalingMat);
        positionMat = XMMatrixTranslationFromVector(position);
        //positionMat = XMMatrixTranspose(positionMat);

        rotationMat._11 *= scaling.m128_f32[0]; rotationMat._21 *= scaling.m128_f32[0];  rotationMat._31 *= scaling.m128_f32[0]; 
        rotationMat._12 *= scaling.m128_f32[1]; rotationMat._22 *= scaling.m128_f32[1];  rotationMat._32 *= scaling.m128_f32[1];
        rotationMat._13 *= scaling.m128_f32[2]; rotationMat._23 *= scaling.m128_f32[2];  rotationMat._33 *= scaling.m128_f32[2];
        rotationMat._14 *= position.m128_f32[0]; rotationMat._24 *= position.m128_f32[1];  rotationMat._34 *= position.m128_f32[3];

        nodeTransformation = rotationMat;//rotationMat * scalingMat * positionMat ;
        //nodeTransformation = XMMatrixTranspose(nodeTransformation);
        bool finish = true;
    }

    XMMATRIX globalTransformation = parentTransform * nodeTransformation ;
    XMMATRIX GlobalInverse = m_GlobalInverseTransform;
    XMMATRIX offsetMatrix = m_pOffsetMatrices[index];
    XMMATRIX result =  GlobalInverse * globalTransformation * offsetMatrix;
    result = XMMatrixTranspose(result);
    m_pFinalTransformsCollection->at(0)->skeletonData[index] = result;

    for(int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessAnimation(animTime, node->mChildren[i], globalTransformation);
    }
}

//lerp scaling
void AnimationManager::InterpolateScaling(XMVECTOR& result,float animTime, const aiNodeAnim* pNodeAnim)
{
    if(pNodeAnim->mNumScalingKeys == 1)
    {
        result = XMLoadFloat3((const XMFLOAT3*)&pNodeAnim->mScalingKeys[0].mValue);       
        return;
    }

    unsigned int scalingKey = FindScaling(animTime,pNodeAnim);
    unsigned int nextScalingKey = scalingKey + 1;

    XMVECTOR startScale;
    XMVECTOR endScale;

    startScale =  XMLoadFloat3((const XMFLOAT3*)&pNodeAnim->mScalingKeys[scalingKey].mValue.x);
    endScale = XMLoadFloat3((const XMFLOAT3*)&pNodeAnim->mScalingKeys[nextScalingKey].mValue.x);
    
    float deltaTime = pNodeAnim->mScalingKeys[nextScalingKey].mTime - pNodeAnim->mScalingKeys[scalingKey].mTime;
    float factor = (animTime -(float)pNodeAnim->mScalingKeys[scalingKey].mTime) / deltaTime;; 
    result =  XMVectorLerp(startScale, endScale, factor);
}



//Lerp Position
void AnimationManager::InterpolatePosition(XMVECTOR& result,float animTime, const aiNodeAnim* pNodeAnim)
{
    if(pNodeAnim->mNumPositionKeys == 1)
    {
        result = XMLoadFloat3((const XMFLOAT3*)&pNodeAnim->mScalingKeys[0].mValue); 
        return;
    }

    unsigned int scalingKey = FindPosition(animTime,pNodeAnim);
    unsigned int nextScalingKey = scalingKey + 1;

    XMVECTOR startPosition;
    XMVECTOR endPosition;

    startPosition =  XMLoadFloat3((const XMFLOAT3*)&pNodeAnim->mScalingKeys[scalingKey].mValue.x);
    endPosition = XMLoadFloat3((const XMFLOAT3*)&pNodeAnim->mScalingKeys[nextScalingKey].mValue.x);
    
    float deltaTime = pNodeAnim->mScalingKeys[nextScalingKey].mTime - pNodeAnim->mScalingKeys[scalingKey].mTime;
    float factor = (animTime -(float)pNodeAnim->mScalingKeys[scalingKey].mTime) / deltaTime; 
    result = XMVectorLerp(startPosition, endPosition, factor);
}

//slerp quaternions
void AnimationManager::InterpolateRotation(XMVECTOR& result, float AnimTime, const aiNodeAnim* pNodeAnim)
{
    if(pNodeAnim->mNumRotationKeys == 1)
    {
        result.m128_f32[0] = pNodeAnim->mRotationKeys[0].mValue.x;
        result.m128_f32[1] = pNodeAnim->mRotationKeys[0].mValue.y;
        result.m128_f32[2] = pNodeAnim->mRotationKeys[0].mValue.z;
        result.m128_f32[3] = pNodeAnim->mRotationKeys[0].mValue.w;
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
    XMVECTOR startQ;
    XMVECTOR endQuat;
    XMVECTOR resultQuat;

    startQ.m128_f32[0] = startRotationQ.x;
    startQ.m128_f32[1] = startRotationQ.y;
    startQ.m128_f32[2] = startRotationQ.z;
    startQ.m128_f32[3] = startRotationQ.w;

    endQuat.m128_f32[0] = endRotationQ.x;
    endQuat.m128_f32[1] = endRotationQ.y;
    endQuat.m128_f32[2] = endRotationQ.z;
    endQuat.m128_f32[3] = endRotationQ.w;

    resultQuat = XMQuaternionSlerp(startQ, endQuat,factor);
    result = XMQuaternionNormalize(resultQuat);
    bool finish = true;
}

//Slerp quaternions
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

//Scaling index
unsigned int AnimationManager::FindScaling(float animTime, const aiNodeAnim* pNodeAnim)
{
    for(int i = 0; i < pNodeAnim->mNumScalingKeys - 1; ++i)
    {
        if(animTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

//Find rotation index
unsigned int AnimationManager::FindRotation(float animTime, const aiNodeAnim* pNodeAnim)
{
    for(int i = 0; i < pNodeAnim->mNumRotationKeys - 1; ++i)
    {
        if(animTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

//Find position index
unsigned int AnimationManager::FindPosition(float animTime, const aiNodeAnim* pNodeAnim)
{
    for(int i = 0; i < pNodeAnim->mNumPositionKeys - 1; ++i)
    {
        if(animTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
            return i;
    }

    return 0;
}