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
    IDLE_LEFT, IDLE_RIGHT, IDLE_RIGHT_BIG, IDLE_LEFT_BIG,
    WALKING_LEFT, WALKING_RIGHT, WALKING_LEFT_BIG, WALKING_RIGHT_BIG,
    JUMPING_LEFT, JUMPING_RIGHT, JUMPING_LEFT_BIG, JUMPING_RIGHT_BIG,
    LEVITATING_LEFT, LEVITATING_RIGHT, LEVITATING_LEFT_BIG, LEVITATING_RIGHT_BIG,
    FALLING_LEFT, FALLING_RIGHT, FALLING_LEFT_BIG, FALLING_RIGHT_BIG,
    CLIMBING, CLIMBING_PRE_TOP, CLIMBING_TOP,
    SHOCK_LEFT, SHOCK_RIGHT,
    DEAD,
    FLAG_LEFT,FLAG_RIGHT, FLAG_LEFT_BIG, FLAG_RIGHT_BIG,
    TELEPORT_LEFT, TELEPORT_RIGHT, TELEPORT_LEFT_BIG, TELEPORT_RIGHT_BIG,
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
    Vector2 GetVelocity() const;
    void SetAnimationByState();
    void ChangeColliderSize();
    bool IsInvincible() const { return invincibleTimer > 0.0f; }
    void SetInvincible(float duration) { invincibleTimer = duration; }
public:
    State state;

    bool teletransportation = false;

    //Sounds
    Sound jumpSound;
    Sound dieSound;
    Sound coinSound;
    Sound flagSound;
    Sound powerUpSound;
    bool stopMusic = false;

    //Death
    float deathTimer = 0.0f;
    bool deathStarted = false;
    bool playerisDead;

    //Star Power 
    bool isStarMario = false;
    float starPowerTimer = 0.0f;
    const float maxStarPowerTime = 10.0f; 

    //Mushroom Power
    bool isBigMario = false;

    //Fire Power
    bool isFireMario = false;

    float invincibleTimer = 0.0f;
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
    bool active = false;

    int timee = 100;
};
