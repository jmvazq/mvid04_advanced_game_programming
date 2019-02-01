#version 330 core

in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float ambientStrength;
uniform int shininess;
uniform float specularStrength;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 phong = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(phong, 1.0);
}