#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D tex0;

void main()
{
    vec3 col = texture(tex0, texCoord).rgb;
    FragColor = vec4(col, 1.0);
} 