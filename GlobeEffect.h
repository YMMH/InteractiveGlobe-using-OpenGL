#pragma once
#include "glad/glad.h"
#include "geojson.hh"
#include <vector>
#define PI 3.141592
#define COUNTRY1 0x00
#define COUNTRYALL 0x01

typedef struct VertexObject {
	std::vector<float> vertices;
	std::vector<int> indices;
	int indiceCnt;
}VertexObject;

class GlobeEffect {
private:
	float radius_;
	int dimension_;


public:
	geojson_t geojson_;

	std::vector<VertexObject> ObjectList_;

	GlobeEffect(float radius, int dimension);
	~GlobeEffect();

	int ConvertGeoJson(const char* filePath, int option);
	void MakeAreaObjectList();
	void MakeVertex(std::vector<float>& vertices, std::vector<int>& indices, float* lat, float* lng, const int size);
	void LatLon2Xyz(float lat, float lon, float& x, float& y, float& z);
	void BindAreaBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO, int bufferSize);
	void BindAreaObject(GLuint& VAO, GLuint& VBO, GLuint& EBO, VertexObject vo);
	void GetCentorVector(float* lat, float* lng, const int size, float& x, float& y, float& z);

};
