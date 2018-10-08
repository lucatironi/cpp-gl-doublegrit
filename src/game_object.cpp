#include "game_object.hpp"

GameObject::GameObject()
    : Position(0), Size(0.2f, 0.5f, 0.2f), Velocity(0.0f), Rotation(0.0f), Sprite() {}

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, glm::vec3 velocity)
    : Position(pos), Size(size), Velocity(velocity), Rotation(0.0f), Sprite(sprite) {}

void GameObject::Draw(SpriteRenderer &renderer)
{
    // renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation);
}