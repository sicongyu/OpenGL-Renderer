#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec2 offset;

uniform sampler2D heightmap;

uniform mat4 projectionView;
uniform mat4 bbdMat;
//uniform mat4 view;

out vec2 uv;

const float chunkRealSize = 5120;
const float chunkNumber = 32;
const float range = chunkNumber * chunkRealSize;
const float maxHeight = 8000;

void main() {
	uv = texcoord;
	uv.y = -uv.y;
	float height = texture(heightmap, offset).r * maxHeight;
	vec4 pos = bbdMat * vec4(position * 100.f, 1);
	pos.xz += offset * range * pos.w;
	pos.y += height - 5.f;
    gl_Position = projectionView * pos;
}  