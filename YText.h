#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H  
#include <map>
#include "Shader.h"

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class YText {
public:
	GLuint VAO_;
	GLuint VBO_;

	GLuint VAO2_;
	GLuint VBO2_;

	std::map<FT_ULong, Character> Characters;

	YText();
	~YText();

	void PrepareCharacters();
	void BindBuffer();
	void BindBufferC();
	void RenderText(Shader& s, std::wstring text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void RenderCountryText(Shader& s, std::wstring text, GLfloat x, GLfloat y, GLfloat z, GLfloat scale, glm::vec3 color);
};
