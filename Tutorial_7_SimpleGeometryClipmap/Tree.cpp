#include "Tree.h"
#include "Utils.h"
using namespace std;

void Tree::Init() {
	programID = LoadShaders("shaders/tree.vert", "shaders/tree.frag");

	offsets.clear();
	for (int i = 0; i < count; i++) {
		float x, y;
		x = y = 0.f;
		while (x < 0.1 || y < 0.1 || x > 0.9 || y > 0.9) {
			x = (float)rand() / RAND_MAX;
			y = (float)rand() / RAND_MAX;
		}
		x *= .5f; y *= .5f;
		float p = (float)rand() / RAND_MAX;
		if (p < .1f) {
		}
		else if (p < .3f) {
			x += .5f;
		}
		else if (p < .5f) {
			y += .5f;
		}
		else {
			x += .5f;
			y += .5f;
		}
		offsets.emplace_back(x);
		offsets.emplace_back(y);
	}
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(float), &offsets[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	LoadModels();
}

void Tree::LoadModels() {
	treeObj = unique_ptr<Mesh>(new Mesh("../models/terrain/lod2.obj", true));
	treeObj->LoadTexture(Diffuse, "../models/terrain/T_PineTree_BillBoard_D.bmp");
	treeObj->LoadTexture(Normal_Map, "../models/terrain/T_PineTree_BillBoard_N.bmp");
	treeObj->LoadTexture(Displace, "../models/terrain/T_PineTree_BillBoard_M.bmp");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeObj->index_id);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, treeObj->pos_id);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, treeObj->uv_id);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, treeObj->normal_id);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, treeObj->binormal_id);
	glVertexAttribPointer(
		3,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, treeObj->tangent_id);
	glVertexAttribPointer(
		4,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(5, 1);
	glBindVertexArray(0);
}

void Tree::Draw(Camera *camera) {
	glUseProgram(programID);
	SetUniformInt(programID, "heightmap", 0);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, treeObj->diffuse_texture_id);
	SetUniformInt(programID, "diffuse0", 6);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, treeObj->normal_texture_id);
	//SetUniformInt(programID, "normal0", 7);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, treeObj->displace_texture_id);
	SetUniformInt(programID, "opacity0", 8);

	SetUniformMat4f(programID, "projectionView", camera->projectViewMat);
	mat4 billboardMatrix = mat4(transpose(mat3(camera->viewMat)));
	billboardMatrix[0][1] = 0;
	billboardMatrix[1][1] = 1;
	billboardMatrix[2][1] = 0;
	SetUniformMat4f(programID, "bbdMat", billboardMatrix);

	glBindVertexArray(vao);
	glDrawElementsInstanced(GL_TRIANGLES, treeObj->face_size * 3, GL_UNSIGNED_SHORT, 0, count);
	glBindVertexArray(0);

	glUseProgram(0);
}