#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <irrKlang.h>
using namespace irrklang;

#include "game_object.hpp"
#include "resource_manager.hpp"
#include "gritty_renderer.hpp"
#include "text_renderer.hpp"
#include "post_processor.hpp"
#include "pixelator.hpp"
#include "level.hpp"
#include "camera.hpp"

enum GameState
{
    GAME_ACTIVE,
    GAME_PAUSED,
    GAME_MENU,
    GAME_WIN,
    GAME_LOST
};

class Game
{
    public:
        GameState State;
        GLboolean Keys[1024];
        GLboolean KeysProcessed[1024];

        Game(GLFWwindow *window, GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight);
        ~Game();

        void Init();
        void Reset();

        void ProcessInput(GLfloat deltaTime);
        void ProcessMouse(GLfloat xpos, GLfloat ypos);

        void Update(GLfloat deltaTime);
        void Render(GLfloat deltaTime);

        void SetFramebufferSize(GLuint framebufferWidth, GLuint framebufferHeight)
            { this->FramebufferWidth = framebufferWidth; this->FramebufferHeight = framebufferHeight; };

    private:
        GLFWwindow *Window;
        GLuint WindowWidth, WindowHeight, FramebufferWidth, FramebufferHeight;
        GLfloat lastMouseX, lastMouseY;
        bool firstMouse;

        GrittyRenderer *Renderer;
        SpriteRenderer *Sprite;
        Pixelator      *Pixel;
        TextRenderer   *Text;
        ISoundEngine   *SoundEngine;

        Camera         *FreeCam;
        glm::vec3      CamPosition;
        GameObject     *Player;
        Level          *CurrentLevel;

        void InitPlayer();
        void UpdateCamera();
};

#endif