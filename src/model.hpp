#ifndef MODEL_H
#define MODEL_H

#include <fstream>
#include <sstream>
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

#include "mesh.hpp"
#include "shader.hpp"

// For converting between ASSIMP and glm
static inline glm::vec3 vec3Convert(const aiVector3D& vector) { return glm::vec3(vector.x, vector.y, vector.z); }
static inline glm::vec2 vec2Convert(const aiVector3D& vector) { return glm::vec2(vector.x, vector.y); }
static inline glm::quat quatConvert(const aiQuaternion& quaternion) { return glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z); }
static inline glm::mat4 mat4Convert(const aiMatrix4x4& matrix) { return glm::transpose(glm::make_mat4(&matrix.a1)); }
static inline glm::mat4 mat4Convert(const aiMatrix3x3& matrix) { return glm::transpose(glm::make_mat3(&matrix.a1)); }

unsigned int TextureFromFile(const char* filename, const std::string& directory, bool gamma = false);

class Model
{
    public:
        Model();
        ~Model();

        void InitFromScene(const aiScene* scene);

        void Draw(Shader shader);

        void SetAnimation(unsigned int animation);
        void SetBoneTransformations(Shader shader, GLfloat currentTime);

        unsigned int BonesCount() const { return bonesCount; }
        bool HasAnimations() { return scene->HasAnimations(); }
        unsigned int GetNumAnimations() { return scene->mNumAnimations; }
        void SetDirectory(std::string directory) { this->directory = directory; }

    private:
        #define NUM_BONES_PER_VERTEX 4

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

        const aiScene* scene;

        std::string directory;
        std::vector<Mesh> meshes;
        // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Texture> loadedTextures;

        glm::mat4 globalInverseTransform;
        GLfloat ticksPerSecond = 0.0f;
        // duration of the animation, can be changed if frames are not present in all interval
        double animDuration;
        unsigned int currentAnimation;

        unsigned int bonesCount = 0;
        std::map<std::string, unsigned int> boneMapping;
        std::vector<BoneMatrix> boneMatrices;

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

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
};
#endif