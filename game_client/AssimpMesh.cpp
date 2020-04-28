////////////////////////////////////////
// AssimpMesh.cpp
////////////////////////////////////////

#include "AssimpMesh.h"

AssimpMesh::AssimpMesh(	vector<Vertex> vertices, 
											vector<unsigned int> indices, 
											vector<Texture> textures,
											glm::mat4 transform=glm::mat4(1.0f))
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->transform = transform;

	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// load data into vertex buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	/* Set the vertex attribute pointers */
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex reference to the bone data
	//		bone ID
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneReference));
	//		weights of each bone
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, BoneReference) + offsetof(BoneReferenceData, weights))); // 16 needs to be checked TODO

	// Set indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

AssimpMesh::~AssimpMesh()
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void AssimpMesh::draw(uint shader)
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	unsigned int ambientNr = 1;

	GLint uniformHasTexture = glGetUniformLocation(shader, "hasTexture");
	if (textures.empty()) {
		// reset textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUniform1i(uniformHasTexture, 0);
	}
	else {
		glUniform1i(uniformHasTexture, 1);

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++); // transfer unsigned int to stream
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);
			else if (name == "texture_ambient")
				number = std::to_string(ambientNr++);

			glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
	}

	// set shading attributes
	glUniform3fv(glGetUniformLocation(shader, "diffuseColor"), 1, &diffuse[0]);
	glUniform3fv(glGetUniformLocation(shader, "ambientColor"), 1, &ambient[0]);
	glUniform3fv(glGetUniformLocation(shader, "specularColor"), 1, &specular[0]);

	glUniformMatrix4fv(glGetUniformLocation(shader, "meshTransform"), 1, false, (float*)&transform);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0); //(TODO: can be redundant)
}

void AssimpMesh::setupShadingAttributes(aiMaterial* material) 
{
	aiColor3D kd(0.f, 0.f, 0.f), ka(0), ks(0);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
	material->Get(AI_MATKEY_COLOR_AMBIENT, ka);
	material->Get(AI_MATKEY_COLOR_SPECULAR, ks);
	this->diffuse = glm::vec3(kd.r, kd.g, kd.b);
	this->ambient = glm::vec3(ka.r, ka.g, ka.b);
	this->specular = glm::vec3(ks.r, ks.g, ks.b);
}