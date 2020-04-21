////////////////////////////////////////
// AssimpModel.cpp
////////////////////////////////////////

#include "AssimpModel.h"

AssimpModel::AssimpModel(string const& path, bool gamma) : gammaCorrection(gamma)
{
	nodeCount = 0;
	meshCount = 0;
	
	// read file via ASSIMP
	m_aiScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	// check for errors
	if (!m_aiScene || m_aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_aiScene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}

	// TODO: animation
	for (int i = 0; i < m_aiScene->mNumAnimations; i++) {
		std::cout << "processing an animation Animations" << std::endl;
		std::cout << "name of animation " << m_aiScene->mAnimations[i]->mName.C_Str() << std::endl;
		std::cout << "duraiton of animation in ticks " << m_aiScene->mAnimations[i]->mDuration << std::endl;
		std::cout << "ticks per sceond " << m_aiScene->mAnimations[i]->mTicksPerSecond << std::endl;
		for (int j = 0; j < m_aiScene->mAnimations[i]->mNumChannels; j++) {
			processAnimNode(m_aiScene->mAnimations[i]->mChannels[j]);
		}
	}

	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	initFromScene(m_aiScene, path);
	//processNode(m_aiScene->mRootNode, m_aiScene);
}

//TODO: animation
void AssimpModel::processAnimNode(aiNodeAnim* node) {
	std::cout << "Animation node process " << (node->mNodeName).C_Str() << nodeCount << std::endl;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void AssimpModel::processNode(aiNode* node, const aiScene* scene)
{
	std::cout << "node process " << (node->mName).C_Str() << " " << nodeCount << " " << node->mNumChildren << std::endl;
	nodeCount++;
	//std::cout << node->mTransformation[1] << std::endl;;


	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (mesh->HasBones()) {
			std::cout << "has bonesss BONESSSSSSSSSSSSSSSS" << std::endl;
		}
		meshes.push_back(processMesh(mesh, scene));
	}

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

AssimpMesh AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::cout << "mesh process " << meshCount << std::endl;
	meshCount++;

	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		aiVector3D pos = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		aiVector3D texCoord = mesh->HasTextureCoords(0) ? 
			mesh->mTextureCoords[0][i]
			: aiVector3D(0.0f, 0.0f, 0.0f);

		vertex.Position = glm::vec3(pos.x, pos.y, pos.z);
		vertex.Normal = glm::vec3(normal.x, normal.y, normal.z);
		vertex.TexCoords = glm::vec2(texCoord.x, texCoord.y);
		
		// TODO
		//loadBone()

		vertices.push_back(vertex);
	}

	// Walk through the mesh's faces (a mesh triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		// assert(Face.mNumIndices == 3);

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 

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

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> AssimpModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
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
void AssimpModel::draw(const glm::mat4& model, const glm::mat4& viewProjMtx, uint shader)
{
	glUseProgram(shader);

	// create a temp model mtx
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&viewProjMtx);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);

	glUseProgram(0);

}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void AssimpModel::initFromScene(const aiScene* scene, const string& path)
{
	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(processMesh(mesh, scene));
	}

	//return InitMaterials(pScene, Filename);
}



unsigned int AssimpModel::textureFromFile(const char* path, const string& directory, bool gamma)
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
