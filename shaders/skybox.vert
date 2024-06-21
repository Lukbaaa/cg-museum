#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = aPos;
    gl_Position = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
}  