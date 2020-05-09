#include "HealthBar.h"

using namespace glm;
using namespace std;

// height and width = 2.0 always even after resize 

HealthBar::HealthBar(uint shader, const char* iconFile, glm::vec3 barColor)
{
	this->shader = shader;
	this->modelMtx = scale(vec3(1.0f));

	// Init the filling status
	float init_filledFraction = 0.3f;
	updateBar(init_filledFraction);

	// Compute the transforms
	float iconWidth = 1.0f;
	float gap = 0.25f;
	float barWidth = 4.0f;
	float barHeight = 0.5f;
	float barEdgePad = barHeight * 0.1f;

	mat4 barTransform = translate(vec3((iconWidth + gap) / 2.0f, 0, 0)) * scale(vec3(barWidth, barHeight, 1));

	mat4 iconMtx = translate(vec3(-(barWidth + gap) / 2.0f, 0, 0))
		* scale(vec3(iconWidth, iconWidth, 1));
	mat4 barBoxMtx = barTransform;
	mat4 barMtx = barTransform
		* scale(vec3((barWidth - 2.0f * barEdgePad) / barWidth, (barHeight - 2.0f * barEdgePad) / barHeight, 1)); // shrink a little from the box

	vec3 whiteColor = vec3(1,1,1);

	// Init each bar components
	barComponents.resize(3);
	barComponents[ICON] = BarComponent(ICON, iconMtx, whiteColor);
	barComponents[BAR_BOX] = BarComponent(BAR_BOX, barBoxMtx, whiteColor);
	barComponents[BAR] = BarComponent(BAR, barMtx, barColor);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		2, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		2 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	for (BarComponent bc : barComponents) {
		if (bc.id == ICON) {
			loadTexture(iconFile, bc.id);
		}
		else {
			loadTexture(NO_TEXTURE, bc.id);
		}
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}

HealthBar::~HealthBar()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO);
}

void HealthBar::draw(const glm::mat4& viewProjMtx)
{
	// Animate the decrease of the bar if filledFraction got updated before
	updateBar(this->filledFraction);

	glUseProgram(shader);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND); 
	glDisable(GL_DEPTH_TEST); // to allow overlap
	glBindVertexArray(VAO);


	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&viewProjMtx);

	for (BarComponent bc : barComponents) {
		//glUniform1i(glGetUniformLocation(shader, "myTexture"), textureIDs[bc->id]);

		glBindTexture(GL_TEXTURE_2D, textureIDs[bc.id]);

		mat4 model = modelMtx * bc.localMtx;
		if (bc.id == BAR) {
			model = model * this->fillingTransform;
		}
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&model);
		glUniform3fv(glGetUniformLocation(shader, "color"), 1, &bc.color[0]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);


	glBindVertexArray(0); 
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glUseProgram(0);
}

void HealthBar::updateBar(float filledFraction) {
	if (this->filledFraction == filledFraction) {
		return;
	}

	this->filledFraction = filledFraction;
	this->fillingTransform = translate(vec3(-0.5 * (1.0 - filledFraction), 0, 0))
		* scale(vec3(filledFraction, 1, 1));
}

void HealthBar::loadTexture(const char* textureFile, uint id)
{
	int width, height, nrComponents;
	unsigned char* tdata;  // texture pixel data

	tdata = stbi_load(textureFile, &width, &height, &nrComponents, 0);
	if (tdata == NULL) return;

	// Create ID for texture
	glGenTextures(1, &textureIDs[id]);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, textureIDs[id]);

	// Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

