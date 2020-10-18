
#version 410 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
const float exposure = 0.5;

void main() {

    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result  = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}