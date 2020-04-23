////////////////////////////////////////
// AssimpModel.h
// 
// Class that imports a model from an external file by Assimp
////////////////////////////////////////

#pragma once

#include "Core.h"

#include "stb_image.h"

#include "AssimpMesh.h"

class AssimpModel
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<AssimpMesh> meshes;
    string directory;
    bool gammaCorrection;
    int nodeCount;
    int meshCount;

    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;
    glm::mat4 transform;

    AssimpModel(const string& path, uint shader, bool gamma = false);

    void draw(const glm::mat4& model, const glm::mat4& viewProjMtx);

private:

    Assimp::Importer importer;
    const aiScene* m_aiScene;

    void initFromScene(const aiScene* scene, const string& path);
    void processAnimNode(aiNodeAnim* node);
    void processNode(aiNode* node, const aiScene* scene);

    AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);

    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName); 
    unsigned int textureFromFile(const char* path, const string& directory, bool gamma = false);

    uint shader;
};