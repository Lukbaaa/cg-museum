#version 330 core

in vec2 texCoord;

uniform sampler2D tex0;
out vec4 FragColor;
void main() {
    FragColor = texture(tex0, texCoord);
}