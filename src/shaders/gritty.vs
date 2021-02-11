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

const int MAX_LIGHTS = 32;
const int MAX_BONES = 100;

uniform bool entity;
uniform bool animated;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 playerLightPos;
uniform vec3 lightColor;
uniform Light lights[MAX_LIGHTS];
uniform mat4 gBones[MAX_BONES];

out vec3 VertexLight;
out vec2 TexCoords;

vec3 CalcPointLight(vec3 lightPos, vec3 vertexPos, vec3 lightColor);

void main()
{
    vec4 worldPos;
    if (entity)
    {
        if (animated)
        {
            mat4 BoneTransform  = gBones[aBoneIDs[0]] * aWeights[0];
                 BoneTransform += gBones[aBoneIDs[1]] * aWeights[1];
                 BoneTransform += gBones[aBoneIDs[2]] * aWeights[2];
                 BoneTransform += gBones[aBoneIDs[3]] * aWeights[3];

            vec4 tPos = BoneTransform * vec4(aPos, 1.0);
            worldPos = model * tPos;
        }
        else
            worldPos = model * vec4(aPos, 1.0);
    }
    else
        worldPos = vec4(aPos, 1.0);

    gl_Position = projection * view * worldPos;

    // ambient
    vec3 ambient = vec3(0.0001);

    VertexLight = ambient + CalcPointLight(playerLightPos, worldPos.xyz, lightColor);

    for (int i = 0; i < lights.length(); i++)
    {
        VertexLight += CalcPointLight(lights[i].position, worldPos.xyz, lights[i].color);
    }

    TexCoords = aTexCoords;
}


vec3 CalcPointLight(vec3 lightPos, vec3 vertexPos, vec3 lightColor)
{
    // attenuation
    float constantAtt = 0.0;
    float linearAtt = 0.7;
    float quadraticAtt = 0.3;

    vec3 normal = normalize(aNormal);
    vec3 lightDir = normalize(lightPos - vertexPos);
    float diffuse = max(dot(normal, lightDir), 0.0);
    float distance = length(lightPos -  vertexPos);
    float attenuation = 1.0 / (constantAtt + linearAtt * distance + quadraticAtt * (distance * distance));
    return (lightColor * attenuation) + (diffuse * attenuation);
}