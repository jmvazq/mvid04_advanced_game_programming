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

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define NUMBER_POINT_LIGHTS 2

uniform Material material; 
uniform DirLight dirLight;
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];
uniform vec3 viewPos;

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, textCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, textCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, textCoords));
    
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, textCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, textCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, textCoords));

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 color = calcDirectionalLight(dirLight, norm, viewDir);

    for (int i=0; i < NUMBER_POINT_LIGHTS; ++i)
        color += calcPointLight(pointLights[i], norm, fragPos, viewDir);

    for (int i=0; i < NUMBER_POINT_LIGHTS; ++i)
        color += calcPointLight(pointLights[i], norm, fragPos, viewDir);
    
    fragColor = vec4(color, 1.0);
}