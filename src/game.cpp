#include <sstream>
#include <iostream>
#include <iomanip>

#include "game.hpp"

bool pixelate = true;
bool retro = false;
bool freeCam = false;
bool showStats = true;
bool debugViz = false;
bool running = true;

Game::Game(GLFWwindow *window, GLuint windowWidth, GLuint windowHeight, GLuint framebufferWidth, GLuint framebufferHeight)
    : State(GAME_MENU),
      Keys(),
      KeysProcessed(),
      window(window),
      windowWidth(windowWidth),
      windowHeight(windowHeight),
      framebufferWidth(framebufferWidth),
      framebufferHeight(framebufferHeight)
{
    lastMouseX = windowWidth / 2.0f;
    lastMouseY = windowHeight / 2.0f;
    firstMouse = true;
}

Game::~Game()
{
    delete shadow;
    delete light;
    delete player;
    delete textRenderer;
    delete pixelator;
    delete freeCamera;
    delete currentLevel;
    soundEngine->drop();
}

void Game::Init()
{
    // Load shaders
    ResourceManager::LoadShader("../src/shaders/gritty.vs", "../src/shaders/gritty.fs", nullptr, "gritty");
    ResourceManager::LoadShader("../src/shaders/text.vs", "../src/shaders/text.fs", nullptr, "text");
    ResourceManager::LoadShader("../src/shaders/normalizer.vs", "../src/shaders/normalizer.fs", "../src/shaders/normalizer.gs", "normalizer");

    // Load Textures
    ResourceManager::LoadTexture("../assets/tiles.png", GL_TRUE, "tiles", GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
    ResourceManager::LoadTexture("../assets/player.png", GL_FALSE, "player", GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
    ResourceManager::LoadTexture("../assets/test.png", GL_FALSE, "test", GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
    ResourceManager::LoadTexture("../assets/shadow.png", GL_TRUE, "shadow", GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);

    // Configure Text Renderer
    glm::mat4 ortho = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), static_cast<GLfloat>(windowHeight), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("text").Use().SetMatrix4("projection", ortho);
    ResourceManager::GetShader("text").Use().SetInteger("text", 0);
    textRenderer = new TextRenderer(ResourceManager::GetShader("text"));
    textRenderer->LoadFont("../assets/PressStart2P-Regular.ttf", 16);

    // Set render-specific controls
    pixelator = new Pixelator(windowWidth, windowHeight, framebufferWidth, framebufferHeight);

    // Initalize Level
    currentLevel = new Level("../assets/level1.png", ResourceManager::GetTexture("tiles"));

    // Configure Player
    player = new PlayerEntity(currentLevel->PlayerStartPosition, glm::vec3(0.0015f), ResourceManager::GetTexture("player"), ResourceManager::LoadModel("../assets/player.fbx", "playerModel"));
    light = new BasicEntity(currentLevel->PlayerStartPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.05f), ResourceManager::GetTexture("test"));
    shadow = new Shadow(currentLevel->PlayerStartPosition, glm::vec3(0.5f), ResourceManager::GetTexture("shadow"));

    // Configure Camera
    freeCamera = new Camera();
    freeCamera->Position = glm::vec3(player->Position.x, player->Position.y + 1.0f, player->Position.z);
    updateCamera();

    // Initialize other objects
    soundEngine = createIrrKlangDevice();
}

void Game::Reset()
{
    initPlayer();
}

void Game::DoTheMainLoop(GLfloat deltaTime)
{
    ProcessInput(deltaTime);
    Update(deltaTime);
    Render(deltaTime);
}

void Game::ProcessInput(GLfloat deltaTime)
{
    if (State == GAME_MENU)
    {
        // ESC quits the game
        if (Keys[GLFW_KEY_ESCAPE] && !KeysProcessed[GLFW_KEY_ESCAPE])
        {
            KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
            glfwSetWindowShouldClose(window, GL_TRUE);
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
                freeCamera->ProcessKeyboard(CAMERA_FORWARD, deltaTime);
            if (Keys[GLFW_KEY_S]) // Backward
                freeCamera->ProcessKeyboard(CAMERA_BACKWARD, deltaTime);
            if (Keys[GLFW_KEY_A]) // Left
                freeCamera->ProcessKeyboard(CAMERA_LEFT, deltaTime);
            if (Keys[GLFW_KEY_D]) // Right
                freeCamera->ProcessKeyboard(CAMERA_RIGHT, deltaTime);
            if (Keys[GLFW_KEY_Q]) // Down
                freeCamera->ProcessKeyboard(CAMERA_DOWN, deltaTime);
            if (Keys[GLFW_KEY_E]) // Up
                freeCamera->ProcessKeyboard(CAMERA_UP, deltaTime);
        }
        else
        {
            if (Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S])
                if (!Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D])
                    player->Move(NORTH, running);
                else if (Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D])
                    player->Move(NW, running);
                else if (Keys[GLFW_KEY_D] && !Keys[GLFW_KEY_A])
                    player->Move(NE, running);
                else
                    player->Stop(LATERAL);
            else if (Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_W])
                if (!Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D])
                    player->Move(SOUTH, running);
                else if (Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D])
                    player->Move(SW, running);
                else if (Keys[GLFW_KEY_D] && !Keys[GLFW_KEY_A])
                    player->Move(SE, running);
                else
                    player->Stop(LATERAL);
            else
                player->Stop(LONGITUDINAL);

            if (Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D])
                if (!Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S])
                    player->Move(WEST, running);
                else if (Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S])
                    player->Move(NW, running);
                else if (Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_W])
                    player->Move(SW, running);
                else
                    player->Stop(LONGITUDINAL);
            else if (Keys[GLFW_KEY_D] && !Keys[GLFW_KEY_A])
                if (!Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S])
                    player->Move(EAST, running);
                else if (Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S])
                    player->Move(NE, running);
                else if (Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_W])
                    player->Move(SE, running);
                else
                    player->Stop(LONGITUDINAL);
            else
                player->Stop(LATERAL);
        }

        // SPACE jump
        if (Keys[GLFW_KEY_SPACE] && !KeysProcessed[GLFW_KEY_SPACE])
        {
            player->Jump();
            KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
        }
        // C Toggle running
        if (Keys[GLFW_KEY_C] && !KeysProcessed[GLFW_KEY_C])
        {
            running = !running;
            KeysProcessed[GLFW_KEY_C] = GL_TRUE;
        }
        // ESC pauses game
        if (Keys[GLFW_KEY_ESCAPE] && !KeysProcessed[GLFW_KEY_ESCAPE])
        {
            State = GAME_PAUSED;
            KeysProcessed[GLFW_KEY_ESCAPE] = GL_TRUE;
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
        // 5 Toggle stats
        if (Keys[GLFW_KEY_5] && !KeysProcessed[GLFW_KEY_5])
        {
            debugViz = !debugViz;
            KeysProcessed[GLFW_KEY_5] = GL_TRUE;
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
        freeCamera->ProcessMouseMovement(xoffset, yoffset);
}

void Game::Update(GLfloat deltaTime)
{
    if (State == GAME_ACTIVE)
    {
        glm::vec3 playerLastPosition = player->Position;
        updateCamera();
        player->Update(deltaTime);
        if (currentLevel->HasWallAt(player->Position.x, player->Position.z))
        {
            player->Position.x = playerLastPosition.x;
            player->Position.z = playerLastPosition.z;
        }
        light->Position = player->Position + glm::vec3(0.0f, 1.0f, 0.0f);
        shadow->Position = glm::vec3(player->Position.x, 0.0f, player->Position.z);
    }
}

void Game::Render(GLfloat deltaTime)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (pixelate)
        pixelator->BeginRender();

    if (State == GAME_ACTIVE ||
        State == GAME_PAUSED ||
        State == GAME_MENU ||
        State == GAME_WIN)
    {

        currentLevel->Draw(ResourceManager::GetShader("gritty"));
        light->Draw(ResourceManager::GetShader("gritty"));
        shadow->Draw(ResourceManager::GetShader("gritty"));
        player->Draw(ResourceManager::GetShader("gritty"));

        if (debugViz)
        {
            currentLevel->Draw(ResourceManager::GetShader("normalizer"));
            light->Draw(ResourceManager::GetShader("normalizer"));
            shadow->Draw(ResourceManager::GetShader("normalizer"));
            player->Draw(ResourceManager::GetShader("normalizer"));
        }

    }

    if (pixelate)
        pixelator->EndRender();

    if (State == GAME_PAUSED)
    {
        textRenderer->RenderText("PAUSED", windowWidth / 2.0f - 50.0f, windowHeight / 2.0f - 50.0f, 1.0f);
        textRenderer->RenderText("PRESS ENTER TO CONTINUE", windowWidth / 2.0f - 190.0f, windowHeight / 2.0f - 20.0f, 1.0f);
        textRenderer->RenderText("PRESS ESC TO EXIT TO MENU", windowWidth / 2.0f - 210.0f, windowHeight / 2.0f + 10.0f, 1.0f);
    }
    if (State == GAME_MENU)
    {
        textRenderer->RenderText("PRESS ENTER TO START", windowWidth / 2.0f - 150.0f, windowHeight / 2.0f - 20.0f, 1.0f);
        textRenderer->RenderText("PRESS ESC TO QUIT", windowWidth / 2.0f - 120.0f, windowHeight / 2.0f + 10.0f, 1.0f);
    }
    if (showStats)
    {
        // Render FPS counter and player position
        std::stringstream stats;
        stats << std::fixed << std::setprecision(1)
                    << "x:" << player->Position.x
                    << " y:" << player->Position.y
                    << " z:" << player->Position.z
                    << " fps:" << (int)(1 / deltaTime);
        textRenderer->RenderText(stats.str(), windowWidth - 220.0f, 5.0f, 0.5f);
    }
}

void Game::initPlayer()
{
    player->Position = currentLevel->PlayerStartPosition;
}

void Game::updateCamera()
{
    camPosition = player->Position + glm::vec3(0.0f, 2.0f, 2.0f);
    glm::mat4 perspective = glm::perspective(glm::radians(80.0f), static_cast<GLfloat>(windowWidth) / static_cast<GLfloat>(windowHeight), 0.1f, 100.0f);
    glm::mat4 view;
    glm::vec3 playerLightPos = light->Position;
    glm::vec3 lightColor = glm::vec3(0.7f, 0.1f, 0.0f);

    if (freeCam)
        view = freeCamera->GetViewMatrix();
    else
        view = glm::lookAt(camPosition, player->Position, glm::vec3(0.0f, 1.0f, 0.0f));

    ResourceManager::GetShader("gritty").Use().SetInteger("freeCam", freeCam);
    ResourceManager::GetShader("gritty").Use().SetInteger("retro", retro);
    ResourceManager::GetShader("gritty").Use().SetMatrix4("view", view);
    ResourceManager::GetShader("gritty").Use().SetMatrix4("projection", perspective);
    ResourceManager::GetShader("gritty").Use().SetVector3f("playerLightPos", playerLightPos);
    ResourceManager::GetShader("gritty").Use().SetVector3f("lightColor", lightColor);

    if (debugViz)
    {
        ResourceManager::GetShader("normalizer").Use().SetMatrix4("view", view);
        ResourceManager::GetShader("normalizer").Use().SetMatrix4("projection", perspective);
    }
}
