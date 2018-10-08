#version 330 core
in vec3 VertexLight;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;

void main()
{
    vec4 tex = texture(image, TexCoords);
    color = tex * vec4(VertexLight, 1.0);
    color.rgb *= smoothstep(12.0, 1.0, TexCoords.x / TexCoords.y);
    color.rgb = floor(color.rgb * 6.35) / 6.35;
}