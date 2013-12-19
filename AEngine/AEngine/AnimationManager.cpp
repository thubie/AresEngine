#include"AnimationManager.h"

AnimationManager::AnimationManager()
{
    
    done = false;
}

AnimationManager::~AnimationManager()
{}

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
    //std::vector<aiNode>* SkeletonBones = (std::vector<aiNode>*)pData->parameter2;
    m_pSkeletonBones = new std::vector<aiNode>;
    m_pSkeletonBones->reserve(60);
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
    
    aiNode* rootNode = pScene->mRootNode->mChildren[0];
    ExtractSkeletonData(m_pSkeletonBones, rootNode);

    unsigned int numAnimations = pScene->mNumAnimations;
    unsigned int numChannels;
    unsigned int Index;
    std::string nameToFind;

    m_pAnimations = new Animation[numAnimations];
    aiAnimation* pAnimation;
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

                pKeyFrame->rotQuat.m128_f32[0] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.x;
                pKeyFrame->rotQuat.m128_f32[1] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.y;
                pKeyFrame->rotQuat.m128_f32[2] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.z;
                pKeyFrame->rotQuat.m128_f32[3] = pAnimation->mChannels[j]->mRotationKeys[k].mValue.w;

                pKeyFrame->scale.x = pAnimation->mChannels[j]->mScalingKeys[k].mValue.x;
                pKeyFrame->scale.y = pAnimation->mChannels[j]->mScalingKeys[k].mValue.y;
                pKeyFrame->scale.y = pAnimation->mChannels[j]->mScalingKeys[k].mValue.z;
            }
        }
    }

    //Test code
    /*unsigned int numBones = m_pSkeletonBones->size();

    for(int i = 0; i < numBones; ++i)
    {
        unsigned int index = 0;
        std::string nameToFind(m_pSkeletonBones->at(i).mName.C_Str());
        index = FindBoneIndex(&nameToFind, m_pSkeletonBones);
    }*/

    done = true;
}

void AnimationManager::DoneImporting(void* thispointer, void* task)
{

}


void AnimationManager::ExtractSkeletonData(std::vector<aiNode>* SkeletonBones, aiNode* Node)
{
    SkeletonBones->push_back(*Node);
    for(int i = 0; i < Node->mNumChildren; ++i)
    {
        ExtractSkeletonData(SkeletonBones, Node->mChildren[i]);
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