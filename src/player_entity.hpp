#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "animated_model.hpp"

// Defines several possible options for player movement. Used as abstraction to stay away from window-system specific input methods
enum PlayerDirection
{
    BACKWARD,
    RIGHT,
    FORWARD,
    LEFT
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
const GLfloat PLAYER_TURN_SPEED = 20.0f;

GLfloat shortestAngle(GLfloat current, GLfloat target);

class PlayerEntity
{
    public:
        glm::vec3 Position;

        PlayerEntity(glm::vec3 position, glm::vec3 size, Texture2D texture, Shader shader, AnimatedModel model);
        ~PlayerEntity();

        void Move(PlayerDirection direction);
        void Jump();
        void Stop(PlayerAxis);

        void Update(GLfloat deltatime);
        void Draw(GLfloat deltatime);

    private:
        glm::vec3 size;
        PlayerDirection direction;
        GLfloat rotation;
        glm::vec3 acceleration, velocity;
        Shader shader;
        Texture2D texture;
        AnimatedModel model;
};

#endif