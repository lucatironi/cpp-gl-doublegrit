#ifndef GRITTY_RENDERER_H
#define GRITTY_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "shader.hpp"

class GrittyRenderer
{
  public:
    GrittyRenderer(Shader shader);
    ~GrittyRenderer();

    void DrawCube(Texture2D texture,
                    glm::vec3 position,
                    glm::vec3 size = glm::vec3(10, 10, 10),
                    GLfloat rotate = 0.0f);

  private:
    Shader shader;
    GLuint VAO;

    void initRenderData();
};

#endif