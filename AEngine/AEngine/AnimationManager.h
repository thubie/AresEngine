#pragma once

#include<vector>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"Tasks.h"
#include"AEngine.h"
#include"SkeletonBuffer.h"

class AEngine;

class AnimationManager
{
public:
    AnimationManager(AEngine* pEngine, const char* currentDir);
    ~AnimationManager();

    Task* ImportTask();
    unsigned int CreateAnimationTasks(); //Returns the amount of animation task created   
    void UpdateAnimationTime(float elapsedTime);
    void RegisterGameObjects(std::vector<unsigned int>& gameId);
    std::vector<SkeletonCBufferData>* GetFinalTransforms(); 

private:
    static void ImportAnimation(TaskData* pData, void* thisPointer);
    static void DoneImporting(void* thispointer, void* task);
    void DoImportAnimation(TaskData* pData);
    void GenerateOffsetMatrixes(const aiScene* pScene);
    void ExtractSkeletonData(const aiNode* rootNode);
    unsigned int FindBoneIndex(std::string* boneName);
    unsigned int FindBoneIndex(const aiNode* node);

    //Skeleton data calculations
    static void AnimationUpdate(TaskData* pData, void* thisPointer);
    static void DoneUpdatingAnimation(void* thisPointer, void* task);
    void DoAnimationUpdate(TaskData* pData);

    //Animation calculation
    void ProcessAnimation(float animTime, const aiNode* node, aiMatrix4x4& parentTransform, unsigned int gameobjectID);
    
    void InterpolateScaling(aiVector3D& result,float animTime, const aiNodeAnim* pNodeAnim);
    void InterpolatePosition(aiVector3D& result,float animTime, const aiNodeAnim* pNodeAnim);
    void InterpolateRotation(aiQuaternion& result, float AnimTime, const aiNodeAnim* pNodeAnim);
    
    unsigned int FindScaling(float animTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float animTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindPosition(float animTime, const aiNodeAnim* pNodeAnim);
    aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const aiNode* node); 

private:   
    AEngine* m_pEngine;
    const aiScene* m_pScene;
    Assimp::Importer* m_pImporter;
    char* m_pAnimationPath;
    LONG m_OpenAnimationTasks;
    unsigned int m_numBones;
    unsigned int animationIndex;
    float m_DeltaGameTime;
    
    std::vector<unsigned int> m_GameObjectIds;    
    std::vector<float> m_TimeScalers;
    std::vector<float> m_AnimationTimes;
    std::vector<aiNode> m_SkeletonBones;   
    std::vector<TaskData> m_AnimationTaskData;
    std::vector<aiMatrix4x4> m_OffsetMatricesAssimp;
    std::vector<SkeletonCBufferData> m_FinalTransforms;

public:    
    std::vector<Task> m_AnimationTask;
};