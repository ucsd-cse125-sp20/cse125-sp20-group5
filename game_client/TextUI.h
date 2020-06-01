#pragma once
////////////////////////////////////////
// TextUI.h
// 
// Class that renders text to the screen
// https://learnopengl.com/In-Practice/Text-Rendering
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

struct Character {
	unsigned int textureID; // ID handle of the glyph texture
	glm::ivec2   size;      // Size of glyph
	glm::ivec2   bearing;   // Offset from baseline to left/top of glyph
	unsigned int advance;   // Horizontal offset to advance to next glyph
};


class TextUI
{
public:

	TextUI(uint shader, const char* fontFile);
	~TextUI();

	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

	// These variables are needed for the shader program
	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
	uint shader;
private:
};
