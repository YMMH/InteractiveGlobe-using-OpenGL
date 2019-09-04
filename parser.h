#pragma once

class GeoJsonParser {

public:
	static int GeoJson2LatLon(const char* filePath, std::vector<float>& lat, std::vector<float>& lon);

};

