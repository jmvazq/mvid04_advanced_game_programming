#version 330 core

in vec3 myColor;
in vec2 textCoord;

out vec4 fragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    fragColor = mix(texture(texture1, textCoord), texture(texture2, textCoord), 0.65);
}