#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ostream>

#include "game.hpp"
#include "resource_manager.hpp"

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xpos, double ypos);

const unsigned int WindowWidth  = 800;
const unsigned int WindowHeight = 600;

const float FramebufferRatio = 1.0f / 4.0f;

Game *DoubleGrit;

int main()
{
    glfwInit();

    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#endif

    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Double Grit", primaryMonitor, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
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

    DoubleGrit = new Game(window, framebufferWidth, framebufferHeight, WindowWidth * FramebufferRatio, WindowHeight * FramebufferRatio);
    DoubleGrit->Init();

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        DoubleGrit->DoTheMainLoop(deltaTime);

        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void KeyCallback(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */)
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

void MouseCallback(GLFWwindow* /* window */, double xpos, double ypos)
{
    DoubleGrit->ProcessMouse(xpos, ypos);
}