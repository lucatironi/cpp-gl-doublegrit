#include "player_entity.hpp"

PlayerEntity::PlayerEntity(glm::vec3 position, glm::vec3 size, Shader shader, Model model) :
    Position(position),
    size(size),
    direction(FORWARD),
    rotation(180.0f),
    acceleration(glm::vec3(0.0f)),
    velocity(glm::vec3(0.0f)),
    shader(shader),
    model(model)
{
}

PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::Move(PlayerDirection direction)
{
    this->direction = direction;

    if (direction == FORWARD)
        acceleration.z = -PLAYER_ACCELERATION;
    if (direction == BACKWARD)
        acceleration.z = PLAYER_ACCELERATION;
    if (direction == LEFT)
        acceleration.x = -PLAYER_ACCELERATION;
    if (direction == RIGHT)
        acceleration.x = PLAYER_ACCELERATION;
}

void PlayerEntity::Jump()
{
    velocity.y = PLAYER_JUMP_VELOCITY;
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
    GLfloat desiredRotation;

    if (direction == FORWARD)
        desiredRotation = 180.0f;
    if (direction == BACKWARD)
        desiredRotation = 0.0f;
    if (direction == LEFT)
        desiredRotation = 270.0f;
    if (direction == RIGHT)
        desiredRotation = 90.0f;

    GLfloat deltaRotation = desiredRotation - rotation;
    if (rotation >= 360.0f)
        rotation = 0.0f;

    if (abs(deltaRotation) > 0.9f)
    {
        if (rotation < desiredRotation)
        {
            if (abs(rotation - desiredRotation) < 180.0f)
                rotation += deltaRotation * 10.0f * deltaTime;
            else
                rotation -= deltaRotation * 10.0f * deltaTime;
        }
        else
        {
            if (abs(rotation - desiredRotation) < 180.0f)
                rotation -= deltaRotation * 10.0f * deltaTime;
            else
                rotation += deltaRotation * 10.0f * deltaTime;
        }
        
        std::cout << "desired: " << desiredRotation << " rotation: " << rotation << " delta: " << deltaRotation << std::endl;
    }

    velocity += acceleration * deltaTime - velocity * std::min(PLAYER_FRICTION * deltaTime, 1.0f);
    velocity.y -= GRAVITY * deltaTime - velocity.y * std::min(PLAYER_FRICTION * deltaTime, 1.0f);
    Position += velocity * deltaTime;

    if (Position.y < 0.0f)
        Position.y = 0.0f;
}

void PlayerEntity::Draw()
{
    // Prepare transformations
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, Position);
    modelMat = glm::rotate(modelMat, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::scale(modelMat, size);

    shader.Use();
    shader.SetInteger("entity", true);
    shader.SetMatrix4("model", modelMat);
    model.Draw(shader);

    shader.SetInteger("entity", false);
}