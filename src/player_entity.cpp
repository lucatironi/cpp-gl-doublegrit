#include "player_entity.hpp"

PlayerEntity::PlayerEntity(glm::vec3 position, glm::vec3 size, Texture2D texture, Shader shader, AnimatedModel model) :
    Position(position),
    size(size),
    direction(FORWARD),
    rotation(FORWARD * 90.0f),
    acceleration(glm::vec3(0.0f)),
    velocity(glm::vec3(0.0f)),
    texture(texture),
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

    switch (direction)
    {
    case FORWARD:
        acceleration.z = -PLAYER_ACCELERATION;
        break;
    case BACKWARD:
        acceleration.z = PLAYER_ACCELERATION;
        break;
    case LEFT:
        acceleration.x = -PLAYER_ACCELERATION;
        break;
    case RIGHT:
        acceleration.x = PLAYER_ACCELERATION;
        break;
    default:
        break;
    }
}

void PlayerEntity::Jump()
{
    if (Position.y == 0.0f) // jump only when on the ground
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
    GLfloat targetRotation = direction * 90.0f;
    if ((int)rotation != targetRotation)
        rotation += shortestAngle(rotation, targetRotation) * PLAYER_TURN_SPEED * deltaTime;
    rotation = fmod(rotation, 360.0f);

    velocity += acceleration * deltaTime - velocity * std::min(PLAYER_FRICTION * deltaTime, 1.0f);
    velocity.y -= GRAVITY * deltaTime - velocity.y * std::min(PLAYER_FRICTION * deltaTime, 1.0f);
    Position += velocity * deltaTime;
    Position.y = glm::clamp(Position.y, 0.0f, 1.0f);
}

void PlayerEntity::Draw(GLfloat deltaTime)
{
    // Prepare transformations
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, Position);
    modelMat = glm::rotate(modelMat, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::scale(modelMat, size);

    shader.Use();
    shader.SetInteger("entity", true);
    shader.SetMatrix4("model", modelMat);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    // Set model transformation
    model.SetBoneTransformations(shader, deltaTime);
    model.Draw(shader);

    shader.SetInteger("entity", false);
}

GLfloat shortestAngle(GLfloat current, GLfloat target)
{
    GLfloat angle = (target - current) / 360.0f;
    angle -= floor(angle + 0.5f);
    angle *= 360;
    return angle;
}