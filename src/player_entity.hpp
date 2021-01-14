#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "shader.hpp"

class PlayerEntity
{
  public:
    glm::vec3 Position, Size, Velocity;
    GLfloat Rotation;

    PlayerEntity(glm::vec3 pos, glm::vec3 size, Texture2D texture, Shader shader);
    ~PlayerEntity();

    void Draw();

  private:
    Texture2D texture;
    Shader shader;
    GLuint VAO;

    void initRenderData();
};

#endif