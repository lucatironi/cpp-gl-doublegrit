#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

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

#include "shader.hpp"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

class AnimatedModel
{
    public:
        AnimatedModel();

        ~AnimatedModel();

        bool LoadMesh(const std::string &filename);

        void Draw(Shader shader);

        unsigned int BonesCount() const
        {
            return bonesCount;
        }

        void BoneTransform(GLfloat timeInSeconds, std::vector<aiMatrix4x4> &transforms);

    private:
        #define NUM_BONES_PER_VERTEX 4

        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
        };

        struct Texture
        {
            unsigned int ID;
            std::string Type;
            std::string Path;
        };

        struct BoneMatrix
        {
            aiMatrix4x4 BoneOffset;
            aiMatrix4x4 FinalTransformation;
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

        void calcInterpolatedScaling(aiVector3D &out, float animationTime, const aiNodeAnim *nodeAnim);
        void calcInterpolatedRotation(aiQuaternion &out, float animationTime, const aiNodeAnim *nodeAnim);
        void calcInterpolatedPosition(aiVector3D &out, float animationTime, const aiNodeAnim *nodeAnim);

        unsigned int findScaling(float animationTime, const aiNodeAnim *nodeAnim);
        unsigned int findRotation(float animationTime, const aiNodeAnim *nodeAnim);
        unsigned int findPosition(float animationTime, const aiNodeAnim *nodeAnim);
        const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string nodeName);

        void readNodeHeirarchy(float animationTime, const aiNode *node, const aiMatrix4x4 &parentTransform);

        bool initFromScene(const aiScene *scene, const std::string &filename);
        void initMesh(unsigned int meshIndex,
                    const aiMesh *mesh,
                    std::vector<Vertex> &vertices,
                    std::vector<unsigned int> &indices,
                    std::vector<Texture> &textures,
                    std::vector<VertexBoneData> &bones);
        void loadBones(unsigned int meshIndex, const aiMesh * mesh, std::vector<VertexBoneData> &bones);
        bool initMaterials(const aiScene* scene, const  std::string &filename);
        void clear();

#define INVALID_MATERIAL 0xFFFFFFFF

enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    BONE_VB,
    NUM_VBs
};

        GLuint VAO;
        GLuint buffers[NUM_VBs];

        struct Mesh {
            Mesh()
            {
                IndicesCount  = 0;
                BaseVertex    = 0;
                BaseIndex     = 0;
                MaterialIndex = INVALID_MATERIAL;
            }

            unsigned int IndicesCount;
            unsigned int BaseVertex;
            unsigned int BaseIndex;
            unsigned int MaterialIndex;
        };

        std::vector<Mesh> meshes;
        std::vector<Texture*> textures;

        std::map<std::string, unsigned int> boneMapping; // maps a bone name to its index
        unsigned int bonesCount;
        std::vector<BoneMatrix> boneMatrices;
        aiMatrix4x4 globalInverseTransform;

        const aiScene *scene;
};

#endif