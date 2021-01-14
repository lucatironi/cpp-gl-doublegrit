#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "game.hpp"
#include "resource_manager.hpp"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

const unsigned int WindowWidth  = 800;
const unsigned int WindowHeight = 600;

const float framebufferRatio = 1.0 / 4.0;

Game *DoubleGrit;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WindowWidth, WindowHeight, "Double Grit",  nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSwapInterval(0); // Disable vsync

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    DoubleGrit = new Game(window, framebufferWidth, framebufferHeight, WindowWidth * framebufferRatio, WindowHeight * framebufferRatio);
    DoubleGrit->Init();

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        DoubleGrit->ProcessInput(deltaTime);
        DoubleGrit->Update(deltaTime);
        DoubleGrit->Render(deltaTime);

        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            DoubleGrit->Keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
        {
            DoubleGrit->Keys[key] = GL_FALSE;
            DoubleGrit->KeysProcessed[key] = GL_FALSE;
        }
    }
}

void mouse_callback(GLFWwindow* /* window */, double xpos, double ypos)
{
    DoubleGrit->ProcessMouse(xpos, ypos);
}