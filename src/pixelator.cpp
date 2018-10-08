#include "pixelator.hpp"

#include <iostream>
#include <vector>

Pixelator::Pixelator(GLuint width, GLuint height, GLuint framebufferwidth, GLuint framebufferheight)
    : Width(width), Height(height), FrameBufferWidth(framebufferwidth), FrameBufferHeight(framebufferheight)
{
    // Initialize framebuffer/renderbuffers objects
    glGenFramebuffers(1, &this->FBO);
    glGenRenderbuffers(1, &this->colorRBO);
    glGenRenderbuffers(1, &this->depthRBO);

    glBindRenderbuffer(GL_RENDERBUFFER, this->colorRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, framebufferwidth, framebufferheight);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::PIXELATOR: Failed to initialize color RBO" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, this->depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferwidth, framebufferheight);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::PIXELATOR: Failed to initialize depth RBO" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach renderbuffer to framebuffer (at location = GL_COLOR_ATTACHMENT0)
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->colorRBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthRBO);
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

void Pixelator::BeginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, this->FrameBufferWidth, this->FrameBufferHeight);
}

void Pixelator::EndRender()
{
    // copy off-screen framebuffer content into the screen framebuffer (also called "default framebuffer")
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);           // write into default framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FBO);   // read from off-screen framebuffer

    // read from location "GL_COLOR_ATTACHMENT0" from the currently bound GL_READ_FRAMEBUFFER
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    // copy:
    glBlitFramebuffer(
        0, 0, this->FrameBufferWidth, this->FrameBufferHeight, // source area: we rendered into framebufferwidth X framebufferheight
        0, 0, this->Width, this->Height,                       // destination area: copy only the area in which we rendered
        GL_COLOR_BUFFER_BIT,                                   // buffer bitfield: copy the color only (from location "GL_COLOR_ATTACHMENT0")
        GL_NEAREST);                                           // filtering parameter

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer

    glViewport(0, 0, this->Width, this->Height);
}