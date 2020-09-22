#pragma once
#include "Predefine.h"
#include "TerrainInfo.h"

class Border {
public:
	void Init();
	void Draw(int level1, int level2, int k);

private:
	GLuint bVAO[TerrainInfo::levelOfTerrain][2][4];
	GLuint bVBO[TerrainInfo::levelOfTerrain][2][4];
	std::vector<GLfloat> bVertices;
	int sizes[TerrainInfo::levelOfTerrain][2];

	void BuildTwoSideEqualBorder(int level, int dir, int xzratio, int signbitX, int complementX, int signbitZ, int complement);
	void Configure(int level1, int level2, int dir);
	vec3 RotatePoint(float x, float y, float z, int xzratio, int signbitX, int complementX, int signbitZ, int complementZ);
};
