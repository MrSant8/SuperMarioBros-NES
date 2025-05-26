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
    Player* player;
    Player* GetPlayer() const { return player; }

    bool gameOver = false;
    bool mariolive2 = false;
    bool mariolive1 = false;
    bool win = false;
    int life = 3;
    std::vector<Object*> objects;

    ShotManager* shots;

private:
    AppStatus LoadLevel(int stage);

    void CheckObjectCollisions();
    void ClearLevel();
    void RenderObjects() const;
    void RenderObjectsDebug(const Color& col) const;

    void RenderGUI() const;
    //Level structure that contains all the static tiles
    TileMap* level;

    //Dynamic objects of the level: items and collectables

    //Enemies present in the level
    EnemyManager* enemies;

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
