#version 410 core

uniform sampler2D diffuse;
uniform float lod;

in vec2 texcoord;

out vec4 fragColor;

void main() {
	if (texcoord.x <= 0.01 || texcoord.x >= 0.99 || texcoord.y <= 0.01 || texcoord.y >= 0.99 )
   		discard;
	fragColor = texture(diffuse, texcoord);
}