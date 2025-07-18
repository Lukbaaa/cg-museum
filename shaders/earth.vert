#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;

void main() {
   fragPos = vec3(model * vec4(aPosition, 1.0));
   normal = mat3(normalMatrix) * aNormal;
   texCoord = aTexCoord;
   gl_Position = projection*view*model*vec4(aPosition, 1.0);
}