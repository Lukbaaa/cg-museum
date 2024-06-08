#version 330 core

 struct Material {
    vec4 emissive;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct LightSource {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 viewPos; 
uniform Material material;
uniform LightSource light;

void main() {

    // emissive
    vec4 emissive = material.emissive;

    // ambient
    float ambientStrength = 1;
    vec4 ambient = ambientStrength * material.ambient * light.ambient;

    // diffuse 
    float diffuseStrength = 1;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(vec3(light.position) - fragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec4 diffuse = diffuseStrength * diff * material.diffuse * light.diffuse;

    // specular 
    float specularStrength = 1;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 h = normalize((lightDir + viewDir)/length(lightDir + viewDir));
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = specularStrength * spec * material.specular * light.specular;  

    vec4 result = emissive + ambient + diffuse + specular;

    gl_FragColor = result; //vec4(1,0,0,1);
}