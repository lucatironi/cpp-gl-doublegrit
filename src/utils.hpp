#ifndef UTILS_H
#define UTILS_H

#include <iostream>

#define SUPPRESS_WARNING(x) (void)x;

static inline GLenum glCheckError_(const char *file, int line)
{
    SUPPRESS_WARNING(glCheckError_)
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

inline bool isNearlyEqual(GLfloat x, GLfloat y)
{
    const GLfloat epsilon = 0.00001f;
    return std::abs(x - y) <= epsilon * std::abs(x);
    // see Knuth section 4.2.2 pages 217-218
}

inline GLfloat shortestAngle(GLfloat current, GLfloat target)
{
    GLfloat angle = (target - current) / 360.0f;
    angle -= floor(angle + 0.5f);
    angle *= 360;
    return angle;
}

#endif