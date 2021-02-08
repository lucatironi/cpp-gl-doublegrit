#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <string>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils.hpp"
#include "shader.hpp"
#include "animated_model.hpp"

// Defines several possible options for player movement. Used as abstraction to stay away from window-system specific input methods
enum PlayerDirection
{
    FORWARD,
    LEFT,
    BACKWARD,
    RIGHT
};

enum PlayerAxis
{
    LONGITUDINAL,
    LATERAL
};

enum PlayerAnimations
{
    NONE,
    RUN,
    DEATH,
    IDLE,
    JUMP,
    PUNCH,
    RUN2,
    WALK,
    WORK,
    WORK2
};

const GLfloat GRAVITY = 9.8f;
const GLfloat PLAYER_ACCELERATION = 12.0f;
const GLfloat PLAYER_FRICTION = 10.0f;
const GLfloat PLAYER_JUMP_VELOCITY = 4.0f;
const GLfloat PLAYER_TURN_SPEED = 20.0f;

class PlayerEntity
{
    public:
        glm::vec3 Position;

        PlayerEntity(glm::vec3 position, glm::vec3 size, Shader shader, AnimatedModel model);
        ~PlayerEntity();

        void Move(PlayerDirection direction, GLboolean running = false);
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
        AnimatedModel model;
};

#endif