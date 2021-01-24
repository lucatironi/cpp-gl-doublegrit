#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <glad/glad.h>

#include "texture.hpp"
#include "shader.hpp"
#include "model.hpp"

class ResourceManager
{
    public:
        static Shader LoadShader(const GLchar *vShaderFilename, const GLchar *fShaderFilename, const GLchar *gShaderFilename, std::string name);
        static Shader GetShader(std::string name);
        static Texture2D LoadTexture(const GLchar *textureFilename, GLboolean alpha, std::string name, GLuint wrap, GLuint filterMin, GLuint filterMax);
        static Texture2D GetTexture(std::string name);
        static Model LoadModel(const GLchar *modelFilename, std::string name);
        static Model GetModel(std::string name);
        static void Clear();

    private:
        ResourceManager() {}

        static std::map<std::string, Shader> shaders;
        static std::map<std::string, Texture2D> textures;
        static std::map<std::string, Model> models;

        static Shader loadShaderFromFilename(const GLchar *vShaderFilename, const GLchar *fShaderFilename, const GLchar *gShaderFilename = nullptr);
        static Texture2D loadTextureFromFilename(const GLchar *textureFilename, GLboolean alpha, GLuint wrap, GLuint filterMin, GLuint filterMax);
        static Model loadModelFromFilename(const std::string &path);
};

#endif