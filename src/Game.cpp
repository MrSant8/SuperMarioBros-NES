#include "Game.h"
#include "Globals.h"
#include <stdio.h>

Game::Game()
{
    state = GameState::MAIN_MENU;
    
    level = nullptr;
    img_menu = {};
}
Game::~Game()
{
    if (level != nullptr)
    {
        level->Release();
        delete level;
        level = nullptr;
    }
}
bool Game::Initialise()
{
    //Initialise windows
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spaceship: arrow keys + space, f1: god mode");

    //Load resources
    if (!LoadTextures())
    {
        LOG("Failed to load textures");
        return false;
    }

    //Set the target frame rate for the application
    SetTargetFPS(60);

    return true;
}
bool Game::LoadTextures()
{
    img_menu = LoadTexture("Assets/Textures/Hud/Start game.png");
    if(img_menu.id == 0)  
    {
        LOG("Failed to load texture Assets/Textures/Hud/Start game.png");
        return false;
    }
    return true;
}
bool Game::BeginPlay()
{
    level = new Level(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (level == nullptr)
    {
        LOG("Failed to allocate memory for Level");
        return false;
    }
    if (!level->Init())
    {
        LOG("Failed to initialise Level");
        return false;
    }

    return true;
}
void Game::FinishPlay()
{
    level->Release();
    delete level;
    level = nullptr;
}
bool Game::Update()
{
    switch (state)
    {
        case GameState::MAIN_MENU: 
            if (IsKeyPressed(KEY_ESCAPE)) return false;
            if (IsKeyPressed(KEY_SPACE))
            {
                if(!BeginPlay()) return false;
                state = GameState::PLAYING;
            }
            break;

        case GameState::PLAYING:  
            if (IsKeyPressed(KEY_ESCAPE))
            {
                FinishPlay();
                state = GameState::MAIN_MENU;
            }
            else
            {
                //Process Input
                level->HandleInputPlayer();
                //Game logic
                level->Update();
            }
            break;
    }
    return true;
}
void Game::Render()
{
    BeginDrawing();
    ClearBackground(BLACK);
  
    switch (state)
    {
        case GameState::MAIN_MENU:
            //Background image
            DrawTextureEx(img_menu, { 0, 0 }, 0.0f, scale, WHITE);
            break;

        case GameState::PLAYING:
            level->Render();
            break;
    }
    
    EndDrawing();
}
void Game::Cleanup()
{
    UnloadTextures();
    CloseWindow();
}
void Game::UnloadTextures()
{
    UnloadTexture(img_menu);
}