#pragma once

#include<vector>
#include<queue>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<d3dx11.h>
#include<xnamath.h>
#include"Tasks.h"

struct Bone
{
    XMMATRIX transformation;
    int parentIndex;
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

struct SkeletonCBufferData
{
    XMMATRIX skeletonData[100];
};

class AnimationManager
{
public:
    AnimationManager();
    ~AnimationManager();

    Task* ImportTask(const char* pFile);

    //Returns the amount of animation task created
    unsigned int UpdateAnimation(float gameTime);
    void UpdateAnimationTest(float gameTime);
private:
    static void ImportAnimation(TaskData* pData, void* thisPointer);
    static void DoneImporting(void* thispointer, void* task);
    void DoImportAnimation(TaskData* pData);
    void GenerateAnimations(Animation* pAnimations, const aiScene* pScene);
    void GenerateOffsetMatrixes(XMMATRIX* OffsetMatrices, const aiScene* pScene);
    void GenerateSkeletonStructure(Bone* skeletonBones);
    void ExtractSkeletonData(std::vector<aiNode>* SkeletonBones,const aiNode* rootNode);
    unsigned int FindBoneIndex(std::string* boneName, std::vector<aiNode>* SkeletonBones);

    //Skeleton data calculations
    static void UpdateAnimation(TaskData* pData, void* thisPointer);
    static void DoneUpdatingAnimation(void* thisPointer, void* task);

    //Animation calculation
    void ProcessAnimation(float animTime, const aiNode* node, XMMATRIX& parentTransform);
    void InterpolateScaling(XMVECTOR& result,float animTime, const aiNodeAnim* pNodeAnim);
    void InterpolateRotation(XMVECTOR& result,float AnimTime, const aiNodeAnim* pNodeAnim); 
    void InterpolateRotation(aiQuaternion& result, float AnimTime, const aiNodeAnim* pNodeAnim); // testing
    void InterpolatePosition(XMVECTOR& result,float animTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindScaling(float animTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float animTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindPosition(float animTime, const aiNodeAnim* pNodeAnim);
    aiNodeAnim* FindNodeAnim(const aiAnimation* animation, std::string nodeName);

private:
    Animation* m_pAnimations;
    std::vector<aiNode>* m_pSkeletonBones;
    std::vector<Task*>* m_pAnimationTask;
    std::vector<float>* m_pAnimationTimes;
    std::vector<float>* m_pTimeScalers;
    XMMATRIX* m_pOffsetMatrices;
    Bone* m_pSkeletonStructure;  //Clean this up 
    const aiScene* m_pScene;
    Assimp::Importer* m_pImporter;
    XMMATRIX m_GlobalInverseTransform;
    unsigned int m_numBones;
    //Debug testing
    unsigned int animationIndex;
    unsigned int Keyframe;
public:
    bool done;
    std::vector<SkeletonCBufferData*>* m_pFinalTransformsCollection;
};