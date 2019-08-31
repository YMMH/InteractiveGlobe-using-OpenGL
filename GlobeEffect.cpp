#include <glad/glad.h>
#include "GlobeEffect.h"
#include "Type.h"
#include <iostream>

GlobeEffect::GlobeEffect(float radius, int dimension)
{
	radius_ = radius;
	dimension_ = dimension;
	indiceCnt_ = 0;
}

GlobeEffect::~GlobeEffect()
{
}

std::vector<float>& GlobeEffect::GetVertexVector()
{
	return vertices_;
}

void GlobeEffect::CreateRoi(float* lat, float* lng, const int size)
{
	if (indiceCnt_ != 0)
		std::cout << "WARNING::indiceCnt is not zero" << std::endl;

	for (int i = 0; i < size; i++) {
		float rt = lat[i] * PI / 180;
		float rp = lng[i] * PI / 180;

		float x, y, z;
		if (dimension_ == DIMENSION3) {
			x = cos(rt) * cos(rp);
			y = cos(rt) * sin(rp);
			z = sin(rt);
			vertices_.push_back(x * radius_);
			vertices_.push_back(y * radius_);
			vertices_.push_back(z * radius_);
		}
		else {
			x = rt / PI;
			y = rp / (PI * 2);
			z = -0.002;
			vertices_.push_back(x);
			vertices_.push_back(2*y);
			vertices_.push_back(z);
		}
	}

	for (int i = 0; i+2 < size; i++) {

		indices_.push_back(0);
		indices_.push_back(i+1);
		indices_.push_back(i+2);
		indiceCnt_++;
	}
}

void GlobeEffect::BindRoiBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices_.size(), &vertices_[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices_.size(), &indices_[0], GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//Clean
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
