#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

float y;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    texCoord = aTexCoord;
    float xOffset = time + aPosition.x;
    float yOffset = time + aPosition.y;
    float zOffset = time + aPosition.z;
    //vec3 displacement = vec3(0, sin(xOffset)+sin(zOffset)+sin(xOffset*3)+sin(zOffset*3), 0)*0.1;
    vec3 displacement = vec3(sin(xOffset), 1, sin(zOffset+xOffset))*1;

    y = aPosition.y + displacement.y;
    gl_Position = projection*view*model*vec4(aPosition+displacement, 1.0);
}