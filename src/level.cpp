#include "level.hpp"

#include <stb_image.h>

Level::Level(const GLchar *file, Shader shader)
{
    this->shader = shader;
    this->load(file);
    this->initRenderData();
}

Level::~Level()
{
    glDeleteVertexArrays(1, &this->VAO);
}

void Level::Draw(Texture2D texture)
{
    this->shader.Use();

    this->shader.SetVector3f("lightColor", 1.0f, 1.0f, 1.0f);
    this->shader.SetVector3f("lightPos", this->PlayerStartPosition);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
    glBindVertexArray(0);
}

void Level::pushQuad(GLfloat x1, GLfloat y1, GLfloat z1,
                     GLfloat x2, GLfloat y2, GLfloat z2,
                     GLfloat x3, GLfloat y3, GLfloat z3,
                     GLfloat x4, GLfloat y4, GLfloat z4,
                     GLfloat nx, GLfloat ny, GLfloat nz,
                     GLuint tile)
{
    GLfloat u = tile * tileFraction;

    std::vector<GLfloat> newVertices = {
        x1, y1, z1, nx, ny, nz, u, 0.0f,
        x2, y2, z2, nx, ny, nz, u + tileFraction, 0.0f,
        x3, y3, z3, nx, ny, nz, u, 1.0f,
        x2, y2, z2, nx, ny, nz, u + tileFraction, 0.0f,
        x3, y3, z3, nx, ny, nz, u, 1.0f,
        x4, y4, z4, nx, ny, nz, u + tileFraction, 1.0f};
    this->vertices.insert(end(this->vertices), begin(newVertices), end(newVertices));
}

void Level::pushFloor(GLfloat x, GLfloat z)
{
    this->pushQuad(x, 0.0f, z,
                   x + QUAD_SIZE, 0.0f, z,
                   x, 0.0f, z + QUAD_SIZE,
                   x + QUAD_SIZE, 0.0f, z + QUAD_SIZE,
                   0.0f, 1.0f, 0.0f,
                   1);
}

void Level::pushBlock(GLfloat x, GLfloat z)
{
    GLfloat y = QUAD_SIZE;

    // top
    this->pushQuad(x, y, z,
                   x + QUAD_SIZE, y, z,
                   x, y, z + QUAD_SIZE,
                   x + QUAD_SIZE, y, z + QUAD_SIZE,
                   0.0f, 1.0f, 0.0f,
                   3);
    // right
    this->pushQuad(x + QUAD_SIZE, y, z,
                   x + QUAD_SIZE, y, z + QUAD_SIZE,
                   x + QUAD_SIZE, 0.0f, z,
                   x + QUAD_SIZE, 0.0f, z + QUAD_SIZE,
                   1.0f, 0.0f, 0.0f,
                   7);
    // front
    this->pushQuad(x, y, z + QUAD_SIZE,
                   x + QUAD_SIZE, y, z + QUAD_SIZE,
                   x, 0.0f, z + QUAD_SIZE,
                   x + QUAD_SIZE, 0.0f, z + QUAD_SIZE,
                   0.0f, 0.0f, 1.0f,
                   9);
    // left
    this->pushQuad(x, y, z,
                   x, y, z + QUAD_SIZE,
                   x, 0.0f, z,
                   x, 0.0f, z + QUAD_SIZE,
                   -1.0f, 0.0f, 0.0f,
                   10);
    // back
    this->pushQuad(x, y, z,
                   x + QUAD_SIZE, y, z,
                   x, 0.0f, z,
                   x + QUAD_SIZE, 0.0f, z,
                   0.0f, 0.0f, -1.0f,
                   11);
}

void Level::load(const GLchar *file)
{
    // Load image
    int width, height, channels;
    unsigned char *image = stbi_load(file, &width, &height, &channels, 1);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int colorKey = image[width * y + x];

            switch (colorKey)
            {
            case 255: // white, floor
                pushFloor(x * QUAD_SIZE, y * QUAD_SIZE);
                break;
            case 128: // grey, wall
                pushBlock(x * QUAD_SIZE, y * QUAD_SIZE);
                break;
            case 76: // red
                pushFloor(x * QUAD_SIZE, y * QUAD_SIZE);
                break;
            case 149: // green, player
                this->PlayerStartPosition = glm::vec3(x + QUAD_SIZE / 2.0f, QUAD_SIZE / 2.0f, y + QUAD_SIZE / 2.0f);
                pushFloor(x * QUAD_SIZE, y * QUAD_SIZE);
                break;
            case 28: // blue
                pushFloor(x * QUAD_SIZE, y * QUAD_SIZE);
                break;
            }
        }
    }

    stbi_image_free(image);
}

void Level::initRenderData()
{
    // Configure VAO/VBO
    GLuint VBO;

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}