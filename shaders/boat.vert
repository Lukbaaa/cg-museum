#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float yDisplacement;

out vec2 texCoord;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    //n(x,z) = (-cos(x), -1, cos(z))
    // (-cos(x), -cos(z), 1);
    // 150, 2*154, 2*192, 
    vec3 displacement = vec3(0 ,yDisplacement, 0);
    
    texCoord = aTexCoord;
    
    gl_Position = projection*view*model*vec4(aPosition+displacement, 1.0);

}
