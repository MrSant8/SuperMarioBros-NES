#pragma once
#include <raylib.h>
#include "Entity.h"

//Velocities
#define PLAYER_SPEED    2
#define SCENE_SPEED     2


class Level
{
public:
    Level(int width, int height);
    ~Level();

    bool Init();
    void HandleInputPlayer();
    void Update();
    void Render();
    void Release();

private:
    bool LoadTextures();
    void UnloadTextures();

private:
    int window_width, window_height;
    Entity *Player, *Scene;

    bool god_mode;

    Texture2D img_background;
    Texture2D img_player;

    bool move_camera = false;
};

