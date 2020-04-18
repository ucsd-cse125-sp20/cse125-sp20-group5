////////////////////////////////////////
// AssimpMesh.h
// 
// Class that represents each mesh entity of an Assimp Model; each AssimpModel
// consists of multiple AssimpMesh
////////////////////////////////////////

#pragma once

#include "Core.h"//#include <glad/glad.h> // holds all OpenGL type declarations

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
    glm::vec3 color; // TODO: for test

    /*  Functions  */
    AssimpMesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void draw(uint shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};
