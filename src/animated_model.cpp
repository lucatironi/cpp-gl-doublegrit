#include "animated_model.hpp"

void AnimatedModel::VertexBoneData::AddBoneData(unsigned int boneID, float weight)
{
    for (unsigned int i = 0 ; i < (sizeof(IDs)/sizeof(IDs[0])) ; i++)
    {
        if (Weights[i] == 0.0)
        {
            IDs[i]     = boneID;
            Weights[i] = weight;
            return;
        }
    }
    // should never get here - more bones than we have space for
    assert(0);
}

AnimatedModel::AnimatedModel()
{
    VAO = 0;
    memset(buffers, 0, sizeof(buffers));
    bonesCount = 0;
    scene = nullptr;
}

AnimatedModel::~AnimatedModel()
{
    clear();
}

void AnimatedModel::clear()
{
    for (unsigned int i = 0 ; i < textures.size() ; i++)
    {
        if (textures[i])
        {
            delete textures[i];
            textures[i] = nullptr;
        }
    }

    if (buffers[0] != 0)
        glDeleteBuffers((sizeof(buffers)/sizeof(buffers[0])), buffers);

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

void AnimatedModel::Draw(Shader shader)
{
    glBindVertexArray(VAO);

    for (unsigned int i = 0 ; i < meshes.size() ; i++) {
        const unsigned int MaterialIndex = meshes[i].MaterialIndex;

        assert(MaterialIndex < textures.size());

        if (textures[MaterialIndex]) {
            textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

		glDrawElementsBaseVertex(GL_TRIANGLES,
                                 meshes[i].IndicesCount,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(unsigned int) * meshes[i].BaseIndex),
                                 meshes[i].BaseVertex);
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
}