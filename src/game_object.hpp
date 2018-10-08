#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>

#include "sprite_renderer.hpp"

class GameObject
{
    public:
      glm::vec3 Position, Size, Velocity;
      GLfloat Rotation;

      Texture2D Sprite;

      GameObject();
      GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, glm::vec3 velocity = glm::vec3(0.0f));

      virtual void Draw(SpriteRenderer &renderer);
};

#endif