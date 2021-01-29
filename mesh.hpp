#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/matrix4x4.h>

#include "shader.hpp"

const unsigned int NUM_BONES_PER_VERTEX = 4;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

struct BoneMatrix
{
    aiMatrix4x4 offsetMatrix;
    aiMatrix4x4 finalWorldTransform;
};

struct VertexBoneData
{
    unsigned int ids[NUM_BONES_PER_VERTEX];
    float weights[NUM_BONES_PER_VERTEX];

    VertexBoneData()
    {
        // init all values in array = 0
        memset(ids, 0, sizeof(ids));
        memset(weights, 0, sizeof(weights));
    }

    void addBoneData(unsigned int boneID, float weight);
};

class Mesh
{
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::vector<VertexBoneData> bones);
        ~Mesh();

        void Draw(Shader shader);

    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        std::vector<VertexBoneData> bones;
        unsigned int VAO, VBO, EBO;

        // initializes all the buffer objects/arrays
        void setupMesh();
};
#endif