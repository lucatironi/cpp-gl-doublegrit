#ifndef BASIC_ENTITY_H
#define BASIC_ENTITY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "shader.hpp"

#include <string>

class BasicEntity
{
    public:
        glm::vec3 Position;

        BasicEntity(glm::vec3 position, glm::vec3 size, Texture2D texture, Shader shader);
        ~BasicEntity();

        void Update(GLfloat deltatime);
        void Draw();

    private:
        glm::vec3 size;
        GLfloat rotation;
        Texture2D texture;
        Shader shader;
        GLuint VAO;

        void initRenderData();
};

#endif