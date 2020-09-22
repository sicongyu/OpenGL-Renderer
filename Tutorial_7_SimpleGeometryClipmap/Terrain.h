#pragma once
#include "Predefine.h"
#include "TerrainInfo.h"
#include "Border.h"
#include "RenderGrid.h"
#include "Camera.h"

struct Block {
	vec3 offset;
	int level;
	bool inFrustum;
};

class Terrain {
public:
	void Init();
	void Update(Camera *camera);
	void Draw();

private:
	const std::vector<std::string> paths = {
		"Heightmap.png",
		"texture_4k.png"
	};

	const std::vector<std::string> shaderVarName = {
		"heightmap",
		"diffuse"
	};

	bool initFlag = false;

	GLuint programID;
	RenderGrid rg;
	Border bd;
	std::vector<GLuint> texs;
	Block *blocks;
	Camera *camera;

	void LoadTextures();
	void InitBlocks();
	void FrustumCulling();
};