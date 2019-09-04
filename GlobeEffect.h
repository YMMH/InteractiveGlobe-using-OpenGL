#pragma once
#include <vector>
#define PI 3.141592

class GlobeEffect {
private:
	float radius_;
	int dimension_;


public:
	std::vector<float> vertices_;
	std::vector<int> indices_;
	int indiceCnt_;

	GlobeEffect(float radius, int dimension);
	~GlobeEffect();

	std::vector<float>& GetVertexVector();
	//std::vector<float> GetIndiceVector();

	void CreateRoi(float* lat, float* lng, const int size);
	void BindRoiBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO);
	void GetCentorVector(float* lat, float* lng, const int size, float& x, float& y, float& z);
	void PushCentorPos(float x, float y, float z);
};
