#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
    public:
        GLuint ID;

        Shader() {}

        Shader &Use();

        void Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);
        void SetFloat(const std::string &name, GLfloat value, GLboolean useShader = false);
        void SetInteger(const std::string &name, GLint value, GLboolean useShader = false);
        void SetVector2f(const std::string &name, GLfloat x, GLfloat y, GLboolean useShader = false);
        void SetVector2f(const std::string &name, const glm::vec2 &value, GLboolean useShader = false);
        void SetVector3f(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
        void SetVector3f(const std::string &name, const glm::vec3 &value, GLboolean useShader = false);
        void SetVector4f(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
        void SetVector4f(const std::string &name, const glm::vec4 &value, GLboolean useShader = false);
        void SetMatrix4(const std::string &name, const glm::mat4 &matrix, GLboolean useShader = false);

    private:
        void checkCompileErrors(GLuint object, std::string type); 
};

#endif