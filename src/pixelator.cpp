#include "pixelator.hpp"

#include <iostream>
#include <vector>

Pixelator::Pixelator(GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight), framebufferWidth(framebufferWidth), framebufferHeight(framebufferHeight)
{
    // Initialize framebuffer/renderbuffers objects
    glGenFramebuffers(1, &FBO);
    glGenRenderbuffers(1, &colorRBO);
    glGenRenderbuffers(1, &depthRBO);

    glBindRenderbuffer(GL_RENDERBUFFER, colorRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, framebufferWidth, framebufferHeight);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::PIXELATOR: Failed to initialize color colorRBO" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferWidth, framebufferHeight);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::PIXELATOR: Failed to initialize depth depthRBO" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach renderbuffer to framebuffer (at location = GL_COLOR_ATTACHMENT0)
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    // specify the attachments in which to draw:
    std::vector<GLenum> drawbuffers = {
        GL_COLOR_ATTACHMENT0,
        GL_DEPTH_ATTACHMENT
    };
    glDrawBuffers(drawbuffers.size(), drawbuffers.data());
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::PIXELATOR: Failed to initialize FBO" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Pixelator::~Pixelator()
{
}

void Pixelator::SetFramebufferSize(GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight)
{
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    this->framebufferWidth = framebufferWidth;
    this->framebufferHeight = framebufferHeight;
}

void Pixelator::BeginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void Pixelator::EndRender()
{
    // copy off-screen framebuffer content into the screen framebuffer (also called "default framebuffer")
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);   // write into default framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO); // read from off-screen framebuffer

    // read from location "GL_COLOR_ATTACHMENT0" from the currently bound GL_READ_FRAMEBUFFER
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    // copy:
    glBlitFramebuffer(
        0, 0, framebufferWidth, framebufferHeight, // source area: we rendered into framebufferwidth X framebufferheight
        0, 0, windowWidth,      windowHeight,      // destination area: copy only the area in which we rendered
        GL_COLOR_BUFFER_BIT,                       // buffer bitfield: copy the color only (from location "GL_COLOR_ATTACHMENT0")
        GL_NEAREST);                               // filtering parameter

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer

    glViewport(0, 0, windowWidth, windowHeight);
}