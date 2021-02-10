#include "resource_manager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Instantiate static variables
std::map<std::string, Texture2D> ResourceManager::textures;
std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, AnimatedModel> ResourceManager::models;

Shader ResourceManager::LoadShader(const GLchar *vShaderFilename, const GLchar *fShaderFilename, const GLchar *gShaderFilename, std::string name)
{
    shaders[name] = loadShaderFromFilename(vShaderFilename, fShaderFilename, gShaderFilename);
    return shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *textureFilename, GLboolean alpha, std::string name, GLuint wrap, GLuint filterMin, GLuint filterMax)
{
    textures[name] = loadTextureFromFilename(textureFilename, alpha, wrap, filterMin, filterMax);
    return textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return textures[name];
}

AnimatedModel ResourceManager::LoadModel(const GLchar *modelFilename, std::string name)
{
    models[name] = loadModelFromFilename(modelFilename);
    return models[name];
}

AnimatedModel ResourceManager::GetModel(std::string name)
{
    return models[name];
}

void ResourceManager::Clear()
{
    // (Properly) delete all shaders
    for (auto iter : shaders)
        glDeleteProgram(iter.second.ID);
    // (Properly) delete all textures
    for (auto iter : textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFilename(const GLchar *vShaderFilename, const GLchar *fShaderFilename, const GLchar *gShaderFilename)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // Open files
        std::ifstream vertexShaderFile(vShaderFilename);
        std::ifstream fragmentShaderFile(fShaderFilename);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // If geometry shader path is present, also load a geometry shader
        if (gShaderFilename != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFilename);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();
    // 2. Now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFilename != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFilename(const GLchar *textureFilename, GLboolean alpha, GLuint wrap, GLuint filterMin, GLuint filterMax)
{
    // Create Texture object
    Texture2D texture;
    if (alpha)
    {
        texture.InternalFormat = GL_RGBA;
        texture.ImageFormat = GL_RGBA;
    }
    texture.WrapS = wrap;
    texture.WrapT = wrap;
    texture.FilterMin = filterMin;
    texture.FilterMax = filterMax;
    // Load image
    int width, height, channels;
    unsigned char *image = stbi_load(textureFilename, &width, &height, &channels, 0);
    // Now generate texture
    texture.Generate(width, height, image);
    stbi_image_free(image);
    return texture;
}

AnimatedModel ResourceManager::loadModelFromFilename(const std::string &path)
{
    AnimatedModel model;
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_ForceGenNormals);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << std::endl;
    }
    else {
        // retrieve the directory path of the filepath
        model.SetDirectory(path.substr(0, path.find_last_of('/')));
        model.InitFromScene(importer.GetOrphanedScene());
    }
    return model;
}