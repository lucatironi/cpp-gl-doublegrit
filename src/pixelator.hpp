#ifndef PIXELATOR_H
#define PIXELATOR_H

#include <glad/glad.h>

#include "shader.hpp"

class Pixelator
{
  public:
    GLuint WindowWidth, WindowHeight;
    GLuint FramebufferWidth, FramebufferHeight;

    Pixelator(GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight);
    ~Pixelator();

    void BeginRender();
    void EndRender();
    void Render(GLfloat time);

  private:
    GLuint FBO, colorRBO, depthRBO; // framebuffer, color and depth renderbuffers
};

#endif