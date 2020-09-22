#include "RenderGrid.h"
using namespace std;

void RenderGrid::Init() {
	vector<GLfloat> vertices;
	int grid = TerrainInfo::chunkSize;
	for (int i = 0; i < TerrainInfo::levelOfTerrain; i++, grid >>= 1) {
		int k = i;
		vertices.clear();
		BuildGrid(grid, vertices);

		glGenVertexArrays(1, &vao[k]);
		glGenBuffers(1, &vbo[k]);
		glBindVertexArray(vao[k]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[k]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
			&vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
			(GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		size[k] = vertices.size() / 3;
	}
}

void RenderGrid::BuildGrid(int grid, vector<GLfloat> &vertices) {
	vertices.clear();

	for (int j = 1; j < grid - 1; j++)
		for (int i = 1; i < grid; i++) {

			for (int k = 0; k < ((i == 1) ? 2 : 1); k++) {
				vertices.emplace_back(float(j) / grid);
				vertices.emplace_back(0);
				vertices.emplace_back(float(i) / grid);
			}

			j++;

			for (int k = 0; k < ((i == grid - 1) ? 2 : 1); k++) {
				vertices.emplace_back(float(j) / grid);
				vertices.emplace_back(0);
				vertices.emplace_back(float(i) / grid);
			}
			j--;
		}
}

void RenderGrid::Draw(int level) {
	glBindVertexArray(vao[level]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, size[level]);
	glBindVertexArray(0);
}
