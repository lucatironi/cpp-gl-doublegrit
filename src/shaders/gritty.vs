#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

struct Light {
    vec3 position;
    vec3 color;
    float attenuation;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 playerPos;
uniform vec3 lightColor;
uniform Light lights[32];

uniform bool entity;
uniform bool animated;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];

out vec3 VertexLight;
out vec2 TexCoords;

void main()
{
    // ambient
    vec3 ambient = vec3(0.4, 0.3, 0.3);

    // diffuse
    vec3 norm = normalize(aNormal);
    vec3 lightDir = normalize(playerPos - aPos);
    float diffuse = max(dot(aNormal, lightDir), 0.0);

    // attenuation
    float distance = length(playerPos - aPos);
    float attenuation = 1.0 / (0.05 * distance);

    // combine
    VertexLight = ambient + lightColor * diffuse * attenuation;

    for (int i = 0; i < lights.length(); i++)
    {
        if (lights[i].position.x == 0.0f)
            break;
        vec3 lightPos = lights[i].position;
        VertexLight += lights[i].color * max(dot(aNormal, normalize(lightPos - aPos)), 0.0) * (1.0 / (lights[i].attenuation * (length(lightPos - aPos))));
    }

    TexCoords = aTexCoords;

    if (entity)
    {
        mat4 BoneTransform  = gBones[aBoneIDs[0]] * aWeights[0];
             BoneTransform += gBones[aBoneIDs[1]] * aWeights[1];
             BoneTransform += gBones[aBoneIDs[2]] * aWeights[2];
             BoneTransform += gBones[aBoneIDs[3]] * aWeights[3];

        vec4 pos = BoneTransform * vec4(aPos, 1.0);
        if (animated)
            gl_Position = projection * view * model * pos;
        else
            gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    else
    {
        gl_Position = projection * view * vec4(aPos, 1.0);
    }
}