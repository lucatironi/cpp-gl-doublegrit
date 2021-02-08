#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <stb_image.h>

#include <assimp/scene.h>

#include "utils.hpp"
#include "shader.hpp"

// For converting between ASSIMP and glm
static inline glm::vec3 vec3Convert(const aiVector3D& vector) { return glm::vec3(vector.x, vector.y, vector.z); }
static inline glm::vec2 vec2Convert(const aiVector3D& vector) { return glm::vec2(vector.x, vector.y); }
static inline glm::quat quatConvert(const aiQuaternion& quaternion) { return glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z); }
static inline glm::mat4 mat4Convert(const aiMatrix4x4& matrix) { return glm::transpose(glm::make_mat4(&matrix.a1)); }
static inline glm::mat4 mat4Convert(const aiMatrix3x3& matrix) { return glm::transpose(glm::make_mat3(&matrix.a1)); }

class AnimatedModel
{
    public:
        AnimatedModel();
        ~AnimatedModel() {};

        void InitFromScene(const aiScene* scene);

        void Draw(Shader shader);

        void SetAnimation(unsigned int animation);
        void SetBoneTransformations(Shader shader, GLfloat currentTime);

        unsigned int BonesCount() const { return bonesCount; }
        bool HasAnimations() { return scene->HasAnimations(); }
        unsigned int GetNumAnimations() { return scene->mNumAnimations; }
        void SetDirectory(std::string directory) { this->directory = directory; }

    private:
        #define INVALID_MATERIAL 0xFFFFFFFF
        #define NUM_BONES_PER_VERTEX 4

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

        struct BoneMatrix
        {
            glm::mat4 BoneOffset;
            glm::mat4 FinalTransformation;

            BoneMatrix()
            {
                BoneOffset = glm::mat4(0.0f);
                FinalTransformation = glm::mat4(0.0f);
            }
        };

        struct Mesh {
            Mesh()
            {
                IndicesCount  = 0;
                BaseVertex    = 0;
                BaseIndex     = 0;
                MaterialIndex = INVALID_MATERIAL;
            }

            unsigned int BaseVertex;
            unsigned int BaseIndex;
            unsigned int IndicesCount;
            unsigned int MaterialIndex;
        };

        const aiScene* scene;
        glm::mat4 globalInverseTransform;
        // duration of the animation, can be changed if frames are not present in all interval
        double animDuration;
        unsigned int currentAnimation;

        std::string directory;
        std::vector<Mesh> meshes;
        std::vector<Texture> textures;
        // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Texture> loadedTextures;

        unsigned int bonesCount = 0;
        std::map<std::string, unsigned int> boneMapping;
        std::vector<BoneMatrix> boneMatrices;

        GLuint VAO, VBO, EBO;

        void processMesh(unsigned int meshIndex,
                         const aiMesh* mesh,
                         std::vector<Vertex>& vertices,
                         std::vector<unsigned int>& indices,
                         std::vector<Texture>& textures);
        void boneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms);

        unsigned int findPosition(float animationTime, const aiNodeAnim* nodeAnim);
        unsigned int findRotation(float animationTime, const aiNodeAnim* nodeAnim);
        unsigned int findScaling(float animationTime, const aiNodeAnim* nodeAnim);

        void calcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
        void calcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
        void calcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);

        void readNodeHeirarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform);
        const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string nodeName);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
        unsigned int textureFromFile(const char* filename, const std::string& directory, bool gamma = false);
};

#endif