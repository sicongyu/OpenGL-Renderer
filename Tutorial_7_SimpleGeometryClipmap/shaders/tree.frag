#version 410 core

uniform sampler2D diffuse0;
uniform sampler2D normal0;
uniform sampler2D opacity0;

in vec2 uv;

out vec4 FragColor;

void main() {
	vec4 opacityMask = texture(opacity0, uv);
	opacityMask.r = pow(opacityMask.r, 2.2);
	opacityMask.g = pow(opacityMask.g, 2.2);
	if (opacityMask.r < 0.3)
	    discard;

	vec3 baseColor = texture(diffuse0, uv).rgb;

	FragColor = vec4(baseColor, 1);
}
