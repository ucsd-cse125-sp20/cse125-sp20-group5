////////////////////////////////////////
// AssimpModel.cpp
////////////////////////////////////////

#include "AssimpModel.h"

AssimpModel::AssimpModel(string const& path)
{
	// read file via ASSIMP
	m_aiScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	// check for errors
	if (!m_aiScene || m_aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_aiScene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	initFromScene(m_aiScene, path);
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void AssimpModel::initFromScene(const aiScene* scene, const string& path)
{
	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(loadMesh(mesh, scene));
	}

	// Check if there's too many bones
	assert(bones.size() <= 100); // which is the MAX_BONES, set in the vertex shader 
}

// create an AssimpMesh for each mesh
AssimpMesh AssimpModel::loadMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	vector<BoneReferenceData> boneReferences;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		aiVector3D pos = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		aiVector3D texCoord = mesh->HasTextureCoords(0) ? 
			mesh->mTextureCoords[0][i]
			: aiVector3D(0.0f, 0.0f, 0.0f);

		vertex.Position = glm::vec3(pos.x, pos.y, pos.z);
		vertex.Normal = glm::vec3(normal.x, normal.y, normal.z);
		vertex.TexCoords = glm::vec2(texCoord.x, texCoord.y);

		vertices.push_back(vertex);
	}

	// Load Bones for this mesh
	boneReferences.resize(vertices.size());
	loadBoneData(mesh, boneReferences);
	// Add bone reference data to the vertices
	for (unsigned int i = 0; i < vertices.size(); i++)
		vertices[i].BoneReference = boneReferences[i];

	// Walk through the mesh's faces (a mesh triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// Each diffuse texture in the shaders should be named as 'texture_diffuseN' (1 <= N <= MAX_SAMPLER_NUMBER). 
	// diffuse: texture_diffuseN
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// specular: texture_specularN
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// normal: texture_normalN
	vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// height maps
	vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	// ambient
	vector<Texture> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
	textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

	// return a mesh object created from the extracted mesh data
	AssimpMesh resultMesh(vertices, indices, textures);
	resultMesh.setupShadingAttributes(material);

	return resultMesh;
}

// populates the bone vectors which stores the transformation data for each bone, and
// populates the bone reference vectors for the mesh
void AssimpModel::loadBoneData(const aiMesh* mesh, vector<BoneReferenceData>& boneReferences) {
	for (uint i = 0; i < mesh->mNumBones; i++) {
		uint boneIndex = 0;
		string boneName(mesh->mBones[i]->mName.data);

		if (boneMap.find(boneName) == boneMap.end()) {
			// Allocate an index for a new bone
			boneIndex = bones.size();
			Bone bone;
			bones.push_back(bone);
			bones[boneIndex].boneOffset = convertToGlmMat(mesh->mBones[i]->mOffsetMatrix);
			boneMap[boneName] = boneIndex;
		}
		else {
			boneIndex = boneMap[boneName];
		}

		for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			uint vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			boneReferences[vertexID].addData(boneIndex, weight);
		}
	}
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> AssimpModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = textureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

// draws the model, and thus all its meshes
void AssimpModel::draw(uint shader)
{
	// feed in bone info (identity matrices if no animation)
	for (uint i = 0; i < bones.size(); i++) {
		// set the uniform
		string str = "gBones[" + to_string(i) + "]";
		const char* uniformName = &(str)[0];
		glUniformMatrix4fv(glGetUniformLocation(shader, uniformName), 1, false,
			(float*)&(bones[i].finalTransformation));
	}

	// Render
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
}

////////////////////////////////////////////////////////////////////////

glm::mat4 convertToGlmMat(const aiMatrix4x4& AssimpMatrix) {
	glm::mat4 m;
	m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
	m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
	m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
	m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
	return glm::transpose(m); // since aiMatrix4x4 treats glm::mat4's rows as columns
}

unsigned int AssimpModel::textureFromFile(const char* path, const string& directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
