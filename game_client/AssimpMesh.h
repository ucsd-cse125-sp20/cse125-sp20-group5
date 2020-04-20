////////////////////////////////////////
// AssimpMesh.h
// 
// Class that represents each mesh entity of an Assimp Model; each AssimpModel
// consists of multiple AssimpMesh
////////////////////////////////////////

#pragma once

#include "Core.h"//#include <glad/glad.h> // holds all OpenGL type declarations

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;

    //TODO
    //vector<VertexBoneData> Bones;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class AssimpMesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;
    glm::mat4 transform;

    /*  Functions  */
    AssimpMesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void draw(uint shader);
    void setupShadingAttributes(aiMaterial* material);

private:
    unsigned int VBO, EBO;
    void setupMesh();

    /* Shading attributes */
    glm::vec3 ambient; // Ka in .mat file
    glm::vec3 diffuse; // Kd
    glm::vec3 specular; // Ks
    /*
    // Refer to http://paulbourke.net/dataformats/mtl/
    glm::vec3 transmissionFilter; // Tf
    float opacityDensity; // Ni
    glm::vec3 specularReflectivity; // Ks
    glm::vec3 specularExponent; // Ns
    */
};
