#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <string>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "animated_model.hpp"

// Defines several possible options for player movement. Used as abstraction to stay away from window-system specific input methods
enum PlayerDirection
{
    SOUTH,
    SE,
    EAST,
    NE,
    NORTH,
    NW,
    WEST,
    SW
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
const GLfloat PLAYER_ACCELERATION = 8.0f;
const GLfloat PLAYER_FRICTION = 12.0f;
const GLfloat PLAYER_JUMP_VELOCITY = 4.0f;
const GLfloat PLAYER_TURN_VELOCITY = 5.0f;

class PlayerEntity
{
    public:
        glm::vec3 Position;

        PlayerEntity(glm::vec3 position, glm::vec3 size, Texture2D texture, AnimatedModel model);
        ~PlayerEntity();

        void Move(PlayerDirection direction);
        void Jump();
        void ToggleWalk();
        void Stop(PlayerAxis);

        void Update(GLfloat deltatime);
        void Draw(Shader shader);

    private:
        glm::vec3 size;
        GLfloat rotation;
        PlayerDirection direction;
        glm::vec3 acceleration, velocity;
        GLboolean running;
        Texture2D texture;
        AnimatedModel model;
};

#endif