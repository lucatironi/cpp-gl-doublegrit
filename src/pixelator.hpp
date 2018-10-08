#ifndef PIXELATOR_H
#define PIXELATOR_H

#include <glad/glad.h>

#include "shader.hpp"

class Pixelator
{
  public:
    GLuint Width, Height;
    GLuint FrameBufferWidth, FrameBufferHeight;

    Pixelator(GLuint width, GLuint height, GLuint framebufferwidth, GLuint framebufferheight);
    ~Pixelator();

    void BeginRender();
    void EndRender();
    void Render(GLfloat time);

  private:
    GLuint FBO, colorRBO, depthRBO; // framebuffer, color and depth renderbuffers
};

#endif