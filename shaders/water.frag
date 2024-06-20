#version 330 core

in vec2 texCoord;
in float y;

uniform sampler2D tex;

void main() {
    float color = y*0.5;
    if (color < 0) {
        color = 0;
    }

    gl_FragColor = texture(tex, texCoord*10);
}