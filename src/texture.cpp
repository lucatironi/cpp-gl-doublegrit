#include "texture.hpp"

#include <iostream>

Texture2D::Texture2D()
    : Width(0), Height(0),
    InternalFormat(GL_RGB), ImageFormat(GL_RGB),
    WrapS(GL_REPEAT), WrapT(GL_REPEAT),
    FilterMin(GL_LINEAR), FilterMax(GL_LINEAR)
{
    glGenTextures(1, &ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char *data)
{
    Width = width;
    Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, ImageFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterMax);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, ID);
}