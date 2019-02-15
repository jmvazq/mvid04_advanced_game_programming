#version 330 core

in vec3 fragPos;
in vec2 textCoords;

in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

out vec4 fragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};
uniform Material material; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main() {
    vec3 textSpecular = texture(material.specular, textCoords).rgb;
    vec3 color = texture(material.diffuse, textCoords).rgb;
    vec3 normal = texture(material.normal, textCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 ambient = light.ambient * color;

    vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color * light.diffuse;

    vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    vec3 halfwayDir = normalize(-lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * textSpecular * light.specular;
    
    vec3 phong = ambient + diffuse + specular;
    fragColor = vec4(phong, 1.0);
} 