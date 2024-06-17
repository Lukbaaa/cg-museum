#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main() {    
    gl_FragColor = texture(skybox, texCoords);
}