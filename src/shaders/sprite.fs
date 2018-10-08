#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;

void main()
{
    color = floor(texture(image, TexCoords) * 16.0) / 16.0;
}