#version 330 core
in vec3 VertexLight;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D image;
uniform bool freeCam;
uniform bool retro;

void main()
{
    vec4 tex = texture(image, TexCoords);
    FragColor = tex * vec4(VertexLight, 1.0);

    if (!freeCam)
    {
        float distance = gl_FragCoord.z / gl_FragCoord.w;
        FragColor.rgb *= smoothstep(5.0, 0.1, distance);
    }
    if (retro)
    {
        FragColor.rgb = floor(FragColor.rgb * 6.35) / 6.35;
    }
}
