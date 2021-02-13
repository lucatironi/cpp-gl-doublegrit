#include "player_entity.hpp"

PlayerEntity::PlayerEntity(glm::vec3 position, glm::vec3 size, Texture2D texture, AnimatedModel model) :
    Position(position),
    size(size),
    rotation(NORTH * 45.0f),
    direction(NORTH),
    acceleration(glm::vec3(0.0f)),
    velocity(glm::vec3(0.0f)),
    running(GL_TRUE),
    texture(texture),
    model(model)
{
}

PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::Move(PlayerDirection direction)
{
    this->direction = direction;

    GLfloat acc = running ? PLAYER_ACCELERATION * 3.0f : PLAYER_ACCELERATION;

    switch (direction)
    {
    case NORTH:
        acceleration.z = -acc;
        break;
    case NW:
        acceleration.x = -acc;
        acceleration.z = -acc;
        break;
    case WEST:
        acceleration.x = -acc;
        break;
    case SW:
        acceleration.x = -acc;
        acceleration.z = acc;
        break;
    case SOUTH:
        acceleration.z = acc;
        break;
    case SE:
        acceleration.x = acc;
        acceleration.z = acc;
        break;
    case EAST:
        acceleration.x = acc;
        break;
    case NE:
        acceleration.x = acc;
        acceleration.z = -acc;
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

void PlayerEntity::ToggleWalk()
{
    running = !running;
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
    GLfloat targetRotation = direction * 45.0f;
    if ((int)rotation != targetRotation)
        rotation += shortestAngle(rotation, targetRotation) * PLAYER_TURN_VELOCITY * deltaTime;
    rotation = fmod(rotation, 360.0f);

    velocity += acceleration * deltaTime - velocity * std::min(PLAYER_FRICTION * deltaTime, 1.0f);
    velocity.y -= GRAVITY * deltaTime - velocity.y * std::min(PLAYER_FRICTION * deltaTime, 1.0f);
    Position += velocity * deltaTime;
    Position.y = glm::clamp(Position.y, 0.0f, 1.0f);

    if (isNearlyEqual(acceleration.x, 0.0f) && isNearlyEqual(acceleration.z, 0.0f))
        model.SetAnimation(IDLE);
    else
        running ? model.SetAnimation(RUN) : model.SetAnimation(WALK);
}

void PlayerEntity::Draw(Shader shader)
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
    model.SetBoneTransformations(shader, glfwGetTime() * 25.0f);
    model.Draw(shader);

    shader.SetInteger("entity", false);
}