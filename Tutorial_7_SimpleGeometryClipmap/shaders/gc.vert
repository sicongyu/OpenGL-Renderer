#version 410 core
layout (location = 0) in vec3 position;

uniform sampler2D heightmap;

uniform mat4 projectionView;
//uniform mat4 view;

uniform float scale;
uniform vec3 offset;
uniform vec3 cameraPos;

const float chunkRealSize = 5120;
const float chunkNumber = 32;
const float maxHeight = 8000;
const float range = chunkRealSize * chunkNumber;

out vec2 texcoord;

void main() {
	vec4 pos = vec4(position * scale + offset, 1);
	pos.xz += cameraPos.xz;
	texcoord = pos.xz / range;
	pos.y = texture(heightmap, texcoord).r * maxHeight;
//	pos.y = 0;
	gl_Position = projectionView * pos;
}