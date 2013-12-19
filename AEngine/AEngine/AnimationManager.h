#pragma once

#include<vector>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<d3dx11.h>
#include<xnamath.h>
#include"Tasks.h"

struct Bone
{
    XMFLOAT3 position;
    XMVECTOR rotQuat;
    XMFLOAT3 scale;
};

struct KeyFrame
{
    float time;
    XMFLOAT3 position;
    XMVECTOR rotQuat;
    XMFLOAT3 scale;
};

struct Channel
{
    unsigned int boneIndex;
    unsigned int numKeyFrames;
    KeyFrame* keyFrames;
};

struct Animation
{
    unsigned int animationId;
    float duration;
    float ticksPerSecond;
    unsigned int numChannels;
    Channel* channels;
};
class AnimationManager
{
public:
    AnimationManager();
    ~AnimationManager();

    Task* ImportTask(const char* pFile);
    
private:
    static void ImportAnimation(TaskData* pData, void* thisPointer);
    static void DoneImporting(void* thispointer, void* task);
    void DoImportAnimation(TaskData* pData);

    void GenerateAnimations(std::vector<Animation>* Animations);

    void ExtractSkeletonData(std::vector<aiNode>* SkeletonBones, aiNode* Node);
    unsigned int FindBoneIndex(std::string* boneName, std::vector<aiNode>* SkeletonBones);

private:
    std::vector<aiNode>* m_pSkeletonBones;
    Animation* m_pAnimations;

public:
    bool done;
};