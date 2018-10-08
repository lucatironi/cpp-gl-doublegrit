#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 VertexLight;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    float linear = 0.9;
    float quadratic = 0.032;

    // ambient
    vec3 ambient = vec3(0.3, 0.3, 0.6);
    ambient += lightColor;

    // diffuse
    vec3 norm = normalize(aNormal);
    vec3 lightDir = normalize(lightPos - aPos);
    float diffuse = max(dot(norm, lightDir), 0.0);

    // attenuation
    float distance = length(lightPos - aPos);
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * (distance * distance));

    // combine
    ambient *= attenuation;
    diffuse *= attenuation;

    VertexLight = ambient + diffuse;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(aPos, 1.0);
}