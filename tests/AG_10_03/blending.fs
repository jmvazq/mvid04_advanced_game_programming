#version 330 core

in vec2 textCoords;

out vec4 fragColor;

uniform sampler2D texture;

void main() {
    vec4 textColor = texture(texture, textCoords);
    fragColor = textColor;
}  