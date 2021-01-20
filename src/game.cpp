#include <sstream>
#include <iostream>
#include <iomanip>

#include "game.hpp"

bool pixelate = true;
bool retro = true;
bool freeCam = false;
bool showStats = true;

Game::Game(GLFWwindow *window, GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight)
    : State(GAME_MENU),
      Keys(),
      KeysProcessed(),
      Window(window),
      WindowWidth(windowWidth),
      WindowHeight(windowHeight),
      FramebufferWidth(framebufferWidth),
      FramebufferHeight(framebufferHeight)
{
    lastMouseX = windowWidth / 2.0f;
    lastMouseY = windowHeight / 2.0f;
    firstMouse = true;
}

Game::~Game()
{
    delete Player;
    delete Text;
    delete Pixel;
    delete FreeCam;
    SoundEngine->drop();
}

void Game::Init()
{
    // Load shaders
    ResourceManager::LoadShader("../src/shaders/gritty.vs", "../src/shaders/gritty.fs", nullptr, "gritty");
    ResourceManager::LoadShader("../src/shaders/text.vs", "../src/shaders/text.fs", nullptr, "text");

    // Configure Text Renderer
    glm::mat4 ortho = glm::ortho(0.0f, static_cast<GLfloat>(WindowWidth), static_cast<GLfloat>(WindowHeight), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("text").Use().SetMatrix4("projection", ortho);
    ResourceManager::GetShader("text").Use().SetInteger("text", 0);
    Text = new TextRenderer(ResourceManager::GetShader("text"));
    Text->LoadFont("../assets/PressStart2P-Regular.ttf", 16);

    // Set render-specific controls
    Pixel = new Pixelator(WindowWidth, WindowHeight, FramebufferWidth, FramebufferHeight);
    // Load Tilemap Texture
    ResourceManager::LoadTexture("../assets/tiles.png", GL_TRUE, "tiles", GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);
    ResourceManager::LoadTexture("../assets/player.png", GL_FALSE, "player", GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);

    // Initalize Level
    CurrentLevel = new Level("../assets/level1.png", ResourceManager::GetShader("gritty"));

    // Configure Player
    ResourceManager::LoadModel("../assets/VikingHelmet.obj", "playerModel");
    Player = new PlayerEntity(CurrentLevel->PlayerStartPosition, glm::vec3(0.25f), ResourceManager::GetShader("gritty"), ResourceManager::GetModel("playerModel"));

    // Configure Camera
    FreeCam = new Camera(Player->Position, glm::vec3(0.0f, 1.0f, 0.0f));
    UpdateCamera();

    // Initialize other objects
    SoundEngine = createIrrKlangDevice();
}

void Game::ProcessInput(GLfloat deltaTime)
{
    if (State == GAME_MENU)
    {
        // ESC quits the game
        if (Keys[GLFW_KEY_ESCAPE] && !KeysProcessed[GLFW_KEY_ESCAPE])
        {
            KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
            glfwSetWindowShouldClose(Window, GL_TRUE);
        }
        // ENTER (re)starts the game
        if (Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER])
        {
            Reset();
            State = GAME_ACTIVE;
            KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }
    if (State == GAME_WIN || State == GAME_LOST)
    {
        // ENTER restart the game and goes to the menu
        if (Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER])
        {
            Reset();
            State = GAME_MENU;
            KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }
    if (State == GAME_PAUSED)
    {
        // ESC goes to Menu
        if (Keys[GLFW_KEY_ESCAPE] && !KeysProcessed[GLFW_KEY_ESCAPE])
        {
            State = GAME_MENU;
            KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
        }
        // ENTER resumes the game
        if (Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER])
        {
            State = GAME_ACTIVE;
            KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }
    if (State == GAME_ACTIVE)
    {
        if (freeCam)
        {
            if (Keys[GLFW_KEY_W]) // Forward
                FreeCam->ProcessKeyboard(CAMERA_FORWARD, deltaTime);
            if (Keys[GLFW_KEY_S]) // Backward
                FreeCam->ProcessKeyboard(CAMERA_BACKWARD, deltaTime);
            if (Keys[GLFW_KEY_A]) // Left
                FreeCam->ProcessKeyboard(CAMERA_LEFT, deltaTime);
            if (Keys[GLFW_KEY_D]) // Right
                FreeCam->ProcessKeyboard(CAMERA_RIGHT, deltaTime);
            if (Keys[GLFW_KEY_Q]) // Down
                FreeCam->ProcessKeyboard(CAMERA_DOWN, deltaTime);
            if (Keys[GLFW_KEY_E]) // Up
                FreeCam->ProcessKeyboard(CAMERA_UP, deltaTime);
        }
        else
        {
            if (Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S]) // Forward
                Player->Move(FORWARD);
            else if (Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_W]) // Backward
                Player->Move(BACKWARD);
            else
                Player->Stop(LONGITUDINAL);

            if (Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D]) // Left
                Player->Move(LEFT);
            else if (Keys[GLFW_KEY_D] && !Keys[GLFW_KEY_A]) // Right
                Player->Move(RIGHT);
            else
                Player->Stop(LATERAL);
        }

        if (Keys[GLFW_KEY_SPACE] && !KeysProcessed[GLFW_KEY_SPACE])
        {
            KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
        }
        // 1 Toggle Pixelate
        if (Keys[GLFW_KEY_1] && !KeysProcessed[GLFW_KEY_1])
        {
            pixelate = !pixelate;
            KeysProcessed[GLFW_KEY_1] = GL_TRUE;
        }
        // 2 Toggle retro look (256 color palette)
        if (Keys[GLFW_KEY_2] && !KeysProcessed[GLFW_KEY_2])
        {
            retro = !retro;
            KeysProcessed[GLFW_KEY_2] = GL_TRUE;
        }
        // 3 Toggle free camera
        if (Keys[GLFW_KEY_3] && !KeysProcessed[GLFW_KEY_3])
        {
            freeCam = !freeCam;
            KeysProcessed[GLFW_KEY_3] = GL_TRUE;
        }
        // 4 Toggle stats
        if (Keys[GLFW_KEY_4] && !KeysProcessed[GLFW_KEY_4])
        {
            showStats = !showStats;
            KeysProcessed[GLFW_KEY_4] = GL_TRUE;
        }
        // ESC pauses game
        if (Keys[GLFW_KEY_ESCAPE] && !KeysProcessed[GLFW_KEY_ESCAPE])
        {
            State = GAME_PAUSED;
            KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
        }
    }
}

void Game::ProcessMouse(GLfloat xpos, GLfloat ypos)
{
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    lastMouseX = xpos;
    lastMouseY = ypos;

    if (State == GAME_ACTIVE)
    {
        FreeCam->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Game::Update(GLfloat deltaTime)
{
    if (State == GAME_ACTIVE)
    {
        UpdateCamera();
        Player->Update(deltaTime);
    }
}

void Game::Render(GLfloat deltaTime)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (State == GAME_ACTIVE ||
        State == GAME_PAUSED ||
        State == GAME_MENU ||
        State == GAME_WIN)
    {
        if (pixelate)
            Pixel->BeginRender();

        CurrentLevel->Draw(ResourceManager::GetTexture("tiles"));
        Player->Draw();

        if (pixelate)
            Pixel->EndRender();
    }

    if (State == GAME_PAUSED)
    {
        Text->RenderText("PAUSED", WindowWidth / 2.0f - 50.0f, WindowHeight / 2.0f - 50.0f, 1.0f);
        Text->RenderText("PRESS ENTER TO CONTINUE", WindowWidth / 2.0f - 190.0f, WindowHeight / 2.0f - 20.0f, 1.0f);
        Text->RenderText("PRESS ESC TO EXIT TO MENU", WindowWidth / 2.0f - 210.0f, WindowHeight / 2.0f + 10.0f, 1.0f);
    }
    if (State == GAME_MENU)
    {
        Text->RenderText("PRESS ENTER TO START", WindowWidth / 2.0f - 150.0f, WindowHeight / 2.0f - 20.0f, 1.0f);
        Text->RenderText("PRESS ESC TO QUIT", WindowWidth / 2.0f - 120.0f, WindowHeight / 2.0f + 10.0f, 1.0f);
    }
    if (showStats)
    {
        // Render FPS counter and player position
        std::stringstream stats;
        stats << std::fixed << std::setprecision(1)
                    << "x:" << Player->Position.x
                    << " z:" << Player->Position.z
                    << " fps:" << (int)(1 / deltaTime);
        Text->RenderText(stats.str(), WindowWidth - 180.0f, 5.0f, 0.5f);
    }
}

void Game::Reset()
{
    InitPlayer();
}

void Game::InitPlayer()
{
    Player->Position = CurrentLevel->PlayerStartPosition;
}

void Game::UpdateCamera()
{
    CamPosition = Player->Position + glm::vec3(0.0f, 2.5f, 2.0f);
    glm::mat4 perspective = glm::perspective(glm::radians(90.0f), static_cast<GLfloat>(WindowWidth) / static_cast<GLfloat>(WindowHeight), 0.1f, 100.0f);
    glm::mat4 view;
    glm::vec3 lightPos;
    glm::vec3 lightColor = glm::vec3(0.5f, 0.25f, 0.0f);

    if(freeCam)
    {
        view = FreeCam->GetViewMatrix();
        lightPos = FreeCam->Position;
    }
    else
    {
        view = glm::lookAt(CamPosition, Player->Position, glm::vec3(0.0f, 1.0f, 0.0f));
        lightPos = Player->Position;
        lightPos.y = 0.5f;
        lightPos.x += 0.1f;
    }

    ResourceManager::GetShader("gritty").Use().SetInteger("freeCam", freeCam);
    ResourceManager::GetShader("gritty").Use().SetInteger("retro", retro);
    ResourceManager::GetShader("gritty").Use().SetMatrix4("view", view);
    ResourceManager::GetShader("gritty").Use().SetMatrix4("projection", perspective);
    ResourceManager::GetShader("gritty").Use().SetVector3f("lightPos", lightPos);
    ResourceManager::GetShader("gritty").Use().SetVector3f("lightColor", lightColor);
}
