#version 330 core

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
out vec4 FragColor;
void main() {
    FragColor = ambient + diffuse + specular;
}