#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec2 textCoords;

out vec4 fragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// Spot light w/ soft edge
struct Light {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

uniform Material material; 
uniform Light light;
uniform vec3 viewPos;

void main() {
    vec3 textDiffuse = vec3(texture(material.diffuse, textCoords));
    vec3 textSpecular = vec3(texture(material.specular, textCoords));
    vec3 ambient = light.ambient * textDiffuse;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * textDiffuse * light.diffuse;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * textSpecular * light.specular;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 phong = (ambient + (diffuse * intensity) + (specular * intensity)) * attenuation;
    fragColor = vec4(phong, 1.0);
} 