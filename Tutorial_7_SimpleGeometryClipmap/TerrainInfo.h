#pragma once

class TerrainInfo {
public:
	static const int chunkSize = 512;
	static const int chunkNumber = 32;
	static const int chunkRealSize = 5120;
	static const int maxHeight = 8000;
	static const int levelOfTerrain = 7;

	static constexpr float heightRatio = 65535.0f / maxHeight;
};
