#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <map>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"
#include "shader.hpp"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
    public:
        Model() {};

        void Init(const aiScene *scene);

        // draws the model, and thus all its meshes
        void Draw(Shader shader);

        void SetDirectory(std::string directory) { this->directory = directory; }

    private:
        aiMatrix4x4 globalInverseTransformMatrix;
        GLfloat ticksPerSecond = 0.0f;

        std::string directory;
        std::vector<Mesh> meshes;
        // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Texture> loadedTextures;

        unsigned int bonesCount = 0;
        std::map<std::string, unsigned int> boneMapping;
        std::vector<BoneMatrix> boneMatrices;

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
#endif