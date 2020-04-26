////////////////////////////////////////
// AnimatedAssimpModel.h
// 
// Class that inherits AssimpModel and adds animation
////////////////////////////////////////

#pragma once

#include "AssimpModel.h"
#include "SceneNode.h"
#include <unordered_map>

class AnimatedAssimpModel : public AssimpModel
{
public:
    AnimatedAssimpModel(const string& path, uint shader);
    ~AnimatedAssimpModel();

    void draw(SceneNode* node, const glm::mat4& model, const glm::mat4& viewProjMtx) override;
    void update(SceneNode* node) override;
    void updateBoneTransform(int animId, float TimeInSeconds);
    SceneNode * createSceneNodes(uint objectId, aiNode * curNode);
    void loadSceneNodes(SceneNode * node);
    void loadBoneFromSceneNodes(SceneNode * node);
    

private:
    /* Animation Data */
    chrono::system_clock::time_point startTime;

    // used when getting the animation value based on node name
    std::vector<std::unordered_map<std::string, aiNodeAnim*>*> animatedNodeMapList;

    aiNode* rootBone;
    void setRootBone();

    /* Animation related function */
    void calcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findAnimScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findAnimRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint findAnimPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* findAnimNode(const int animId, const string NodeName);
    void calcAnimByNodeTraversal(int animId, float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

    void loadBoneData(const aiMesh* mesh, vector<BoneReferenceData>& boneReferences) override;
};

