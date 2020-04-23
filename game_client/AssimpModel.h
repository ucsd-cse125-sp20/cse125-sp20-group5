////////////////////////////////////////
// AssimpModel.h
// 
// Class that imports a model from an external file by Assimp
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "stb_image.h"
#include "AssimpMesh.h"

struct Bone
{
    glm::mat4 boneOffset;
    glm::mat4 finalTransformation;

    Bone()
    {
        boneOffset = glm::mat4(1); //identity
        finalTransformation = glm::mat4(1);
    }
};

class AssimpModel
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<AssimpMesh> meshes;
    string directory;
    bool gammaCorrection;

    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;
    glm::mat4 transform;

    AssimpModel(const string& path);

    void draw(uint shader);
    void updateBoneTransform(float TimeInSeconds);

private:
    /* Animation Data */
    Assimp::Importer importer;
    const aiScene* m_aiScene;
    map<string, uint> boneMap; // maps a bone name to its index
    vector<Bone> bones;
    chrono::system_clock::time_point startTime;

    /* Animation related function */
    void calcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const string NodeName);
    void traverseNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

    void initFromScene(const aiScene* scene, const string& path);

    AssimpMesh loadMesh(aiMesh* mesh, const aiScene* scene);
    void loadBoneData(const aiMesh* mesh, vector<BoneReferenceData>& boneReferences);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName); 
    
    unsigned int textureFromFile(const char* path, const string& directory);
};

glm::mat4 convertToGlmMat(const aiMatrix4x4& AssimpMatrix); //TODO should be moved to a util class