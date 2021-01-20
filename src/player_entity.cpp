#include "player_entity.hpp"

PlayerEntity::PlayerEntity(glm::vec3 position, glm::vec3 size, Shader shader, Model model) :
    Position(position),
    size(size),
    rotation(0.0f),
    acceleration(glm::vec3(0.0f)),
    velocity(glm::vec3(0.0f)),
    shader(shader),
    model(model)
{
}

PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::Move(PlayerMovement direction)
{
    if (direction == FORWARD)
       acceleration.z = -PLAYER_ACCELERATION;
    if (direction == BACKWARD)
       acceleration.z = PLAYER_ACCELERATION;
    if (direction == LEFT)
       acceleration.x = -PLAYER_ACCELERATION;
    if (direction == RIGHT)
        acceleration.x = PLAYER_ACCELERATION;
}

void PlayerEntity::Stop(PlayerAxis axis)
{
    if (axis == LONGITUDINAL)
        acceleration.z = 0.0f;
    if (axis == LATERAL)
        acceleration.x = 0.0f;
}

void PlayerEntity::Update(GLfloat deltaTime)
{
    velocity.x += acceleration.x * deltaTime - velocity.x * std::min(PLAYER_FRICTION * deltaTime, 1.0f);
    velocity.z += acceleration.z * deltaTime - velocity.z * std::min(PLAYER_FRICTION * deltaTime, 1.0f);

    // position
    Position.x += velocity.x * deltaTime;
    Position.z += velocity.z * deltaTime;
}

void PlayerEntity::Draw()
{
    // Prepare transformations
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, Position);

    modelMat = glm::translate(modelMat, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.5f * size.z));
    modelMat = glm::rotate(modelMat, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::translate(modelMat, size * -0.5f);

    modelMat = glm::scale(modelMat, size);

    shader.Use();
    shader.SetInteger("entity", true);
    shader.SetMatrix4("model", modelMat);
    model.Draw(shader);

    shader.SetInteger("entity", false);
}