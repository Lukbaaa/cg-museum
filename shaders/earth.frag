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
uniform float time;
uniform Material material;
uniform LightSource light0;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

out vec4 FragColor;

void main() {
    LightSource light = light0;
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
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 h = normalize((lightDir + viewDir)/length(lightDir + viewDir));
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    vec4 specular = specularStrength * spec * material.specular * light.specular;  

    vec4 day = texture(tex0, texCoord);
    vec4 night = texture(tex1, texCoord);
    vec4 ocean = texture(tex2, texCoord);
    vec4 clouds = texture(tex3, texCoord);//vec2(texCoord.x+time/5, texCoord.y)); 

    vec4 day_night_ocean = mix(night, day, diff*diffuseStrength) + ocean*spec*specularStrength;
    FragColor = mix(day_night_ocean, clouds, clouds.a);
}