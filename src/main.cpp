
#include <array>
#include <ostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "game.hpp"
#include "resource_manager.hpp"

static void ToggleFullScreen();

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void MouseCallback(GLFWwindow* window, double xPos, double yPos);

GLFWwindow*  window  = nullptr;
GLFWmonitor* monitor = nullptr;

std::array<int, 2> WindowSize      {800, 600};
std::array<int, 2> WindowPosition  {0, 0};
const float FramebufferRatio = 1.0f / 4.0f;

Game* DoubleGrit;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#endif

    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    window = glfwCreateWindow(mode->width, mode->height, "Double Grit", monitor, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // Disable vsync
    // glfwSwapInterval(0);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Dear ImGui Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // Setup OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    DoubleGrit = new Game(window, mode->width, mode->height, WindowSize[0] * FramebufferRatio, WindowSize[1] * FramebufferRatio);
    DoubleGrit->Init();

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DoubleGrit->DoTheMainLoop(deltaTime);

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();

    // imgui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

static void ToggleFullScreen()
{
    if (glfwGetWindowMonitor(window))
    {
        glfwSetWindowMonitor(window, nullptr,
                             WindowPosition[0], WindowPosition[1],
                             WindowSize[0], WindowSize[1], 0);
        DoubleGrit->SetFramebufferSize(WindowSize[0], WindowSize[1],
                                       WindowSize[0] * FramebufferRatio, WindowSize[1] * FramebufferRatio);
    }
    else
    {
        monitor = glfwGetPrimaryMonitor();
        if (monitor)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwGetWindowSize(window, &WindowSize[0], &WindowSize[1]);
            glfwGetWindowPos(window, &WindowPosition[0], &WindowPosition[1]);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            DoubleGrit->SetFramebufferSize(mode->width, mode->height,
                                           WindowSize[0] * FramebufferRatio, WindowSize[1] * FramebufferRatio);
        }
    }
}

static void KeyCallback(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */)
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

        // F10 toglle fullscreen
        if (DoubleGrit->Keys[GLFW_KEY_F10] && !DoubleGrit->KeysProcessed[GLFW_KEY_F10])
        {
            ToggleFullScreen();
        }
    }
}

static void MouseCallback(GLFWwindow* /* window */, double xpos, double ypos)
{
    DoubleGrit->ProcessMouse(xpos, ypos);
}
