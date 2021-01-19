#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "shader.hpp"

// Defines several possible options for player movement. Used as abstraction to stay away from window-system specific input methods
enum Player_Movement
{
    PLAYER_FORWARD,
    PLAYER_BACKWARD,
    PLAYER_LEFT,
    PLAYER_RIGHT,
    PLAYER_STOPZ,
    PLAYER_STOPX
};

// Default camera values
const GLfloat PLAYER_ACCELERATION = 15.0f;
const GLfloat PLAYER_FRICTION = 5.0f;

class PlayerEntity
{
    public:
        glm::vec3 Position;

        PlayerEntity(glm::vec3 position, glm::vec3 size, Texture2D texture, Shader shader);
        ~PlayerEntity();

        void ProcessKeyboard(Player_Movement direction);
        void Update(GLfloat deltatime);
        void Draw();

    private:
        glm::vec3 size;
        GLfloat rotation;
        glm::vec3 acceleration, velocity;
        Texture2D texture;
        Shader shader;
        GLuint VAO;

        void initRenderData();
};

#endif