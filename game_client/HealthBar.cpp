#include "HealthBar.h"

using namespace glm;
using namespace std;

/* static variables */
GLuint HealthBar::VBO, HealthBar::VBO2, HealthBar::VAO, HealthBar::EBO;
uint HealthBar::shader;
uint HealthBar::numHealthBar = 0;

HealthBar::HealthBar(uint shader, const char* iconFile, float initFilledFraction, glm::vec3 barColor)
{
	this->shader = shader;
	this->modelMtx = scale(vec3(1.0f));

	// Init the filling status
	resetBar(initFilledFraction);

	// Compute the transforms
	float iconWidth = 1.0f;
	float gap = 0.25f;
	float barWidth = 4.0f;
	float barHeight = 0.5f;
	float barEdgePad = barHeight * 0.1f;

	mat4 iconMtx = translate(vec3(-(barWidth + gap) / 2.0f, 0, 0))
		* scale(vec3(iconWidth, iconWidth, 1));
	mat4 barBoxMtx = translate(vec3((iconWidth + gap) / 2.0f, 0, 0)) * scale(vec3(barWidth, barHeight, 1));
	mat4 barMtx = barBoxMtx
		* scale(vec3((barWidth - 2.0f * barEdgePad) / barWidth, (barHeight - 2.0f * barEdgePad) / barHeight, 1)); // shrink a little from the box

	vec3 whiteColor = vec3(1,1,1);

	// Init each bar components
	barComponents.resize(3);
	barComponents[ICON] = BarComponent(ICON, iconMtx, whiteColor);
	barComponents[BAR_BOX] = BarComponent(BAR_BOX, barBoxMtx, whiteColor);
	barComponents[BAR] = BarComponent(BAR, barMtx, barColor);

	// Init (static) rendering related fields 
	if (numHealthBar == 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &VBO2);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Texture loading and binding
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

	numHealthBar++;
}

HealthBar::~HealthBar()
{
	numHealthBar--;

	if (numHealthBar == 0) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &VBO2);
		glDeleteBuffers(1, &EBO);
	}
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
	if (this->currFilledFraction == filledFraction) {
		return;
	}

	if (filledFraction > this->currFilledFraction) { // increasing bar
		this->currFilledFraction = std::min(filledFraction, this->currFilledFraction + fillingStep);
	}
	else { // decreasing bar
		this->currFilledFraction = std::max(filledFraction, this->currFilledFraction - fillingStep);
	}

	this->filledFraction = filledFraction;
	this->fillingTransform = translate(vec3(-0.5 * (1.0 - this->currFilledFraction), 0, 0))
		* scale(vec3(this->currFilledFraction, 1, 1));
}

void HealthBar::resetBar(float defaultFilledFraction) {
	this->currFilledFraction = defaultFilledFraction;
	this->filledFraction = defaultFilledFraction;
	this->fillingTransform = translate(vec3(-0.5 * (1.0 - this->currFilledFraction), 0, 0))
		* scale(vec3(this->currFilledFraction, 1, 1));
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

