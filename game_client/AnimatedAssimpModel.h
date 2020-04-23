////////////////////////////////////////
// AnimatedAssimpModel.h
// 
// Class that inherits AssimpModel and adds animation
////////////////////////////////////////

#pragma once

#include "AssimpModel.h"

class AnimatedAssimpModel : public AssimpModel
{
public:
    AnimatedAssimpModel(const string& path);

    void draw(uint shader) override;
    void updateBoneTransform(float TimeInSeconds);

private:
    /* Animation Data */
    chrono::system_clock::time_point startTime;

    /* Animation related function */
    void calcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findAnimScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findAnimRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findAnimPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* findAnimNode(const aiAnimation* pAnimation, const string NodeName);
    void calcAnimByNodeTraversal(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

    void loadBoneData(const aiMesh* mesh, vector<BoneReferenceData>& boneReferences) override;
};

