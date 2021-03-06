#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <irrKlang.h>
using namespace irrklang;

#include "utils.hpp"
#include "basic_entity.hpp"
#include "player_entity.hpp"
#include "shadow.hpp"
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

        Game(GLFWwindow* window, GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight);
        ~Game();

        void Init();
        void Reset();

        void DoTheMainLoop(GLfloat deltaTime);

        void ProcessInput(GLfloat deltaTime);
        void ProcessMouse(GLfloat xpos, GLfloat ypos);

        void Update(GLfloat deltaTime);
        void Render(GLfloat deltaTime);

        void SetFramebufferSize(GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight);

    private:
        GLFWwindow *window;
        GLuint windowWidth, windowHeight, framebufferWidth, framebufferHeight;
        GLfloat lastMouseX, lastMouseY;
        bool firstMouse;

        Pixelator      *pixelator;
        TextRenderer   *textRenderer;
        ISoundEngine   *soundEngine;

        glm::vec3      camPosition;
        Camera         *freeCamera;
        PlayerEntity   *player;
        BasicEntity    *light;
        Shadow         *shadow;
        Level          *currentLevel;

        void initPlayer();
        void updateCamera();
        void showGameStatsOverlay(bool* pOpen, GLfloat deltaTime);
        void showGameEditorWindow(bool* pOpen);
};

#endif