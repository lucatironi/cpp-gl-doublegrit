#include <sstream>

#include "game.hpp"

bool pixelate = true;
bool freeCam = false;

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
    delete this->Renderer;
    delete this->Sprite;
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
    this->Sprite = new SpriteRenderer(ResourceManager::GetShader("gritty"));
    this->Renderer = new GrittyRenderer(ResourceManager::GetShader("gritty"));
    this->Pixel = new Pixelator(this->WindowWidth, this->WindowHeight, this->FramebufferWidth, this->FramebufferHeight);
    // Load Tilemap Texture
    Texture2D tiles = ResourceManager::LoadTexture("../assets/q2.png", GL_TRUE, "tiles", GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);

    // Initalize level and game objects
    this->CurrentLevel = new Level("../assets/l1.png", ResourceManager::GetShader("gritty"));

    // Configure Camera
    this->Player = new GameObject();
    this->Player->Sprite = ResourceManager::GetTexture("tiles");
    this->Player->Position = this->CurrentLevel->PlayerStartPosition;

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
                this->FreeCam->ProcessKeyboard(FORWARD, deltaTime);
            if (this->Keys[GLFW_KEY_S]) // Backward
                this->FreeCam->ProcessKeyboard(BACKWARD, deltaTime);
            if (this->Keys[GLFW_KEY_A]) // Left
                this->FreeCam->ProcessKeyboard(LEFT, deltaTime);
            if (this->Keys[GLFW_KEY_D]) // Right
                this->FreeCam->ProcessKeyboard(RIGHT, deltaTime);
            if (this->Keys[GLFW_KEY_Q]) // Down
                this->FreeCam->ProcessKeyboard(DOWN, deltaTime);
            if (this->Keys[GLFW_KEY_E]) // Up
                this->FreeCam->ProcessKeyboard(UP, deltaTime);
        }
        else
        {
            GLfloat deltaSpace = 4.0f * deltaTime;
            if (this->Keys[GLFW_KEY_W]) // Forward
                this->Player->Position.z -= deltaSpace;
            if (this->Keys[GLFW_KEY_S]) // Backward
                this->Player->Position.z += deltaSpace;
            if (this->Keys[GLFW_KEY_A]) // Left
                this->Player->Position.x -= deltaSpace;
            if (this->Keys[GLFW_KEY_D]) // Right
                this->Player->Position.x += deltaSpace;
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
        // 2 Toggle free camera
        if (this->Keys[GLFW_KEY_2] && !this->KeysProcessed[GLFW_KEY_2])
        {
            freeCam = !freeCam;
            this->KeysProcessed[GLFW_KEY_2] = GL_TRUE;
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
    }
}

void Game::Render(GLfloat deltaTime)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->State == GAME_ACTIVE || this->State == GAME_PAUSED ||  this->State == GAME_MENU || this->State == GAME_WIN)
    {
        if (pixelate)
            Pixel->BeginRender();
        this->CurrentLevel->Draw(ResourceManager::GetTexture("tiles"));
        this->Player->Draw(*Sprite);
        if (pixelate)
            Pixel->EndRender();
    }

    if (this->State != GAME_ACTIVE)
        Text->RenderText("PAUSED", 4.0f, 4.0f, 0.5f);

    // Render FPS Counter
    std::stringstream fps;
    fps << (int)(1 / deltaTime);
    Text->RenderText(fps.str(), this->WindowWidth - 30.0f, 4.0f, 0.5f);
}

void Game::Reset()
{
    InitPlayer();
}

void Game::InitPlayer()
{

}

void Game::UpdateCamera()
{
    this->CamPosition = this->Player->Position + glm::vec3(0.0f, 4.0f, 4.0f);
    glm::mat4 perspective = glm::perspective(glm::radians(90.0f), static_cast<GLfloat>(this->WindowWidth) / static_cast<GLfloat>(this->WindowHeight), 0.1f, 100.0f);
    glm::mat4 view;
    if(freeCam)
    {
        view = this->FreeCam->GetViewMatrix();
    }
    else
    {
        view = glm::lookAt(this->CamPosition, this->Player->Position, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    ResourceManager::GetShader("gritty").Use().SetMatrix4("view", view);
    ResourceManager::GetShader("gritty").Use().SetMatrix4("projection", perspective);
}
