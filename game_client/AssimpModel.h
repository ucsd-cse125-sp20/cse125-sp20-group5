#ifndef ASSIMP_MODEL_H
#define ASSIMPMODELH

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "TestMesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class AssimpModel
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;
    int nodeCount;
    int meshCount;

    AssimpModel(const string& path, bool gamma = false);

    void draw(uint shader);

private:

    void loadModel(string const& path);
    void processAnimNode(aiNodeAnim* node);
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName); 
    unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
};


#endif