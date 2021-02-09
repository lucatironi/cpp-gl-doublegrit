#ifndef SHADOW_H
#define SHADOW_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "shader.hpp"

class Shadow
{
    public:
        glm::vec3 Position;

        Shadow(glm::vec3 position, glm::vec3 size, Texture2D texture, Shader shader) :
            Position(position),
            size(size),
            rotation(0.0f),
            texture(texture),
            shader(shader)
        {
            // Configure VAO/VBO
            GLuint VBO;
            GLfloat vertices[] = {
                // positions        // normals        // texture coords
                1.0f, 0.01f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.01f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                -1.0f, 0.01f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, 0.01f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, 0.01f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 0.01f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);
            // normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            // texture coord attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        ~Shadow()
        {
            glDeleteVertexArrays(1, &VAO);
        }

        void Update(GLfloat deltatime)
        {
        }

        void Draw()
        {
            // Prepare transformations
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, Position);

            modelMat = glm::translate(modelMat, size * 0.5f);
            modelMat = glm::rotate(modelMat, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
            modelMat = glm::translate(modelMat, size * -0.5f);

            modelMat = glm::scale(modelMat, size);

            shader.Use();
            shader.SetInteger("entity", true);
            shader.SetMatrix4("model", modelMat);

            glActiveTexture(GL_TEXTURE0);
            texture.Bind();

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            shader.SetInteger("entity", false);
        }

    private:
        glm::vec3 size;
        GLfloat rotation;
        Texture2D texture;
        Shader shader;
        GLuint VAO;
};

#endif