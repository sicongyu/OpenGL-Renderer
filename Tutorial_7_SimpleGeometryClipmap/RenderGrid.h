#pragma once
#include "Predefine.h"
#include "TerrainInfo.h"

class RenderGrid {
public:
	void Init();
	void Draw(int level);

private:	
	GLuint vao[TerrainInfo::levelOfTerrain], vbo[TerrainInfo::levelOfTerrain];
	GLuint size[TerrainInfo::levelOfTerrain];

	void BuildGrid(int grid, std::vector<GLfloat> &vertices);
};