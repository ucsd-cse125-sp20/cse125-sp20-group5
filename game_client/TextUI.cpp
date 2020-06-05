#include "TextUI.h"
#include "Client.h"

using namespace glm;
using namespace std;

GLuint TextUI::VBO, TextUI::VAO;
std::map<FontType, std::map<GLchar, Character >> TextUI::fontCharacters;
bool TextUI::staticInitialized = false;

std::map<FontType, const char*> TextUI::fontFileMap = {
	   { FontType::CARTOON, "font/From Cartoon Blocks.ttf" },
	   { FontType::CHUNK, "font/Chunk Five Print.otf" }
};
std::string TextUI::backgroundFiles[NUM_OF_BACKGROUND] = {
	   "texture/think_comic_bubble.png"
};
float TextUI::backgroundVertices[6][4] = {
		   { -1.0f, 1.0f,	0.0f, 0.0f },
		   { -1.0f, -1.0f,	0.0f, 1.0f },
		   { 1.0f, -1.0f,	1.0f, 1.0f },

		   { -1.0f, 1.0f,	0.0f, 0.0f },
		   { 1.0f, -1.0f,	1.0f, 1.0f },
		   { 1.0f, 1.0f,	1.0f, 0.0f }
};


TextUI::TextUI(uint shader, FontType usedFont, glm::vec3 usedColor, std::string reservedText, mat4 model)
{
	this->shader = shader;
	this->model = model;
	this->reservedText = reservedText;
	this->usedFont = usedFont;
	this->usedColor = usedColor;

	if (!staticInitialized) {
		for (auto it = fontFileMap.begin(); it != fontFileMap.end(); it++)
		{
			FontType type = it->first;
			const char* fontFile = it->second;
			fontCharacters[type] = this->initFont(fontFile);
		}

		for (int i = 0; i < NUM_OF_BACKGROUND; i++) {
			loadTexture(backgroundFiles[i].c_str(), &backgroundTextureIDs[i]);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	

	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

std::map<GLchar, Character > TextUI::initFont(const char* fontFile) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, fontFile, 0, &face))
	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	std::map<GLchar, Character >Characters;
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return Characters;
}

TextUI::~TextUI()
{
	// Since the below are static in this class
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
}

// render line of text
// -------------------
void TextUI::renderText(std::string text, float x, float y, float scale, glm::vec3 color, int textAlignment)
{
	glUseProgram(shader);

	glUniform1i(glGetUniformLocation(shader, "renderInWorld"), 0);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Client::getWinX()), 0.0f, static_cast<float>(Client::getWinY()));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (float*)&projection);
	glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(shader, "alpha"), alphaValue);
	
	//renderBackground(COMIC_BUBBLE, 1);
	renderFont(text, x, y, scale, false, textAlignment);

	glUseProgram(0);
}

void TextUI::renderTextInWorld(std::string text, glm::mat4 viewProjMtx, glm::mat4 modelMtx, float scale, glm::vec3 color, bool haveBackground)
{
	glUseProgram(shader);

	glUniform1i(glGetUniformLocation(shader, "renderInWorld"), 1);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&modelMtx);
	glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(shader, "alpha"), alphaValue);

	if (haveBackground) { renderBackground(COMIC_BUBBLE, 1); }
	renderFont(text, 0, 0, scale, haveBackground, CENTER);

	glUseProgram(0);
}

void TextUI::renderFont(std::string text, float x, float y, float scale, bool haveBackground, int textAlignment) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // to allow transparent

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	glUniform1i(glGetUniformLocation(shader, "text"), 0);
	glUniform1i(glGetUniformLocation(shader, "isBackground"), 0);

	// get characters for the chosen font
	std::map<GLchar, Character >Characters = fontCharacters[usedFont];

	// get the width of the text
	float totalWidth = 0;
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		totalWidth += (ch.advance >> 6) * scale;
	}

	// adjust scale
	if (haveBackground && totalWidth >= 1.5f) {
		scale = scale * 1.5f / totalWidth;
		totalWidth = 1.5f;
	}

	// set text alignment
	switch (textAlignment) {
		case LEFT:			x = x; break;
		case CENTER:		x = x - totalWidth / 2.0f; break;
		case RIGHT:			x = x - totalWidth; break;
	}

	// iterate through all characters
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void TextUI::renderBackground(int bgType, float ratioToText) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // to allow transparent

	glActiveTexture(GL_TEXTURE1);
	glBindVertexArray(VAO);

	glUniform1i(glGetUniformLocation(shader, "background"), 1);
	glUniform1i(glGetUniformLocation(shader, "isBackground"), 1);

	glBindTexture(GL_TEXTURE_2D, backgroundTextureIDs[bgType]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(backgroundVertices), backgroundVertices); // be sure to use glBufferSubData and not glBufferData
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// reset
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_BLEND);
}

void TextUI::draw(SceneNode& node, const glm::mat4& viewProjMtx) {
	if (!canDraw()) { return; }

	mat4 modelMtx = translate(vec3(node.transform[3])) * model;
	renderTextInWorld(reservedText, viewProjMtx, modelMtx, 0.005f, usedColor);
}
