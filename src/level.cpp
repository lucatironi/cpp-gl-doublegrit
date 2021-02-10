#include "level.hpp"

#include <stb_image.h>

Level::Level(const GLchar *file, Texture2D texture) : texture(texture)
{
    load(file);
    initRenderData();
}

Level::~Level()
{
    stbi_image_free(levelData);
    glDeleteVertexArrays(1, &VAO);
}

void Level::Draw(Shader shader)
{
    shader.Use();
    shader.SetMatrix4("model", glm::mat4(1.0f));

    for (unsigned int i = 0; i < lights.size(); i++)
    {
        shader.SetVector3f("lights[" + std::to_string(i) + "].position", lights[i].position);
        shader.SetVector3f("lights[" + std::to_string(i) + "].color", lights[i].color);
        shader.SetFloat("lights[" + std::to_string(i) + "].attenuation", lights[i].attenuation);
    }

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}

GLboolean Level::HasWallAt(GLfloat x, GLfloat z)
{
    return tileAt(x, z) == 128;
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
        x3, y3, z3, nx, ny, nz, u, 1.0f,
        x2, y2, z2, nx, ny, nz, u + tileFraction, 0.0f,
        x1, y1, z1, nx, ny, nz, u, 0.0f,
        x2, y2, z2, nx, ny, nz, u + tileFraction, 0.0f,
        x3, y3, z3, nx, ny, nz, u, 1.0f,
        x4, y4, z4, nx, ny, nz, u + tileFraction, 1.0f};
    vertices.insert(end(vertices), begin(newVertices), end(newVertices));
}

void Level::pushFloor(GLfloat x, GLfloat z)
{
    pushQuad(x, 0.0f, z,
            x + quadSize, 0.0f, z,
            x, 0.0f, z + quadSize,
            x + quadSize, 0.0f, z + quadSize,
            0.0f, 1.0f, 0.0f,
            randomFloorTile());
}

void Level::pushBlock(GLfloat x, GLfloat z)
{
    GLfloat y = quadSize;

    // top
    pushQuad(x, y, z,
            x + quadSize, y, z,
            x, y, z + quadSize,
            x + quadSize, y, z + quadSize,
            0.0f, 1.0f, 0.0f,
            0);
    // right
    pushQuad(x + quadSize, y, z + quadSize,
            x + quadSize, y, z,
            x + quadSize, 0.0f, z + quadSize,
            x + quadSize, 0.0f, z,
            1.0f, 0.0f, 0.0f,
            randomWallTile());
    // front
    pushQuad(x, y, z + quadSize,
            x + quadSize, y, z + quadSize,
            x, 0.0f, z + quadSize,
            x + quadSize, 0.0f, z + quadSize,
            0.0f, 0.0f, 1.0f,
            randomWallTile());
    // left
    pushQuad(x, y, z,
            x, y, z + quadSize,
            x, 0.0f, z,
            x, 0.0f, z + quadSize,
            -1.0f, 0.0f, 0.0f,
            randomWallTile());
    // back
    pushQuad(x + quadSize, y, z,
            x, y, z,
            x + quadSize, 0.0f, z,
            x, 0.0f, z,
            0.0f, 0.0f, -1.0f,
            randomWallTile());
}

void Level::load(const GLchar *file)
{
    // Load level data from image
    int channels;
    levelData = stbi_load(file, &levelWidth, &levelHeight, &channels, 1);

    for (int y = 0; y < levelHeight; y++)
    {
        for (int x = 0; x < levelWidth; x++)
        {
            int colorKey = levelData[levelWidth * y + x];

            switch (colorKey)
            {
            case 255: // white, floor
                pushFloor(x * quadSize, y * quadSize);
                break;
            case 128: // grey, wall
                pushBlock(x * quadSize, y * quadSize);
                break;
            case 76: // red
                pushFloor(x * quadSize, y * quadSize);
                break;
            case 149: // green, player
                PlayerStartPosition = glm::vec3(x + quadSize / 2.0f, 0.0f, y + quadSize / 2.0f);
                pushFloor(x * quadSize, y * quadSize);
                break;
            case 28: // blue, light
                Light light;
                light.position = glm::vec3(x + quadSize / 2.0f, quadSize, y + quadSize / 2.0f);
                light.color = glm::vec3(0.0f, 0.1f, 0.7f);
                light.attenuation = 0.08f;
                lights.push_back(light);
                pushFloor(x * quadSize, y * quadSize);
                break;
            default:
                break;
            }
        }
    }
}

void Level::initRenderData()
{
    // Configure VAO/VBO
    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int Level::randomFloorTile()
{
    const int array[] = {0, 0, 0, 0, 0, 2, 2, 1, 4, 4, 4, 4, 4, 4, 6, 6, 5};
    return array[rand() % 17];
}

int Level::randomWallTile()
{
    const int chance[] = {0, 1, 2, 3, 4, 5};
    if (chance[rand() % 6] < 4)
    {
        return 7;
    }
    else
    {
        const int array[] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        return array[rand() % 10];
    }
}

int Level::tileAt(GLfloat x, GLfloat z)
{
    int pos = levelWidth * (int)z + (int)x;
    return levelData[pos];
}
