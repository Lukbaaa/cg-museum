// #version 330 core

// in float dt_out;

// out vec4 FragColor;

// void main() {

//     FragColor = vec4(0.0, 1.0, 1.0, dt_out*1.0);
// }
#version 330 core

struct Material {
    vec4 emissive;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct LightSource {
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

in vec3 fragPos;
in vec3 normal;

uniform vec3 viewPos;
uniform Material material;
uniform LightSource light;
uniform float time;

out vec4 FragColor;
void main() {

    // emissive
    vec4 emissive = material.emissive;

    // ambient
    float ambientStrength = 1;
    vec4 ambient = ambientStrength * material.ambient * light.ambient;

    // diffuse
    float diffuseStrength = 1;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position + time - fragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec4 diffuse = diffuseStrength * diff * material.diffuse * light.diffuse;

    // specular
    float specularStrength = 1;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 h = normalize((lightDir + viewDir) / length(lightDir + viewDir));
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = specularStrength * spec * material.specular * light.specular;

    vec4 result = emissive + ambient + diffuse + specular;

    FragColor = result; 
    // FragColor = vec4(1,0,0,1);
}
