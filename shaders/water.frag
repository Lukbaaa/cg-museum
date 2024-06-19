#version 330 core

in vec2 texCoord;
in float y;

uniform sampler2D tex;

void main() {
    float color = y*0.5;
    if (color < 0) {
        color = 0;
    }

    gl_FragColor = texture(tex, texCoord*10);//vec2((int(texCoord.x*1000000))%100)/1000000.0, (int(texCoord.y*1000000)%100)/1000000.0);//vec4(1/255.0+color, 93/255.0+color, 185/255.0+color,1);
}