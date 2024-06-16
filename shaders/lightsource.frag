#version 330 core

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;

void main() {
    gl_FragColor = ambient + diffuse + specular;
}