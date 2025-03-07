#pragma once
#include "raylib.h"

class Entity
{
public:
    Entity();
    Entity(const Vector2& p, const Vector2& dir, int w, int h, int s);
    ~Entity();

    void Init(const Vector2& p, const Vector2& dir, int w, int h, int s);
    void SetPosition(const Vector2& p);
    void SetDirection(const Vector2& dir);
    void SetAlive(bool b);
    
    //Update position based on direction and speed
    void Update();
    
    //Update position within the given limits
    void ClampPosition(int x1, int y1, int x2, int y2);
    
    Vector2 GetPosition() const;
    int GetWidth() const;
    int GetHeight() const;
    bool IsAlive() const;

    void Render(const Texture& img) const;
    void Render(const Texture& img, const Vector2& offset) const;
    void RenderDebug(const Color& col) const;

private:
    Vector2 pos, dir;
    int width, height;
    int speed;
    bool is_alive;
};