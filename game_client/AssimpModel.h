////////////////////////////////////////
// AssimpModel.h
// 
// Class that imports a model from an external file by Assimp
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "stb_image.h"
#include "AssimpMesh.h"
#include "Drawable.h"

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

class AssimpModel : public Drawable
{
public:
    AssimpModel();
    AssimpModel(const string& path, uint shader);
    ~AssimpModel();

    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<AssimpMesh> meshes;
    string directory;
    bool gammaCorrection;

    virtual void draw(const glm::mat4& model, const glm::mat4& viewProjMtx) override;
    virtual void update(SceneNode * node) override;

protected:
    /* Animation Data */
    Assimp::Importer importer;
    const aiScene* m_aiScene;
    map<string, uint> boneMap; // maps a bone name to its index
    vector<Bone> bones;

    uint shader;

    void importScene(const string& path, uint shader);
    void loadModelByNodeTraversal(aiNode* node, const glm::mat4& parentTransform);

    AssimpMesh loadMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 meshTransform);
    virtual void loadBoneData(const aiMesh* mesh, vector<BoneReferenceData>& boneReferences);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName); 
    
    unsigned int textureFromFile(const char* path, const string& directory);
};

glm::mat4 convertToGlmMat(const aiMatrix4x4& AssimpMatrix); //TODO should be moved to a util class