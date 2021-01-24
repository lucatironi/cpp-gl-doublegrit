#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <irrKlang.h>
using namespace irrklang;

#include "player_entity.hpp"
#include "resource_manager.hpp"
#include "text_renderer.hpp"
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

        void DoTheMainLoop(GLfloat deltaTime);

        void ProcessInput(GLfloat deltaTime);
        void ProcessMouse(GLfloat xpos, GLfloat ypos);

        void Update(GLfloat deltaTime);
        void Render(GLfloat deltaTime);

    private:
        GLFWwindow *window;
        GLuint windowWidth, windowHeight, framebufferWidth, framebufferHeight;
        GLfloat lastMouseX, lastMouseY;
        bool firstMouse;

        Pixelator      *pixelator;
        TextRenderer   *textRenderer;
        ISoundEngine   *soundEngine;

        Camera         *freeCamera;
        glm::vec3      camPosition;
        PlayerEntity   *player;
        Level          *currentLevel;

        void initPlayer();
        void updateCamera();
};

#endif