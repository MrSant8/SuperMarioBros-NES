#pragma once

#include "Entity.h"
#include "TileMap.h"
#include "Enemy.h"
#include "EnemyManager.h"


// Visual Model: 16x16
#define PLAYER_FRAME_SIZE        32

// Physics Model: 12x28
#define PLAYER_PHYSICAL_WIDTH    16
#define PLAYER_PHYSICAL_HEIGHT   16

// Velocities
#define PLAYER_SPEED             2
#define PLAYER_SPEED_BOOST       4
#define PLAYER_LADDER_SPEED      1
// Ladder Animation
#define ANIM_LADDER_DELAY        (2 * ANIM_DELAY)

// Force & Gravity
#define PLAYER_JUMP_FORCE        10
#define PLAYER_JUMP_DELAY        2
#define BOUNCE_FORCE             4
#define PLAYER_LEVITATING_SPEED  4
#define GRAVITY_FORCE            1

#define DEATH_DURATION           2.5
// Logic States
enum class State {
    IDLE, WALKING, JUMPING, FALLING, CLIMBING, DEAD, FLAG
};

// Animation States
enum class PlayerAnim {
    IDLE_LEFT, IDLE_RIGHT,
    WALKING_LEFT, WALKING_RIGHT,
    JUMPING_LEFT, JUMPING_RIGHT,
    LEVITATING_LEFT, LEVITATING_RIGHT,
    FALLING_LEFT, FALLING_RIGHT,
    CLIMBING, CLIMBING_PRE_TOP, CLIMBING_TOP,
    SHOCK_LEFT, SHOCK_RIGHT,
    DEAD,
    FLAG_LEFT,FLAG_RIGHT,
    TELEPORT_LEFT, TELEPORT_RIGHT,
    NUM_ANIMATIONS
};

class EnemyManager;

class Player : public Entity {
public:
    Player(const Point& p, State s, Look view);
    ~Player();

    AppStatus Initialise();
    void SetTileMap(TileMap* tilemap);

    void InitScore();
    void IncrScore(int n);
    int GetScore();
    void UpdateTime(float deltaTime);
    int GetTime();

    void StartDeath();

    void Update();
    void DrawDebug(const Color& col) const;
    void Release();

    void Bounce();
    void MushroomPower();

    void FlowerPower();

    void StarPower();

    Vector2 GetPosition() const {
        return Vector2{ static_cast<float>(pos.x), 0.0f };
    }

    Point GetDirection() const {
        return look == Look::RIGHT ? Point(1, 0) : Point(-1, 0);
    }

    bool IsDead() const {
        return state == State::DEAD;
    }

    PlayerAnim GetAnimation();
    void SetAnimation(int id);

public:
    State state;
    bool playerisDead;
    bool teletransportation = false;
    bool stopMusic = false;
    Sound jumpSound;
    Sound dieSound;
    Sound coinSound;
    Sound flagSound;
    Sound powerUpSound;

    float deathTimer = 0.0f;
    bool deathStarted = false;

    bool isStarMario = false;

private:
    // Direction
    bool IsLookingRight() const;
    bool IsLookingLeft() const;

    // Movement & Logic
    void MoveX();
    void MoveY();
    void LogicJumping();
    void LogicClimbing();

    // Animations Control
    void Stop();
    void StartWalkingLeft();
    void StartWalkingRight();
    void StartFalling();
    void StartJumping();
    void StartClimbingUp();
    void StartClimbingDown();
    void ChangeAnimRight();
    void ChangeAnimLeft();

    // Jump States
    bool IsAscending() const;
    bool IsLevitating() const;
    bool IsDescending() const;

    // Ladders Logic
    bool IsInFirstHalfTile() const;
    bool IsInSecondHalfTile() const;

    Look look;
    int jump_delay = 0;

    TileMap* map = nullptr;          
    EnemyManager* enemyManager = nullptr;
    Enemy* enemy = nullptr;

    int score = 0;
    int time = 400;
    float timeCounter = 0.0f;

    bool move_camera = false;

    TileMap* tilemap;

    bool walkingcastle = false;
    bool flagSoundPlayed = false;

    bool isBigMario = false;
    bool isFireMario = false;
};
