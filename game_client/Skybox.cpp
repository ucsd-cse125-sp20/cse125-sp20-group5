#include "Skybox.h"

Skybox::Skybox(uint shader, glm::mat4 modelMtx)
{
	this->shader = shader;
	this->modelMtx = modelMtx;

	toWorld = glm::scale(glm::mat4(1.f), glm::vec3(400.f));
	std::string directory = "texture/skybox/";
	filePaths.push_back(directory + "starfield_rt.ppm");
	filePaths.push_back(directory + "starfield_lf.ppm");
	filePaths.push_back(directory + "starfield_up.ppm");
	filePaths.push_back(directory + "starfield_dn.ppm");
	filePaths.push_back(directory + "starfield_ft.ppm");
	filePaths.push_back(directory + "starfield_bk.ppm");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	loadTexture();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Skybox::~Skybox()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Skybox::draw(const glm::mat4& viewProjMtx)
{
	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&modelMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&viewProjMtx);

	// set skybox texture ID
	glUniform1i(glGetUniformLocation(shader, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // TODO

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glUseProgram(0);
}

// load image file into texture object
void Skybox::loadTexture()
{
	std::vector<unsigned char*> tdata;  // texture pixel data
	int width, height, nrComponents;

	// Load image file
	for (int i = 0; i < filePaths.size(); i++) {
		tdata.push_back(stbi_load(filePaths[i].c_str(), &width, &height, &nrComponents, 0));
	}

	GLenum colorFormat;
	if (nrComponents == 1)
		colorFormat = GL_RED;
	else if (nrComponents == 3)
		colorFormat = GL_RGB;
	else if (nrComponents == 4)
		colorFormat = GL_RGBA;

	// Create ID for texture
	glGenTextures(1, &textureID);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// Generate the texture
	unsigned char* data;
	for (GLuint i = 0; i < tdata.size(); i++)
	{
		data = tdata[i];
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, colorFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
	}

	// Make sure no bytes are padded
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Use clamp to edge to hide skybox edges
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}