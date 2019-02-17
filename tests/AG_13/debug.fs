#version 330 core

out vec4 fragColor;

in vec2 textCoords;

uniform sampler2D depthMap;

void main () {
    float depthValue = texture(depthMap, textCoords).r;

    fragColor = vec4(depthValue, depthValue, depthValue, 1.0);
}