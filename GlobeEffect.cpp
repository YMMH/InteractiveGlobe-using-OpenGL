#include <glad/glad.h>
#include "geojson.hh"
#include "GlobeEffect.h"
#include "Type.h"
#include <iostream>
#include <fstream>
#include <string>

GlobeEffect::GlobeEffect(float radius, int dimension)
{
	radius_ = radius;
	dimension_ = dimension;
}

GlobeEffect::~GlobeEffect()
{
}

void GlobeEffect::MakeVertex(std::vector<float>& vertices, std::vector<int>& indices, float* lat, float* lng, const int size)
{

	for (int i = 0; i < size; i++) {
		float rt = lat[i] * PI / 180;
		float rp = lng[i] * PI / 180;

		float x, y, z;
		if (dimension_ == DIMENSION3) {
			x = cos(rt) * cos(rp);
			y = cos(rt) * sin(rp);
			z = sin(rt);
			vertices.push_back(x * radius_);
			vertices.push_back(y * radius_);
			vertices.push_back(z * radius_);
		}
		else {
			x = rt / PI;
			y = rp / (PI * 2);
			z = -0.002;
			vertices.push_back(x);
			vertices.push_back(2*y);
			vertices.push_back(z);
		}
	}

	for (int i = 0; i+2 < size; i++) {

		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+2);
	}
}

void GlobeEffect::LatLon2Xyz(float lat, float lon, float& x, float& y, float& z, int dimension)
{
	float rt = lat * PI / 180;
	float rp = lon * PI / 180;

	if (dimension == DIMENSION3) {
		x = cos(rt) * cos(rp);
		y = cos(rt) * sin(rp);
		z = sin(rt);
	}
	else {
		x = rt / PI;
		y = rp / (PI * 2) * 2;
		z = -0.002;
	}
}

void GlobeEffect::BindAreaBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO, int bufferSize)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferSize, NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * bufferSize, NULL, GL_STATIC_DRAW);

	//Clean
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void GlobeEffect::BindAreaObject(GLuint& VAO, GLuint& VBO, GLuint& EBO, VertexObject vo)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*vo.vertices.size(), &vo.vertices[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(float)*vo.indices.size(), &vo.indices[0]);

	//Clean
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw
	//glDrawArrays(GL_LINE_LOOP, 0, vo.vertices.size()/3);
}

void GlobeEffect::GetCentorVector(float* lat, float* lng, const int size, float &x, float &y, float &z)
{
	
	x = 0, y = 0, z = 0;
	for (int i = 0; i < size; i++) {
		float rt = lat[i] * PI / 180;
		float rp = lng[i] * PI / 180;

		x += cos(rt) * cos(rp);
		y += cos(rt) * sin(rp);
		z += sin(rt);
	}

	x = x/size *radius_;
	y = y/size *radius_;
	z = z/size *radius_;
	
}

int GlobeEffect::ConvertGeoJson(const char* filePath, int option)
{
	if (filePath == NULL)
	{
		std::cout << "ERROR in GeoJson2LatLon()::no argu" << std::endl;
		return 1;
	}

	if (option == COUNTRYALL) {
		std::ifstream inputStream("CountryCodeList.txt");

		if (inputStream.is_open()) {
			std::string lineStr;
			while (std::getline(inputStream,lineStr)) {
				std::string pathStr(filePath);
				pathStr.append(lineStr);
				pathStr.append(".geojson");

				if (geojson_.convert(pathStr.c_str()) < 0) {
					return 1;
				}
			}
		}
		inputStream.close();
	}
	else if(option == COUNTRY1) {
		if (geojson_.convert(filePath) < 0) {
			return 1;
		}
	}


	return 0;
}

void GlobeEffect::MakeAreaObjectList()
{

  size_t size_features = geojson_.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = geojson_.m_feature.at(idx_fet);

    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();

      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();

        if (size_crd == 0)
        {
          continue;
        }

		VertexObject vertexObject;
        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
        {
			float lat = polygon.m_coord[idx_crd].y;
			float lon = polygon.m_coord[idx_crd].x;

			float x, y, z;
			LatLon2Xyz(lat, lon, x, y, z, dimension_);
			vertexObject.vertices.push_back(x);
			vertexObject.vertices.push_back(y);
			vertexObject.vertices.push_back(z);
        }

		vertexObject.indiceCnt = 0;
        for (size_t i = 0; i + 2 < size_crd; i++)
        {
			vertexObject.indices.push_back(i);
			vertexObject.indices.push_back(i+1);
			vertexObject.indices.push_back(i+2);
			vertexObject.indiceCnt++;
        }

		ObjectList_.push_back(vertexObject);

        ///////////////////////////////////////////////////////////////////////////////////////
        //render each polygon as a vector of vertices 
        ///////////////////////////////////////////////////////////////////////////////////////

        if (geometry.m_type.compare("Point") == 0)
        {

        }
        else if (geometry.m_type.compare("Polygon") == 0 ||
          geometry.m_type.compare("MultiPolygon") == 0)
        {

        }

      }  //idx_pol
    } //idx_geo
  } //idx_fet
}
