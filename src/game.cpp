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
    this->lastMouseX = windowWidth / 2.0f;
    this->lastMouseY = windowHeight / 2.0f;
    this->firstMouse = true;
}

Game::~Game()
{
    delete this->Player;
    delete this->Text;
    delete this->Pixel;
    delete this->FreeCam;
    this->SoundEngine->drop();
}

void Game::Init()
{
    // Load shaders
    ResourceManager::LoadShader("../src/shaders/gritty.vs", "../src/shaders/gritty.fs", nullptr, "gritty");
    ResourceManager::LoadShader("../src/shaders/text.vs", "../src/shaders/text.fs", nullptr, "text");

    // Configure Text Renderer
    glm::mat4 ortho = glm::ortho(0.0f, static_cast<GLfloat>(this->WindowWidth), static_cast<GLfloat>(this->WindowHeight), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("text").Use().SetMatrix4("projection", ortho);
    ResourceManager::GetShader("text").Use().SetInteger("text", 0);
    this->Text = new TextRenderer(ResourceManager::GetShader("text"));
    this->Text->LoadFont("../assets/PressStart2P-Regular.ttf", 16);

    // Set render-specific controls
    this->Pixel = new Pixelator(this->WindowWidth, this->WindowHeight, this->FramebufferWidth, this->FramebufferHeight);
    // Load Tilemap Texture
    ResourceManager::LoadTexture("../assets/tiles.png", GL_TRUE, "tiles", GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);
    ResourceManager::LoadTexture("../assets/player.png", GL_FALSE, "player", GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);

    // Initalize Level
    this->CurrentLevel = new Level("../assets/level1.png", ResourceManager::GetShader("gritty"));

    // Configure Player
    this->Player = new PlayerEntity(this->CurrentLevel->PlayerStartPosition, glm::vec3(0.25f), ResourceManager::GetTexture("player"), ResourceManager::GetShader("gritty"));

    // Configure Camera
    this->FreeCam = new Camera(this->Player->Position, glm::vec3(0.0f, 1.0f, 0.0f));
    UpdateCamera();

    // Initialize other objects
    this->SoundEngine = createIrrKlangDevice();
}

void Game::ProcessInput(GLfloat deltaTime)
{
    if (this->State == GAME_MENU)
    {
        // ESC quits the game
        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE])
        {
            this->KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
            glfwSetWindowShouldClose(this->Window, GL_TRUE);
        }
        // ENTER (re)starts the game
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->Reset();
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }
    if (this->State == GAME_WIN || this->State == GAME_LOST)
    {
        // ENTER restart the game and goes to the menu
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->Reset();
            this->State = GAME_MENU;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }
    if (this->State == GAME_PAUSED)
    {
        // ESC goes to Menu
        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE])
        {
            this->State = GAME_MENU;
            this->KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
        }
        // ENTER resumes the game
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }
    if (this->State == GAME_ACTIVE)
    {
        if (freeCam)
        {
            if (this->Keys[GLFW_KEY_W]) // Forward
                this->FreeCam->ProcessKeyboard(CAMERA_FORWARD, deltaTime);
            if (this->Keys[GLFW_KEY_S]) // Backward
                this->FreeCam->ProcessKeyboard(CAMERA_BACKWARD, deltaTime);
            if (this->Keys[GLFW_KEY_A]) // Left
                this->FreeCam->ProcessKeyboard(CAMERA_LEFT, deltaTime);
            if (this->Keys[GLFW_KEY_D]) // Right
                this->FreeCam->ProcessKeyboard(CAMERA_RIGHT, deltaTime);
            if (this->Keys[GLFW_KEY_Q]) // Down
                this->FreeCam->ProcessKeyboard(CAMERA_DOWN, deltaTime);
            if (this->Keys[GLFW_KEY_E]) // Up
                this->FreeCam->ProcessKeyboard(CAMERA_UP, deltaTime);
        }
        else
        {
            if (this->Keys[GLFW_KEY_W] && !this->Keys[GLFW_KEY_S]) // Forward
                this->Player->ProcessKeyboard(PLAYER_FORWARD);
            else if (this->Keys[GLFW_KEY_S] && !this->Keys[GLFW_KEY_W]) // Backward
                this->Player->ProcessKeyboard(PLAYER_BACKWARD);
            else
                this->Player->ProcessKeyboard(PLAYER_STOPZ);

            if (this->Keys[GLFW_KEY_A] && !this->Keys[GLFW_KEY_D]) // Left
                this->Player->ProcessKeyboard(PLAYER_LEFT);
            else if (this->Keys[GLFW_KEY_D] && !this->Keys[GLFW_KEY_A]) // Right
                this->Player->ProcessKeyboard(PLAYER_RIGHT);
            else
                this->Player->ProcessKeyboard(PLAYER_STOPX);
        }

        if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
        {
            this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
        }
        // 1 Toggle Pixelate
        if (this->Keys[GLFW_KEY_1] && !this->KeysProcessed[GLFW_KEY_1])
        {
            pixelate = !pixelate;
            this->KeysProcessed[GLFW_KEY_1] = GL_TRUE;
        }
        // 2 Toggle retro look (256 color palette)
        if (this->Keys[GLFW_KEY_2] && !this->KeysProcessed[GLFW_KEY_2])
        {
            retro = !retro;
            this->KeysProcessed[GLFW_KEY_2] = GL_TRUE;
        }
        // 3 Toggle free camera
        if (this->Keys[GLFW_KEY_3] && !this->KeysProcessed[GLFW_KEY_3])
        {
            freeCam = !freeCam;
            this->KeysProcessed[GLFW_KEY_3] = GL_TRUE;
        }
        // 4 Toggle stats
        if (this->Keys[GLFW_KEY_4] && !this->KeysProcessed[GLFW_KEY_4])
        {
            showStats = !showStats;
            this->KeysProcessed[GLFW_KEY_4] = GL_TRUE;
        }
        // ESC pauses game
        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE])
        {
            this->State = GAME_PAUSED;
            this->KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
        }
    }
}

void Game::ProcessMouse(GLfloat xpos, GLfloat ypos)
{
    if (this->firstMouse)
    {
        this->lastMouseX = xpos;
        this->lastMouseY = ypos;
        this->firstMouse = false;
    }

    float xoffset = xpos - this->lastMouseX;
    float yoffset = this->lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    this->lastMouseX = xpos;
    this->lastMouseY = ypos;

    if (this->State == GAME_ACTIVE)
    {
        this->FreeCam->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Game::Update(GLfloat deltaTime)
{
    if (this->State == GAME_ACTIVE)
    {
        UpdateCamera();
        Player->Update(deltaTime);
    }
}

void Game::Render(GLfloat deltaTime)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->State == GAME_ACTIVE ||
        this->State == GAME_PAUSED ||
        this->State == GAME_MENU ||
        this->State == GAME_WIN)
    {
        if (pixelate)
            Pixel->BeginRender();

        this->CurrentLevel->Draw(ResourceManager::GetTexture("tiles"));
        this->Player->Draw();

        if (pixelate)
            Pixel->EndRender();
    }

    if (this->State != GAME_ACTIVE)
    {
        Text->RenderText("PAUSED", this->WindowWidth / 2.0f - 50.0f, this->WindowHeight / 2.0f - 50.0f, 1.0f);
        Text->RenderText("PRESS ENTER TO START ", this->WindowWidth / 2.0f - 150.0f, this->WindowHeight / 2.0f - 20.0f, 1.0f);
        Text->RenderText("PRESS ESC TO QUIT ", this->WindowWidth / 2.0f - 120.0f, this->WindowHeight / 2.0f + 10.0f, 1.0f);
    }
    if (showStats)
    {
        // Render FPS counter and player position
        std::stringstream stats;
        stats << std::fixed << std::setprecision(1)
                    << "x:" << this->Player->Position.x
                    << " z:" << this->Player->Position.z
                    << " fps:" << (int)(1 / deltaTime);
        Text->RenderText(stats.str(), this->WindowWidth - 180.0f, 5.0f, 0.5f);
    }
}

void Game::Reset()
{
    InitPlayer();
}

void Game::InitPlayer()
{
    this->Player->Position = this->CurrentLevel->PlayerStartPosition;
}

void Game::UpdateCamera()
{
    this->CamPosition = this->Player->Position + glm::vec3(0.0f, 2.5f, 2.0f);
    glm::mat4 perspective = glm::perspective(glm::radians(90.0f), static_cast<GLfloat>(this->WindowWidth) / static_cast<GLfloat>(this->WindowHeight), 0.1f, 100.0f);
    glm::mat4 view;
    glm::vec3 lightPos;
    glm::vec3 lightColor = glm::vec3(0.5f, 0.25f, 0.0f);

    if(freeCam)
    {
        view = this->FreeCam->GetViewMatrix();
        lightPos = this->FreeCam->Position;
    }
    else
    {
        view = glm::lookAt(this->CamPosition, this->Player->Position, glm::vec3(0.0f, 1.0f, 0.0f));
        lightPos = this->Player->Position;
        lightPos.y = 0.5f;
        lightPos.x += 0.5f;
    }

    ResourceManager::GetShader("gritty").Use().SetInteger("freeCam", freeCam);
    ResourceManager::GetShader("gritty").Use().SetInteger("retro", retro);
    ResourceManager::GetShader("gritty").Use().SetMatrix4("view", view);
    ResourceManager::GetShader("gritty").Use().SetMatrix4("projection", perspective);
    ResourceManager::GetShader("gritty").Use().SetVector3f("lightPos", lightPos);
    ResourceManager::GetShader("gritty").Use().SetVector3f("lightColor", lightColor);
}
