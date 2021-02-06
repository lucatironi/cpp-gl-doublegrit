#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

#define INVALID_MATERIAL 0xFFFFFFFF

// For converting between ASSIMP and glm
static inline glm::vec3 vec3Convert(const aiVector3D &vector) { return glm::vec3(vector.x, vector.y, vector.z); }
static inline glm::vec2 vec2Convert(const aiVector3D &vector) { return glm::vec2(vector.x, vector.y); }
static inline glm::quat quatConvert(const aiQuaternion &quaternion) { return glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z); }
static inline glm::mat4 mat4Convert(const aiMatrix4x4 &matrix) { return glm::transpose(glm::make_mat4(&matrix.a1)); }
static inline glm::mat4 mat4Convert(const aiMatrix3x3 &matrix) { return glm::transpose(glm::make_mat3(&matrix.a1)); }

class AnimatedModel
{
    public:
        AnimatedModel();
        ~AnimatedModel();

        void InitFromScene(const aiScene* scene);

        void Draw(Shader shader);

        void SetAnimation(unsigned int animation);
        void SetBoneTransformations(Shader shader, GLfloat deltaTime);
        void BoneTransform(float deltaTime, std::vector<glm::mat4> &transforms);
        unsigned int BonesCount() const { return bonesCount; }
        bool HasAnimations() { return scene->HasAnimations(); }
        unsigned int GetNumAnimations() { return scene->mNumAnimations; }

        void SetDirectory(std::string directory) { this->directory = directory; }
    private:
        #define NUM_BONES_PER_VERTEX 4

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

        struct VertexBoneData
        {
            unsigned int IDs[NUM_BONES_PER_VERTEX];
            float Weights[NUM_BONES_PER_VERTEX];

            VertexBoneData()
            {
                Reset();
            };

            void Reset()
            {
                memset(IDs, 0, sizeof(IDs));
                memset(Weights, 0, sizeof(Weights));
            }

            void AddBoneData(unsigned int boneID, float weight);
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
        GLfloat ticksPerSecond = 0.0f;
        // duration of the animation, can be changed if frames are not present in all interval
        double animDuration;
        unsigned int currentAnimation;

        std::string directory;
        std::vector<Mesh> meshes;
        // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Texture> loadedTextures;

        unsigned int bonesCount = 0;
        std::map<std::string, unsigned int> boneMapping;
        std::vector<BoneMatrix> boneMatrices;

        enum VB_TYPES {
            INDEX_BUFFER,
            POSITION_BUFFER,
            NORMAL_BUFFER,
            TEX_COORDS_BUFFER,
            BONE_BUFFER,
            BUFFERS_COUNT
        };

        GLuint VAO;
        GLuint buffers[BUFFERS_COUNT];

        unsigned int findPosition(float animationTime, const aiNodeAnim* nodeAnim);
        unsigned int findRotation(float animationTime, const aiNodeAnim* nodeAnim);
        unsigned int findScaling(float animationTime, const aiNodeAnim* nodeAnim);

        void calcInterpolatedPosition(aiVector3D &out, float animationTime, const aiNodeAnim* nodeAnim);
        void calcInterpolatedRotation(aiQuaternion &out, float animationTime, const aiNodeAnim* nodeAnim);
        void calcInterpolatedScaling(aiVector3D &out, float animationTime, const aiNodeAnim* nodeAnim);

        void readNodeHeirarchy(float animationTime, const aiNode* node, const glm::mat4 &parentTransform);
        const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string nodeName);

        void processMesh(unsigned int meshIndex,
                         const aiMesh* mesh,
                         std::vector<glm::vec3> &positions,
                         std::vector<glm::vec3> &normals,
                         std::vector<glm::vec2> &texCoords,
                         std::vector<VertexBoneData> &bones,
                         std::vector<unsigned int> &indices);
        void loadBones(unsigned int meshIndex, const aiMesh* mesh, std::vector<VertexBoneData> &bones);
        void loadMaterials(const aiScene* pScene, std::vector<Texture> &textures);
        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
        unsigned int textureFromFile(const char* path, const std::string &directory, bool gamma = false);
        void clear();
};

#endif