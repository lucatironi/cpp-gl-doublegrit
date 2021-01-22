#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "shader.hpp"
#include "model.hpp"

#include <string>

// Defines several possible options for player movement. Used as abstraction to stay away from window-system specific input methods
enum PlayerDirection
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum PlayerAxis
{
    LONGITUDINAL,
    LATERAL
};

const GLfloat GRAVITY = 9.8f;
const GLfloat PLAYER_ACCELERATION = 15.0f;
const GLfloat PLAYER_FRICTION = 5.0f;
const GLfloat PLAYER_JUMP_VELOCITY = 4.0f;

class PlayerEntity
{
    public:
        glm::vec3 Position;

        PlayerEntity(glm::vec3 position, glm::vec3 size, Shader shader, Model model);
        ~PlayerEntity();

        void Move(PlayerDirection direction);
        void Jump();
        void Stop(PlayerAxis);

        void Update(GLfloat deltatime);
        void Draw();

    private:
        glm::vec3 size;
        PlayerDirection direction;
        GLfloat rotation;
        glm::vec3 acceleration, velocity;
        Shader shader;
        Model model;
};

#endif