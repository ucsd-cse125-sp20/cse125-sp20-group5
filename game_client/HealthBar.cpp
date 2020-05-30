#include "HealthBar.h"

using namespace glm;
using namespace std;

/* static variables */
GLuint HealthBar::VBO, HealthBar::VBO2, HealthBar::VAO, HealthBar::EBO;
uint HealthBar::shader;
bool HealthBar::staticInitialized = false;
bool HealthBar::isDrawUiMode = false;
mat4 HealthBar::iconMtx;
mat4 HealthBar::barBoxMtx;
mat4 HealthBar::barMtx;

HealthBar::HealthBar(uint shader, const char* iconFile, float translateY, float initFilledFraction, glm::vec3 barColor)
{
	this->shader = shader;
	this->modelMtx = translate(vec3(0, translateY, 0)) * scale(vec3(0.3f));

	// Init the filling status
	resetBar(initFilledFraction);

	// Init (static) barComponent matrices and rendering related fields 
	if (!staticInitialized) {
		// Compute the transforms
		float iconWidth = 0.8f;
		float gap = 0.15f;
		float barWidth = 4.0f;
		float barHeight = BAR_HEIGHT; // = 0.5f
		float barEdgePad = barHeight * 0.125f;

		// Assign barComp matrices
		iconMtx = translate(vec3(-(barWidth + gap) / 2.0f, 0, 0))
			* scale(vec3(iconWidth, iconWidth, 1));
		barBoxMtx = translate(vec3((iconWidth + gap) / 2.0f, 0, 0)) * scale(vec3(barWidth, barHeight, 1));
		barMtx = translate(vec3(0, 0, 0.005f)) // to make the bar overlap on barbox
			* barBoxMtx
			* scale(vec3((barWidth - 2.0f * barEdgePad) / barWidth, (barHeight - 2.0f * barEdgePad) / barHeight, 1)); // shrink a little from the box

		// Rendering related fields
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

		staticInitialized = true;
	}

	// Init each bar components
	vec3 whiteColor = vec3(1, 1, 1);
	barComponents.resize(3);
	barComponents[ICON] = BarComponent(ICON, iconMtx, barColor);
	barComponents[BAR_BOX] = BarComponent(BAR_BOX, barBoxMtx, whiteColor);
	barComponents[BAR] = BarComponent(BAR, barMtx, barColor);

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
}

HealthBar::~HealthBar()
{
}

void HealthBar::draw(SceneNode& node, const glm::mat4& viewProjMtx)
{
	if (!HealthBar::isDrawUiMode) { return; } // isDrawUiMode is toggled when drawing all UI at the end of render pass for the sake of alpha blending

	if (this->alphaEffectOn) { 
		if (this->shouldDisplay) {
			alphaValue = std::min(HealthBar::MAX_ALPHA, alphaValue + alphaStep);
		}
		else {
			if (alphaValue == 0.0f) { return; } // do not display
			alphaValue = std::max(0.0f, alphaValue - alphaStep);
		}
	}
	else if (!this->shouldDisplay) { return; } // do not display



	glUseProgram(shader);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // to allow transparent
	glBindVertexArray(VAO);


	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&viewProjMtx);
	glUniform1f(glGetUniformLocation(shader, "alpha"), alphaValue);

	for (BarComponent bc : barComponents) {
		glBindTexture(GL_TEXTURE_2D, textureIDs[bc.id]);

		mat4 model = translate(vec3(node.transform[3])) // only apply parent translate
			* modelMtx 	* bc.localMtx; // apply local transform
		if (bc.id == BAR) {
			model = model * this->fillingTransform;
		}

		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&model);
		glUniform3fv(glGetUniformLocation(shader, "color"), 1, &bc.color[0]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);


	glBindVertexArray(0); 
	glDisable(GL_BLEND);
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

	tdata = stbi_load(textureFile, &width, &height, &nrComponents, STBI_rgb_alpha);
	if (tdata == NULL) return;

	// Create ID for texture
	glGenTextures(1, &textureIDs[id]);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, textureIDs[id]);

	// Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void HealthBar::update(SceneNode* node) {
	// Animate the decrease of the bar if filledFraction got updated before
	updateBar(this->filledFraction);
}

SceneNode* HealthBar::createSceneNodes(uint objectId) {
	return new SceneNode(this, std::string("healthBar"), objectId);
}