#version 330 core
in vec3 VertexLight;
in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D image;

void main()
{
    vec4 tex = texture(image, TexCoords);
    fragColor = tex * vec4(VertexLight, 1.0);
    float distance = gl_FragCoord.z / gl_FragCoord.w;

    fragColor.rgb *= smoothstep(8.0, 1.0, distance);
    fragColor.rgb = floor(fragColor.rgb * 6.35) / 6.35;
}