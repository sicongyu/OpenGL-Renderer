#include "Terrain.h"
#include "Utils.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include <stb_image.h>

using namespace std;

float quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};

void Terrain::Init() {
	if (initFlag) return;
	initFlag = true;

	rg.Init();
	bd.Init();
	LoadTextures();

	InitBlocks();

	programID = LoadShaders("shaders/gc.vert", "shaders/gc.frag");
}

void Terrain::LoadTextures() {
	int size = paths.size();
	texs.resize(size);

	glGenTextures(size, &texs[0]);

	int w, h, n;
	uchar *data;
	ushort *sdata;
	for (int i = 0; i < size; i++) {
		string path = "../models/terrain/" + paths[i];
		if (!i)
			sdata = stbi_load_16(path.c_str(), &w, &h, &n, 0);
		else
			data = stbi_load(path.c_str(), &w, &h, &n, 0);

		GLuint format, type, internalFormat;
		type = i ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
		switch (n) {
		case 1:
			format = GL_RED;
			internalFormat = i ? GL_R8 : GL_R16;
			break;
		case 3:
			internalFormat = format = GL_RGB;
			break;
		case 4:
			internalFormat = format = GL_RGBA;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, texs[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, i ? (void*)data : (void*)sdata);

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(i ? (void*)data : (void*)sdata);
	}
}

void Terrain::InitBlocks() {
	int n = TerrainInfo::chunkNumber;
	int m = n << 1;
	int s = TerrainInfo::chunkRealSize;
	blocks = new Block[m * m];

	for (int i = 0; i < m; i++)
		for (int j = 0; j < m; j++) {
			int x, y;
			x = j - n;
			y = i - n;
			blocks[i * m + j].offset = vec3(x * s, 0, y * s);
			x = x >= 0 ? x + 1 : -x;
			y = y >= 0 ? y + 1 : -y;
			blocks[i * m + j].level = glm::clamp((int)floor(log2((float)std::max(x, y))), 0, TerrainInfo::levelOfTerrain - 1);
		}
}

void Terrain::Update(Camera *camera) {
	this->camera = camera;
	FrustumCulling();
}

void Terrain::FrustumCulling() {
	int m = TerrainInfo::chunkNumber << 1;
	int s = TerrainInfo::chunkRealSize;
	int mh = TerrainInfo::maxHeight;
	mat4 pv = camera->projectViewMat;
	int xs, xb, ys, yb, zs, zb;
	for (int y = 0; y < m; y++) 
		for (int x = 0; x < m; x++) {
			xs = xb = ys = yb = zs = zb = 0;
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						vec3 off = blocks[y * m + x].offset;
						vec4 pos = vec4(off.x + i * s, k * mh, off.z + j * s, 1);
						vec3 cp = camera->GetPos();
						pos.x += cp.x;
						pos.z += cp.z;
						pos = pv * pos;
						xs = (pos.x < -pos.w) ? xs + 1 : xs;
						xb = (pos.x > pos.w) ? xb + 1 : xb;
						ys = (pos.y < -pos.w) ? ys + 1 : ys;
						yb = (pos.y > pos.w) ? yb + 1 : yb;
						zs = (pos.z < -pos.w) ? zs + 1 : zs;
						zb = (pos.z > pos.w) ? zb + 1 : zb;
					}
			
			blocks[y * m + x].inFrustum = !(xs == 8 || xb == 8 || ys == 8 || yb == 8 || zs == 8 || zb == 8);
		}
}

void Terrain::Draw() {
	if (!initFlag) {
		printf("Please initialize this class firstly\n");
		return;
	}

	glUseProgram(programID);

	for (int i = 0; i < texs.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texs[i]);
		SetUniformInt(programID, shaderVarName[i], i);
	}

	SetUniformMat4f(programID, "projectionView", camera->projectViewMat);
	//SetUniformMat4f(programID, "view", camera->viewMat);
	SetUniformFloat(programID, "scale", TerrainInfo::chunkRealSize);
	SetUniformVec3f(programID, "cameraPos", camera->GetPos());

	static const int dx[4] = { 0, -1, 0, 1 };
	static const int dy[4] = { -1, 0, 1, 0 };

	int m = TerrainInfo::chunkNumber << 1;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < m; j++) {
			if (!blocks[i * m + j].inFrustum) continue;
			SetUniformVec3f(programID, "offset", blocks[i * m + j].offset);
			int level1 = blocks[i * m + j].level;
			//SetUniformFloat(programID, "lod", level1 / (TerrainInfo::levelOfTerrain - 1.f));
			rg.Draw(level1);

			for (int k = 0; k < 4; k++) {
				int di, dj;
				di = i + dy[k];
				dj = j + dx[k];
				int level2;
				if (di < 0 || dj < 0 || di >= m || dj >= m) {
					level2 = level1;
				}
				else {
					level2 = blocks[di * m + dj].level;
				}
				level2 = level1  - level2;
				level2 = level2 < 0 ? 0 : level2;
				bd.Draw(level1, level2, k);
			}
		}
	glUseProgram(0);
}