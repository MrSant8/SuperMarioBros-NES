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
#define BOUNCE_FORCE             3
#define PLAYER_LEVITATING_SPEED  4
#define GRAVITY_FORCE            1

#define DEATH_DURATION           2.5
// Logic States
enum class State {
    IDLE, WALKING, JUMPING, FALLING, CLIMBING, DEAD, FLAG, CROUCHING
};

// Animation States
enum class PlayerAnim {
    IDLE_LEFT, IDLE_RIGHT, IDLE_RIGHT_BIG, IDLE_LEFT_BIG,IDLE_RIGHT_FIRE,IDLE_LEFT_FIRE, IDLE_RIGHT_FIRE_SMALL, IDLE_LEFT_FIRE_SMALL, IDLE_RIGHT_STAR, IDLE_LEFT_STAR, IDLE_RIGHT_STAR_BIG, IDLE_LEFT_STAR_BIG,
    WALKING_LEFT, WALKING_RIGHT, WALKING_LEFT_BIG, WALKING_RIGHT_BIG, WALKING_LEFT_FIRE, WALKING_RIGHT_FIRE, WALKING_LEFT_FIRE_SMALL, WALKING_RIGHT_FIRE_SMALL, WALKING_LEFT_STAR, WALKING_RIGHT_STAR, WALKING_LEFT_STAR_BIG, WALKING_RIGHT_STAR_BIG,
    JUMPING_LEFT, JUMPING_RIGHT, JUMPING_LEFT_BIG, JUMPING_RIGHT_BIG, JUMPING_LEFT_FIRE, JUMPING_RIGHT_FIRE, JUMPING_LEFT_FIRE_SMALL, JUMPING_RIGHT_FIRE_SMALL, JUMPING_LEFT_STAR, JUMPING_RIGHT_STAR, JUMPING_LEFT_STAR_BIG, JUMPING_RIGHT_STAR_BIG,
    LEVITATING_LEFT, LEVITATING_RIGHT,
    FALLING_LEFT, FALLING_RIGHT, FALLING_LEFT_BIG, FALLING_RIGHT_BIG, FALLING_LEFT_FIRE, FALLING_RIGHT_FIRE, FALLING_LEFT_FIRE_SMALL, FALLING_RIGHT_FIRE_SMALL, FALLING_LEFT_STAR_BIG, FALLING_RIGHT_STAR_BIG, FALLING_LEFT_STAR, FALLING_RIGHT_STAR, WALKING_RIGHT_LEFT, WALKING_RIGHT_RIGHT,
    CLIMBING, CLIMBING_PRE_TOP, CLIMBING_TOP,
    SHOCK_LEFT, SHOCK_RIGHT,
    DEAD,CROUCH_BIG_LEFT,CROUCH_BIG_RIGHT, CROUCH_FIRE_LEFT, CROUCH_FIRE_RIGHT, CROUCH_STAR_LEFT, CROUCH_STAR_RIGHT,
    FLAG_LEFT,FLAG_RIGHT, FLAG_LEFT_BIG, FLAG_RIGHT_BIG, FLAG_LEFT_FIRE, FLAG_RIGHT_FIRE, FLAG_LEFT_FIRE_SMALL, FLAG_RIGHT_FIRE_SMALL, FLAG_LEFT_STAR, FLAG_RIGHT_STAR, FLAG_LEFT_STAR_BIG, FLAG_RIGHT_STAR_BIG,
    TELEPORT_LEFT, TELEPORT_RIGHT, TELEPORT_LEFT_BIG, TELEPORT_RIGHT_BIG, TELEPORT_LEFT_FIRE, TELEPORT_RIGHT_FIRE, TELEPORT_LEFT_STAR, TELEPORT_RIGHT_STAR, TELEPORT_LEFT_STAR_BIG, TELEPORT_RIGHT_STAR_BIG,
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

    void FirePower();

    void StarPower();

    void DeactivatePowerUps();

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
    void ShootFireball();
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
    Sound pipeSound;
    Sound fireballSound;

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
    float fireCooldown = 0.0f;
    const float FIRE_RATE = 0.3f; 
    int activeFireballs = 0;
    const int MAX_FIREBALLS = 2;

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
    bool isCrouching = false;
    int timee = 100;


    bool bigStar = false;
    bool bigFire = false;
    bool big = false;
    bool smallStar = false;
    State lastState = State::IDLE;    
    Look lastLook = Look::RIGHT;
    bool lastUseBigStar = false;
    bool lastUseBigFire = false;
    bool lastUseBig = false;
    bool lastUseSmallStar = false;
    bool lastUseSmallFire = false;
};
