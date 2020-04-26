////////////////////////////////////////
// AssimpModel.cpp
////////////////////////////////////////

#include "AssimpModel.h"

AssimpModel::AssimpModel()
{
	/* To be called by derived class in order to avoid calling overriden method through the base class constructor */
}

AssimpModel::AssimpModel(string const& path, uint shader)
{
	importScene(path, shader);

	glm::mat4 globalInverseTransform = convertToGlmMat(m_aiScene->mRootNode->mTransformation.Inverse());
	loadModelByNodeTraversal(m_aiScene->mRootNode, globalInverseTransform);

}

AssimpModel::~AssimpModel()
{
	for (Texture tex : textures_loaded) {
		glDeleteTextures(1, &(tex.id));
	}
}

// Because it is highly discouraged to call an overriden method in virtual, this method is created to avoid AnimatedAssimpModel 
// calling the above constructor which would eventually all an overriden method, loadBoneData()
void AssimpModel::importScene(const string& path, uint shader)
{
	this->shader = shader;

	m_aiScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!m_aiScene || m_aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_aiScene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void AssimpModel::loadModelByNodeTraversal(aiNode* node, const glm::mat4& parentTransform)
{
	// get node transformation
	glm::mat4 curNodeTransformation = convertToGlmMat(node->mTransformation);
	glm::mat4 globalTransformation = parentTransform * curNodeTransformation;

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = m_aiScene->mMeshes[node->mMeshes[i]];
		AssimpMesh loadedMesh = loadMesh(mesh, m_aiScene, globalTransformation);
		meshes.push_back(loadedMesh);
	}

	// recursive call on children
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		loadModelByNodeTraversal(node->mChildren[i], globalTransformation);
	}
}

// create an AssimpMesh for each mesh
AssimpMesh AssimpModel::loadMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 meshTransform)
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
	AssimpMesh resultMesh(vertices, indices, textures, meshTransform);
	resultMesh.setupShadingAttributes(material);

	return resultMesh;
}

void AssimpModel::loadBoneData(const aiMesh* mesh, vector<BoneReferenceData>& boneReferences) {
	/* to be overriden by AnimatedAssimpModel */
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

// for right now we don't have to do anythign for non animated models
void AssimpModel::update(SceneNode* node) {}

// draws the model, and thus all its meshes
void AssimpModel::draw(SceneNode* node, const glm::mat4& model, const glm::mat4& viewProjMtx)
{
	glUseProgram(shader);
	
	// create a temp model mtx
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&viewProjMtx);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);

	glUseProgram(0);

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
