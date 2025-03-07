#include "Entity.h"

Entity::Entity()
{
    pos = {0, 0};
    dir = {0, 0};
    width = 0;
    height = 0;
    speed = 0;
    is_alive = false;
}
Entity::Entity(const Vector2& p, const Vector2& d, int w, int h, int s)
{
    pos = p;
    dir = d;
    width = w;
    height = h;
    speed = s;
    is_alive = true;
}
Entity::~Entity()
{
}
void Entity::Init(const Vector2& p, const Vector2& d, int w, int h, int s)
{
    pos = p;
    dir = d;
    width = w;
    height = h;
    speed = s;
    is_alive = true;
}
void Entity::SetPosition(const Vector2& p)
{
    pos = p;
}
void Entity::SetDirection(const Vector2& d)
{
    dir = d;
}
void Entity::SetAlive(bool b)
{
    is_alive = b;
}
void Entity::Update()
{
    pos.x += dir.x * speed;
    pos.y += dir.y * speed;
}
void Entity::ClampPosition(int x1, int y1, int x2, int y2)
{
    if (pos.x < x1)                 pos.x = x1;
    else if (pos.x > x2 - width)    pos.x = x2 - width;

    if (pos.y < y1)                 pos.y = y1;
    else if (pos.y > y2 - height)   pos.y = y2 - height;
}
Vector2 Entity::GetPosition() const
{
    return pos;
}
int Entity::GetWidth() const
{
    return width;
}
int Entity::GetHeight() const
{
    return height;
}
bool Entity::IsAlive() const
{
    return is_alive;
}
void Entity::Render(const Texture& img) const
{
    DrawTexture(img, pos.x, pos.y, WHITE);
}
void Entity::Render(const Texture& img, const Vector2& offset) const
{
    DrawTexture(img, pos.x + offset.x, pos.y + offset.y, WHITE);
}
void Entity::RenderDebug(const Color& col) const
{
    Rectangle rect = {pos.x, pos.y, (float)width, (float)height};
    DrawRectangleLinesEx(rect, 2, col);
}