#include "Border.h"

void Border::Init() {
	int grid1, grid2, level1, level2, d;
	int xzratio, signbitX, complementX, signbitZ, complementZ;
	for (int dir = 0; dir < 4; dir++) {
		switch (dir) {
		case 0:
			xzratio = 1;
			signbitX = 1;
			complementX = 0;
			signbitZ = 1;
			complementZ = 0;
			break;
		case 1:
			xzratio = 0;
			signbitX = 1;
			complementX = 0;
			signbitZ = -1;
			complementZ = 1;
			break;
		case 2:
			xzratio = 1;
			signbitX = -1;
			complementX = 1;
			signbitZ = -1;
			complementZ = 1;
			break;
		case 3:
			xzratio = 0;
			signbitX = -1;
			complementX = 1;
			signbitZ = 1;
			complementZ = 0;
			break;
		}
		for (int i = 0; i < TerrainInfo::levelOfTerrain; i++)
			for (int j = 0; j < 2; j++) {
				if (i == TerrainInfo::levelOfTerrain - 1 && j == 1) continue;
				level1 = i;
				level2 = j + i;
				grid1 = TerrainInfo::chunkSize >> level1;
				grid2 = TerrainInfo::chunkSize >> level2;
				if (level1 == level2) {
					BuildTwoSideEqualBorder(i, dir, xzratio, signbitX, complementX, signbitZ, complementZ);
					continue;
				}
				d = level2 - level1;
				bVertices.clear();
				int p1, p2;
				vec3 tmp;
				for (int k = 1; k < grid1 + 1; k++) {
					p1 = (k - 1) >> d;
					p1 = (p1 == 0) ? 1 : p1;
					p1 = (p1 >= grid2) ? grid2 - 1 : p1;

					p2 = k >> d;
					p2 = (p2 == 0) ? 1 : p2;
					p2 = (p2 >= grid2) ? grid2 - 1 : p2;

					if (p1 != p2) {
						tmp = RotatePoint(float(k - 1) / grid1, 0, 0, xzratio, signbitX, complementX, signbitZ, complementZ);
						bVertices.push_back(tmp.x);
						bVertices.push_back(tmp.y);
						bVertices.push_back(tmp.z);

						tmp = RotatePoint(float(p1) / grid2, 0, 1.0f / grid2, xzratio, signbitX, complementX, signbitZ, complementZ);
						bVertices.push_back(tmp.x);
						bVertices.push_back(tmp.y);
						bVertices.push_back(tmp.z);

						tmp = RotatePoint(float(p2) / grid2, 0, 1.0f / grid2, xzratio, signbitX, complementX, signbitZ, complementZ);
						bVertices.push_back(tmp.x);
						bVertices.push_back(tmp.y);
						bVertices.push_back(tmp.z);
					}
					tmp = RotatePoint(float(p2) / grid2, 0, 1.0f / grid2, xzratio, signbitX, complementX, signbitZ, complementZ);
					bVertices.push_back(tmp.x);
					bVertices.push_back(tmp.y);
					bVertices.push_back(tmp.z);

					tmp = RotatePoint(float(k) / grid1, 0, 0, xzratio, signbitX, complementX, signbitZ, complementZ);
					bVertices.push_back(tmp.x);
					bVertices.push_back(tmp.y);
					bVertices.push_back(tmp.z);

					tmp = RotatePoint(float(k - 1) / grid1, 0, 0, xzratio, signbitX, complementX, signbitZ, complementZ);
					bVertices.push_back(tmp.x);
					bVertices.push_back(tmp.y);
					bVertices.push_back(tmp.z);
				}

				Configure(level2, 1, dir);
			}
	}
}

void Border::Draw(int level1, int level2, int k) {
	glBindVertexArray(bVAO[level1][level2][k]);
	if (level2 == 0)
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizes[level1][level2] / 3);
	else
		glDrawArrays(GL_TRIANGLES, 0, sizes[level1][level2] / 3);
	glBindVertexArray(0);
}

void Border::BuildTwoSideEqualBorder(int level, int dir, int xzratio, int signbitX, int complementX, int signbitZ, int complementZ) {
	int grid;
	grid = TerrainInfo::chunkSize >> level;
	bVertices.clear();
	vec3 tmp;
	for (int k = 1; k < grid + 1; k++) {
		if (k == 1) {
			tmp = RotatePoint(float(k - 1) / grid, 0, 0, xzratio, signbitX, complementX, signbitZ, complementZ);
			bVertices.push_back(tmp.x);
			bVertices.push_back(tmp.y);
			bVertices.push_back(tmp.z);

			tmp = RotatePoint(float(k) / grid, 0, 1.0f / grid, xzratio, signbitX, complementX, signbitZ, complementZ);
			bVertices.push_back(tmp.x);
			bVertices.push_back(tmp.y);
			bVertices.push_back(tmp.z);

			tmp = RotatePoint(float(k) / grid, 0, 0, xzratio, signbitX, complementX, signbitZ, complementZ);
			bVertices.push_back(tmp.x);
			bVertices.push_back(tmp.y);
			bVertices.push_back(tmp.z);
		}
		else if (k == grid) {
			tmp = RotatePoint(float(k) / grid, 0, 0, xzratio, signbitX, complementX, signbitZ, complementZ);
			bVertices.push_back(tmp.x);
			bVertices.push_back(tmp.y);
			bVertices.push_back(tmp.z);
		}
		else {
			tmp = RotatePoint(float(k) / grid, 0, 1.0f / grid, xzratio, signbitX, complementX, signbitZ, complementZ);
			bVertices.push_back(tmp.x);
			bVertices.push_back(tmp.y);
			bVertices.push_back(tmp.z);

			tmp = RotatePoint(float(k) / grid, 0, 0, xzratio, signbitX, complementX, signbitZ, complementZ);
			bVertices.push_back(tmp.x);
			bVertices.push_back(tmp.y);
			bVertices.push_back(tmp.z);
		}
	}
	Configure(level, 0, dir);
}

void Border::Configure(int level1, int level2, int dir) {
	if (dir == 0) sizes[level1][level2] = bVertices.size();
	glGenVertexArrays(1, &bVAO[level1][level2][dir]);
	glGenBuffers(1, &bVBO[level1][level2][dir]);
	glBindVertexArray(bVAO[level1][level2][dir]);

	glBindBuffer(GL_ARRAY_BUFFER, bVBO[level1][level2][dir]);
	glBufferData(GL_ARRAY_BUFFER, bVertices.size() * sizeof(GLfloat), &bVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

vec3 Border::RotatePoint(float x, float y, float z, int xzratio, int signbitX, int complementX, int signbitZ, int complementZ) {
	vec3 ret;
	ret.x = x * xzratio + z * (1 - xzratio);
	ret.z = x * (1 - xzratio) + z * xzratio;
	ret.x = complementX + signbitX * ret.x;
	ret.z = complementZ + signbitZ * ret.z;
	ret.y = y;
	return ret;
}
