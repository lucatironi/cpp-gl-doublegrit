#ifndef LEVEL_H
#define LEVEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>

#include "texture.hpp"
#include "shader.hpp"

struct Light
{
    glm::vec3 position;
    glm::vec3 color;
    float attenuation;
};

class Level
{
    public:
        Level(const GLchar* file, Texture2D texture);
        ~Level();

        glm::vec3 PlayerStartPosition;

        void Draw(Shader shader);
        GLboolean HasWallAt(GLfloat x, GLfloat z);

    private:
        const GLfloat tileFraction = 16.0f / 1024.0f;
        const GLfloat quadSize = 1.0f;

        int levelWidth, levelHeight;
        unsigned char *levelData;

        GLuint VAO;
        Texture2D texture;
        std::vector<GLfloat> vertices;
        std::vector<Light> lights;

        void load(const GLchar* file);
        void initRenderData();
        void pushQuad(GLfloat x1, GLfloat y1, GLfloat z1,
                    GLfloat x2, GLfloat y2, GLfloat z2,
                    GLfloat x3, GLfloat y3, GLfloat z3,
                    GLfloat x4, GLfloat y4, GLfloat z4,
                    GLfloat nx, GLfloat ny, GLfloat nz,
                    GLuint tile);
        void pushFloor(GLfloat x, GLfloat z);
        void pushBlock(GLfloat x, GLfloat z);
        int randomFloorTile();
        int randomWallTile();
        int tileAt(GLfloat x, GLfloat z);
};

#endif