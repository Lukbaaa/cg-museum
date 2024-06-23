#version 330 core

in vec2 texCoord;
in float y;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
    float color = y*0.5;
    if (color < 0) {
        color = 0;
    }

    FragColor = texture(tex, texCoord*10);
}