#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/matrix4x4.h>

#include <string>
#include <vector>

#include "shader.hpp"

const unsigned int NUM_BONES_PER_VERTEX = 4;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::ivec4 BoneIDs;
    glm::vec4 BoneWeights;
};

struct Texture
{
    unsigned int ID;
    std::string Type;
    std::string Path;
};

class Mesh
{
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        ~Mesh();

        void Draw(Shader shader);

    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        unsigned int VAO, VBO, EBO;

        // initializes all the buffer objects/arrays
        void setupMesh();
};
#endif