#ifndef PIXELATOR_H
#define PIXELATOR_H

#include <glad/glad.h>

#include "shader.hpp"

class Pixelator
{
    public:
        Pixelator(GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight);
        ~Pixelator();

        void BeginRender();
        void EndRender();
        void Render(GLfloat time);

        void SetFramebufferSize(GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight);

    private:
        GLuint windowWidth, windowHeight, framebufferWidth, framebufferHeight;
        GLuint FBO, colorRBO, depthRBO; // framebuffer, color and depth renderbuffers
};

#endif