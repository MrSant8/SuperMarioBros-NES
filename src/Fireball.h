#pragma once
#include "Entity.h"
#include "Player.h"
#include "TileMap.h"
#include "Enemy.h"

class Enemy;

class TileMap;

class Fireball : public Entity
{
public:
    Fireball();
    void Init(Point startPos, int direction); // direction: -1 o 1
    void Update(float dt);
    void Draw();
    void SetPlayerRef(Player* player);
    int GetWidth() const;

private:
    float speed = 150.0f;
    float velocityY = 0.0f;
    const float GRAVITY = 400.0f;
    const float REBOUND_SPEED = 180.0f;
    int dir = 1;
    Player* playerRef = nullptr;
    Texture2D texture;

    Enemy* enemy = nullptr;
    TileMap* map = nullptr;
};
