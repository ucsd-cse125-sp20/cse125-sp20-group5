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

#define NUM_BONES_PER_VEREX 4
/**
 * BoneReferenceData is a struct that, for each vertex, contains reference to the bone along
 * with the weights; the bone data is stored in AssimpModel and will be passed into the 
 * shaders through uniform.
 */
struct BoneReferenceData
{
    // The two arrays' indice
    uint boneIDs[NUM_BONES_PER_VEREX];
    float weights[NUM_BONES_PER_VEREX];

    BoneReferenceData()
    {
        memset(boneIDs, 0, sizeof(boneIDs));
        memset(weights, 0, sizeof(weights));
    };

    void addData(uint boneID, float weight) {
        for (uint i = 0; i < NUM_BONES_PER_VEREX; i++) {
            if (weights[i] == 0.0) {
                boneIDs[i] = boneID;
                weights[i] = weight;
                return;
            }
        }
        // should never get here - more bones than we have space for
        assert(0);
    }
};


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    BoneReferenceData BoneReference;
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

    /* Animation Related Data */
    /*uint baseVertexIndex;
    uint baseIndex;
    uint numIndices;
    uint materialIndex;*/

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
