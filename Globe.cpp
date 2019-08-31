
#include <glad/glad.h>
#include "Globe.h"
#include "Type.h"
#include <iostream>

Globe::Globe(float radius, int dimension)
{
	radius_ = radius;
	dimension_ = dimension;
	indiceCnt_ = 0;
}

Globe::~Globe()
{
}

std::vector<float>& Globe::GetVertexVector()
{
	return vertices_;
}

void Globe::CreateGlobe()//(std::vector<float>& vertices, std::vector<int>& indices, int& indiceCnt)
{
	if (indiceCnt_ != 0)
		std::cout << "WARNING::indiceCnt is not zero" << std::endl;

	int dt = 10;
	int dp = 10;
	for (int t = -90; t <= 90; t+=dt) {//num 19
		for (int p = 0; p <= 360; p+=dp) {//num 37
			float rt = t * PI / 180;
			float rp = p * PI / 180;

			float x, y, z;
			if (dimension_ == DIMENSION3) {
				x = cos(rt) * cos(rp);
				y = cos(rt) * sin(rp);
				z = sin(rt);
				vertices_.push_back(x*radius_);
				vertices_.push_back(y*radius_);
				vertices_.push_back(z*radius_);
			}
			else {
				x = rt / PI;
				y = rp / (PI * 2);
				z = 0.0;
				vertices_.push_back(x);
				vertices_.push_back(2*y);
				vertices_.push_back(z);
			}

			float texS = rp /(2*PI) + 0.5;//0.5~1.5
			float texT = rt / PI + 0.5;//0~1 1~0

			vertices_.push_back(texS);
			vertices_.push_back(texT);
		}
	}

	for (int i = 0; i+1 < 19; i++) {
		for (int j = 0; j+1 < 37; j++) {
			int pos = i * 37 + j;
			int npos = i * 37 + (j + 1);
			int dpos = (i + 1) * 37 + j;
			int dnpos = (i + 1) * 37 + (j + 1);
			
			indices_.push_back(pos);
			indices_.push_back(dpos);
			indices_.push_back(npos);

			indices_.push_back(npos);
			indices_.push_back(dpos);
			indices_.push_back(dnpos);

			indiceCnt_ += 2;
		}
	}
}

void Globe::BindGlobeBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO)
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	// texture coord attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//Clean
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
