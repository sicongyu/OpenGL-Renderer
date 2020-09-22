#pragma once
#include "Predefine.h"
#include "Camera.h"

class Tree {
public:
	void Init();
	void Draw(Camera* camere);

private:
	int count = 10000;
	std::unique_ptr<Mesh> treeObj;
	std::vector<float> offsets;
	GLuint vao, instanceVBO;
	GLuint programID;

	void LoadModels();
};