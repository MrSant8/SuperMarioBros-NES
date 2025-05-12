#pragma once
#include <raylib.h>
#include "Player.h"
#include "TileMap.h"
#include "Object.h"
#include "EnemyManager.h"
#include "ShotManager.h"
#include "ParticleManager.h"
#include "Text.h"

enum class DebugMode { OFF, SPRITES_AND_HITBOXES, ONLY_HITBOXES, SIZE };

class Scene
{
public:
    Scene();
    ~Scene();

    AppStatus Init();
    void Update();
    void Render();
    void Release();

    bool gameOver = false;
    bool win = false;

private:
    AppStatus LoadLevel(int stage);

    void CheckObjectCollisions();
    void ClearLevel();
    void RenderObjects() const;
    void RenderObjectsDebug(const Color& col) const;

    void RenderGUI() const;

    Player* player;

    //Level structure that contains all the static tiles
    TileMap* level;

    //Dynamic objects of the level: items and collectables
    std::vector<Object*> objects;

    //Enemies present in the level
    EnemyManager* enemies;

    //Shots thrown by enemies
    ShotManager* shots;

    //Particles generated from entities interactions
    ParticleManager* particles;

    Camera2D camera;
    DebugMode debug;

    Font marioFont;
    float death_timer = 0;
    const Texture2D* backgroundImage;
    const Texture2D* coinnImage;

    int* map = nullptr;

};
