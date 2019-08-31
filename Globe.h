#pragma once
#include <vector>
#define PI 3.141592

class Globe {
private:
	float radius_;
	int dimension_;

	std::vector<float> vertices_;
	std::vector<int> indices_;

public:
	int indiceCnt_;

	Globe(float radius, int dimension);
	~Globe();

	std::vector<float>& GetVertexVector();
	//std::vector<float> GetIndiceVector();

	void CreateGlobe();//(std::vector<float>& vertices, std::vector<int>& indices, int& indiceCnt);
	void BindGlobeBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO);

};
