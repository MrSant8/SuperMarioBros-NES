#include "Fireball.h"
#include "EnemyManager.h"
#include "TileMap.h"

Fireball::Fireball()
{
    texture = LoadTexture("assets/images/fireball.png"); // Usa tu propia textura
    width = texture.width;
    height = texture.height;
}

void Fireball::Init(Point startPos, int direction)
{
    pos = startPos;
    dir = direction;
    velocityY = -REBOUND_SPEED; // Rebota al salir
}

void Fireball::SetPlayerRef(Player* player)
{
    playerRef = player;
}

int Fireball::GetWidth() const
{
    return texture.width;
}

void Fireball::Update(float dt)
{
    pos.x += dir * speed;
    velocityY += GRAVITY;
    pos.y += velocityY;

    // Rebota al tocar el suelo
    if (!map->TestFalling({ pos, width, height })) {
        velocityY = -REBOUND_SPEED;
    }

    // Rebota si choca con un tile desde abajo
    if (velocityY > 0 && map->CollisionY({ pos.x, pos.y + height }, width)) {
        velocityY = -REBOUND_SPEED;
    }

    // Muere si choca con una pared
    if (map->TestCollisionWallLeft({ pos, width, height }) || map->TestCollisionWallRight({ pos, width, height })) {
        alive = false;
    }

    // Fuera de pantalla
    if (pos.x < -width || pos.x > GetScreenWidth()) {
        //Destruir Fireball
    }
}

void Fireball::Draw()
{
    DrawTexture(texture, (int)pos.x, (int)pos.y, WHITE);
}
