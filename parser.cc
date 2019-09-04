#include <iostream>
#include <stdio.h>
#include <vector>
#include "geojson.hh"
#include "parser.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int GeoJsonParser::GeoJson2LatLon(const char *filePath, std::vector<float> &lat, std::vector<float> &lon)
{
  if (filePath == NULL)
  {
    std::cout << "ERROR in GeoJson2LatLon()::no argu" << std::endl;
    return 1;
  }

  geojson_t geojson;
  if (geojson.convert(filePath) < 0)
  {
    return 1;
  }

  std::cout << std::endl;

  ///////////////////////////////////////////////////////////////////////////////////////
  //render geojson
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t size_features = geojson.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = geojson.m_feature.at(idx_fet);

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

        //std::vector<double> lat;
        //std::vector<double> lon;

        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
        {
          lat.push_back(polygon.m_coord[idx_crd].y);
          lon.push_back(polygon.m_coord[idx_crd].x);
        }

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


  return 0;
}
