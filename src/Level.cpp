#include "Level.h"
#include <stdio.h>
#include "Globals.h"

Level::Level(int width, int height)
{
    window_width = width;
    window_height = height;

    Player = nullptr;

    Scene = nullptr;

    god_mode = false;

    img_background = {};
    img_player = {};
}
Level::~Level()
{
    if (Player != nullptr)
    {
        delete Player;
        Player = nullptr;
    }
    
    if (Scene != nullptr)
    {
        delete Scene;
        Scene = nullptr;
    }
}
bool Level::Init()
{
    //Load resources
    if (!LoadTextures())
    {
        LOG("Failed to load textures");
        return false;
    }

    //Initialise player
    Player = new Entity({ 20, window_height / 2.0f }, {0, 0}, 104, 82, PLAYER_SPEED);
    if (Player == nullptr)
    {
        LOG("Failed to allocate memory for Player");
        return false;
    }


    //Initialise scene
    Scene = new Entity({ 0, 0 }, {-1, 0}, img_background.width, window_height, SCENE_SPEED);
    if (Scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return false;
    }

    //Initialise control variables
    god_mode = false;

    return true;
}
void Level::HandleInputPlayer()
{
    Vector2 p;
    Vector2 dir = { 0, 0 };

    if (IsKeyPressed(KEY_F1))       god_mode = !god_mode;
        
    if (IsKeyDown(KEY_UP)) {
        dir.y = -1; 
        move_camera = true;
    }
    else if (IsKeyDown(KEY_DOWN)) {
        dir.y = 1; 
        move_camera = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        dir.x = -1; 
        move_camera = true;
    }
    else if (IsKeyDown(KEY_RIGHT)) {
        dir.x = 1;
        move_camera = true;
    }
    
    Player->SetDirection(dir);

}
void Level::Update()
{
    Vector2 p;

    //Scene scroll
    if (move_camera) {
        Scene->Update();
        p = Scene->GetPosition();
        if (p.x <= -Scene->GetWidth()) Scene->SetPosition({ 0, p.y });
        move_camera = false;
    }


    //Player movement
    Player->Update();

}
void Level::Render()
{
    //Scene
    Scene->Render(img_background);
    Scene->Render(img_background, { (float)Scene->GetWidth(), 0 });

    //Player
    Player->Render(img_player);
    if (god_mode)   Player->RenderDebug(RED);

}
bool Level::LoadTextures()
{
    img_background = LoadTexture("Assets/Maps/Map.png");
    if (img_background.id == 0)
    {
        LOG("Failed to load texture Assets/Maps/Map.png");
        return false;
    }
    img_player = LoadTexture("images/spaceship.png");
    if (img_player.id == 0)
    {
        LOG("Failed to load texture images/spaceship.png");
        return false;
    }


    return true;
}
void Level::Release()
{
    UnloadTextures();
}
void Level::UnloadTextures()
{
    UnloadTexture(img_background);
    UnloadTexture(img_player);
}