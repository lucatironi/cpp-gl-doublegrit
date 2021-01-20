#include "resource_manager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Instantiate static variables
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Model> ResourceManager::Models;

Shader ResourceManager::LoadShader(const GLchar *vShaderFilename, const GLchar *fShaderFilename, const GLchar *gShaderFilename, std::string name)
{
    Shaders[name] = loadShaderFromFilename(vShaderFilename, fShaderFilename, gShaderFilename);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *textureFilename, GLboolean alpha, std::string name, GLuint wrap, GLuint filterMin, GLuint filterMax)
{
    Textures[name] = loadTextureFromFilename(textureFilename, alpha, wrap, filterMin, filterMax);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

Model ResourceManager::LoadModel(const GLchar *modelFilename, std::string name)
{
    Models[name] = loadModelFromFilename(modelFilename);
    return Models[name];
}

Model ResourceManager::GetModel(std::string name)
{
    return Models[name];
}

void ResourceManager::Clear()
{
    // (Properly) delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (Properly) delete all textures
    for (auto iter : Textures)
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
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    texture.Wrap_S = wrap;
    texture.Wrap_T = wrap;
    texture.Filter_Min = filterMin;
    texture.Filter_Max = filterMax;
    // Load image
    int width, height, channels;
    unsigned char *image = stbi_load(textureFilename, &width, &height, &channels, 0);
    // Now generate texture
    texture.Generate(width, height, image);
    stbi_image_free(image);
    return texture;
}

Model ResourceManager::loadModelFromFilename(const std::string &path)
{
    Model model;
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << std::endl;
    }
    else {
        // retrieve the directory path of the filepath
        model.directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        model.ProcessNode(scene->mRootNode, scene);
    }
    return model;
}