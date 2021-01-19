#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 VertexLight;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform bool entity;

void main()
{
    // ambient
    vec3 ambient = vec3(0.3, 0.3, 0.6);

    // diffuse
    vec3 norm = normalize(aNormal);
    vec3 lightDir = normalize(lightPos - aPos);
    float diffuse = max(dot(aNormal, lightDir), 0.0);

    // attenuation
    float distance = length(lightPos - aPos);
    float attenuation = 1.0 / (0.04 * distance);

    // combine
    VertexLight = ambient + lightColor * diffuse * attenuation;
    TexCoords = aTexCoords;

    if (entity)
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    else
    {
        gl_Position = projection * view * vec4(aPos, 1.0);
    }
}
