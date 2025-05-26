#include "Player.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Globals.h"
#include <raymath.h>
#include "Enemy.h"
#include "EnemyManager.h"

// Constructor
Player::Player(const Point& p, State s, Look view) :
	Entity(p, PLAYER_PHYSICAL_WIDTH, PLAYER_PHYSICAL_HEIGHT, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE),
	state(s),
	look(view),
	jump_delay(PLAYER_JUMP_DELAY),
	map(nullptr),
	score(0),
	playerisDead(false),
	tilemap(nullptr),
	jumpSound{0},
	dieSound{0},
	coinSound{0},
	flagSound{0},
	powerUpSound{0} {}

Player::~Player() {}

// Inicialización
AppStatus Player::Initialise()
{
	const int n = PLAYER_FRAME_SIZE;
	ResourceManager& data = ResourceManager::Instance();

	if (data.LoadTexture(Resource::IMG_PLAYER, "Assets/Textures/playerSprites.png") != AppStatus::OK)
	{
		LOG("Failed to load player sprite texture");
		return AppStatus::ERROR;
	}

	render = new Sprite(data.GetTexture(Resource::IMG_PLAYER));

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)PlayerAnim::NUM_ANIMATIONS);

	// ----------- BIG MARIO ANIMATIONS ------------------------//
	// Idle
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT_BIG, { 0, 32, n, n }); 
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT_BIG, { 0, 32, -n, n });

	// Walk
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT_BIG, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT_BIG, { (float)i * n, 6 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT_BIG, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT_BIG, { (float)i * n, 6 * n, -n, n });

	// Jump
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_RIGHT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_RIGHT_BIG, { 32, 32, n, n }); 
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_LEFT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_LEFT_BIG, { 32, 32, -n, n });

	// Fall
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_RIGHT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT_BIG, { 32, 32, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_LEFT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT_BIG, { 32, 32, -n, n });

	//Crouch
	sprite->SetAnimationDelay((int)PlayerAnim::CROUCH_BIG_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::CROUCH_BIG_LEFT, { n * 3, n, -n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::CROUCH_BIG_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::CROUCH_BIG_RIGHT, { n * 3, n, n, n });
	// Flag
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_LEFT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_LEFT_BIG, { n * 2, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_RIGHT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_RIGHT_BIG, { n * 15, n*4, n, n });


	// ----------- FIRE MARIO ANIMATIONS -------------------------//
	// Idle 
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT_FIRE, { 0, 2 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT_FIRE, { 0, 2*n, -n, n }); 

	// Walk
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT_FIRE, ANIM_DELAY);
	for (int i = 0; i < 5; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT_FIRE, { (float)i * n, 2 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT_FIRE, ANIM_DELAY);
	for (int i = 0; i < 5; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT_FIRE, { (float)i * n, 2 * n, -n, n });

	// Jump
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_RIGHT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_RIGHT_FIRE, { 6 * n, 2 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_LEFT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_LEFT_FIRE, { 6 * n, 2 * n, -n, n });

	// Fall
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_RIGHT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT_FIRE, { 6 * n, 2 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_LEFT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT_FIRE, { 6 * n, 2 * n, -n, n });
	// Crouch
	sprite->SetAnimationDelay((int)PlayerAnim::CROUCH_FIRE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::CROUCH_FIRE_LEFT, { n * 7, 2*n, -n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::CROUCH_FIRE_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::CROUCH_FIRE_RIGHT, { n * 7, 2 *n, n, n });
	// Flag
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_LEFT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_LEFT_FIRE, { n * 8, n*4, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_RIGHT_FIRE, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_RIGHT_FIRE, { n * 4, n, n, n });

	// ----------- SMALL FIRE MARIO ANIMATIONS -------------------------//
	// Idle 
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT_FIRE_SMALL, { 13*n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT_FIRE_SMALL, { 13*n, n, -n, n });

	// Walk
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT_FIRE_SMALL, ANIM_DELAY);
	for (int i = 14; i < 16; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT_FIRE_SMALL, { (float)i * n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT_FIRE_SMALL, ANIM_DELAY);
	for (int i = 14; i < 16; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT_FIRE_SMALL, { (float)i * n, n, -n, n });

	// Jump
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_RIGHT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_RIGHT_FIRE_SMALL, { 17 * n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_LEFT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_LEFT_FIRE_SMALL, { 17 * n, n, -n, n });

	// Fall
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_RIGHT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT_FIRE_SMALL, { 17 * n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_LEFT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT_FIRE_SMALL, { 17 * n, n, -n, n });

	// Flag
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_LEFT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_LEFT_FIRE_SMALL, { n * 18, n, -n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_RIGHT_FIRE_SMALL, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_RIGHT_FIRE_SMALL, { n * 18, n, n, n });

	// ----------- BIG STAR MARIO ANIMATIONS -------------------------//
	// Idle 
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT_STAR_BIG, 2);
	for (int i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT_STAR_BIG, { (float)i * n, 3 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT_STAR_BIG, 2);
	for (int i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT_STAR_BIG, { (float)i * n, 3 * n, -n, n });

	// Walk
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT_STAR_BIG, 2);
	for (int i = 4; i < 15; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT_STAR_BIG, { (float)i * n, 3 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT_STAR_BIG, 2);
	for (int i = 4; i < 15; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT_STAR_BIG, { (float)i * n, 3 * n, -n, n });

	// Jump
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_RIGHT_STAR_BIG, 2);
	for (int i = 16; i < 19; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::JUMPING_RIGHT_STAR_BIG, { (float)i * n, 3 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_LEFT_STAR_BIG, 2);
	for (int i = 16; i < 19; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::JUMPING_LEFT_STAR_BIG, { (float)i * n, 3 * n, -n, n });

	// Fall
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_RIGHT_STAR_BIG, 2);
	for (int i = 16; i < 19; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT_STAR_BIG, { (float)i * n, 3 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_LEFT_STAR_BIG, 2);
	for (int i = 16; i < 19; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT_STAR_BIG, { (float)i * n, 3 * n, -n, n });

	// Crouch
	sprite->SetAnimationDelay((int)PlayerAnim::CROUCH_STAR_RIGHT, 2);
	for (int i = 8; i < 11; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::CROUCH_STAR_RIGHT, { (float)i * n, 2 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::CROUCH_STAR_LEFT, 2);
	for (int i = 8; i < 11; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::CROUCH_STAR_LEFT, { (float)i * n, 2 * n, -n, n });

	// Flag
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_RIGHT_STAR_BIG, 2);
	for (int i = 12; i < 15; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FLAG_RIGHT_STAR_BIG, { (float)i * n, 4 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_LEFT_STAR_BIG, 2);
	for (int i = 12; i < 15; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FLAG_LEFT_STAR_BIG, { (float)i * n, 4 * n, -n, n });

	// ----------- SMALL STAR MARIO ANIMATIONS -------------------------//
	// Idle 
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT_STAR, 2);
	for (int i = 4; i < 7; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT_STAR, { (float)i * n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT_STAR, 2);
	for (int i = 4; i < 7; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT_STAR, { (float)i * n, 0, -n, n });

	// Walk
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT_STAR, 2);
	for (int i = 8; i < 19; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT_STAR, { (float)i * n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT_STAR, 2);
	for (int i = 8; i < 19; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT_STAR, { (float)i * n, 0, -n, n });

	// Jump
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_RIGHT_STAR, 2);
	for (int i = 5; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::JUMPING_RIGHT_STAR, { (float)i * n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_LEFT_STAR, 2);
	for (int i = 5; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::JUMPING_LEFT_STAR, { (float)i * n, n, -n, n });

	// Fall
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_RIGHT_STAR, 2);
	for (int i = 5; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT_STAR, { (float)i * n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_LEFT_STAR, 2);
	for (int i = 5; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT_STAR, { (float)i * n, n, -n, n });

	// Flag
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_RIGHT_STAR, 2);
	for (int i = 9; i < 12; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FLAG_RIGHT_STAR, { (float)i * n, n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_LEFT_STAR, 2);
	for (int i = 9; i < 12; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::FLAG_LEFT_STAR, { (float)i * n, n, -n, n });

	// ----------- SMALL MARIO ANIMATIONS ------------------------//
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { 0, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { 0, 0, -n, n });

	// Walk
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { (float)i * n, 4 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { (float)i * n, 4 * n, -n, n });


	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT, {n, 5 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT, {n, 5 * n, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_RIGHT, { n, 5 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_LEFT, { n, 5 * n, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::LEVITATING_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::LEVITATING_RIGHT, { n, 5 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::LEVITATING_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::LEVITATING_LEFT, { n, 5 * n, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::CLIMBING, ANIM_LADDER_DELAY);
	for (int i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::CLIMBING, { (float)i * n, 6 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::CLIMBING_PRE_TOP, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::CLIMBING_PRE_TOP, { 4 * n, 6 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::CLIMBING_TOP, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::CLIMBING_TOP, { 5 * n, 6 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::DEAD, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::DEAD, { n, 0, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_LEFT, { n*2, 0, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_RIGHT, { n * 3, 0, n, n });

	sprite->SetAnimation((int)PlayerAnim::IDLE_RIGHT);

	//Sounds
	jumpSound = LoadSound("Assets/Audio/Fx/Jump.wav");
	dieSound = LoadSound("Assets/Audio/Fx/Die.wav");
	coinSound = LoadSound("Assets/Audio/Fx/Coin.wav");
	flagSound = LoadSound("Assets/Audio/Fx/Flagpole.wav");
	powerUpSound = LoadSound("Assets/Audio/Fx/Powerup.wav");
	pipeSound = LoadSound("Assets/Audio/Fx/Pipe.wav");
	fireballSound = LoadSound("Assets/Audio/Fx/fireBall.wav");

	return AppStatus::OK;
}
// Score y tiempo
void Player::InitScore() { score = 0; }
void Player::IncrScore(int n) { score += n; }
int Player::GetScore() { return score; }

void Player::UpdateTime(float deltaTime)
{
	timeCounter += deltaTime;
	float timeReductionRate = map->disappear ? 5.0f : 1.0f;
	if (timeCounter >= 1.0f / timeReductionRate)
	{
		time = std::max(0, time - 1);
		timeCounter = 0.0f;
	}
}
int Player::GetTime()
{
	return time;
}

void Player::DecrTime(int amount)
{
	time -= amount;
	if (time < 0) time = 0;
}


// Getters & Setters
void Player::SetTileMap(TileMap* tilemap) { map = tilemap; }
bool Player::IsLookingRight() const { return look == Look::RIGHT; }
bool Player::IsLookingLeft() const { return look == Look::LEFT; }
bool Player::IsAscending() const { return dir.y < -PLAYER_LEVITATING_SPEED; }
bool Player::IsLevitating() const { return fabs(dir.y) <= PLAYER_LEVITATING_SPEED; }
bool Player::IsDescending() const { return dir.y > PLAYER_LEVITATING_SPEED; }
bool Player::IsInFirstHalfTile() const { return pos.y % TILE_SIZE < TILE_SIZE / 2; }
bool Player::IsInSecondHalfTile() const { return pos.y % TILE_SIZE >= TILE_SIZE / 2; }

void Player::SetEnemyManager(EnemyManager* enemyMgr) {
	enemyManager = enemyMgr;
}

void Player::SetAnimation(int id) { dynamic_cast<Sprite*>(render)->SetAnimation(id); }
PlayerAnim Player::GetAnimation() { return (PlayerAnim)dynamic_cast<Sprite*>(render)->GetAnimation(); }

// State Change & Anim
void Player::Stop()
{
	dir = { 0, 0 };
	state = State::IDLE;
}

void Player::StartWalkingLeft()
{
	state = State::WALKING;
	look = Look::LEFT;
}

void Player::StartWalkingRight()
{
	state = State::WALKING;
	look = Look::RIGHT;
}

void Player::StartFalling()
{
	
	dir.y = PLAYER_SPEED * 1.5f;
	state = State::FALLING;
}

void Player::StartJumping()
{
	dir.y = -PLAYER_JUMP_FORCE;
	state = State::JUMPING;
	jump_delay = PLAYER_JUMP_DELAY;
	PlaySound(jumpSound);

}

void Player::StartClimbingUp()
{
	state = State::CLIMBING;
	dynamic_cast<Sprite*>(render)->SetManualMode();
}

void Player::StartClimbingDown()
{
	state = State::CLIMBING;
	dynamic_cast<Sprite*>(render)->SetManualMode();
}
void Player::Update()
{
	if (IsKeyDown(KEY_DOWN) && isBigMario && state != State::JUMPING && state != State::FALLING && state != State::CLIMBING) {
		state = State::CROUCHING;
		ChangeColliderSize();
	}
	else if (IsKeyUp(KEY_DOWN) && isBigMario && state != State::JUMPING && state != State::FALLING && state != State::CLIMBING)
		{
			ChangeColliderSize();
			state = State::IDLE;
		}
	if (isStarMario) {
		StarPower();
	}
	//Fire Shot
	if (isFireMario && fireCooldown <= 0.0f && activeFireballs < MAX_FIREBALLS && IsKeyPressed(KEY_LEFT_CONTROL)) {
		ShootFireball();
		fireCooldown = FIRE_RATE;
	}
	if (fireCooldown > 0.0f) {
		fireCooldown -= GetFrameTime();
	}

	//Hit Cooldown
	if (invincibleTimer > 0.0f) {
		invincibleTimer -= GetFrameTime();
	}

	if (map->disappear) {
		walkingcastle = false;
		pos.x = 4000;
		return;
	}

	if (state == State::DEAD || map->playerDead)
	{
		pos.y += dir.y;
		dir.y += GRAVITY_FORCE;
		dynamic_cast<Sprite*>(render)->Update();

		deathTimer += GetFrameTime();
		if (deathTimer >= DEATH_DURATION)
		{
			playerisDead = true;
		}
		return;
	}

	if (map->LowerFlag && !map->flagFullyLowered)
	{
		playEndLevelMusic = false;
		pos.x = 3155;
		if (pos.y > 191) {
			pos.y = 191;
		}

		if (state != State::FLAG)
		{
			state = State::FLAG;
			SetAnimationByState();

			if (!flagSoundPlayed) {
				PlaySound(flagSound);

				flagSoundPlayed = true;
			}
		}
	}
	else if (map->LowerFlag && map->flagFullyLowered && !walkingcastle) {
		pos.x = 3157;
		if (pos.y > 191) {
			pos.y = 191;
		}

		state = State::FLAG;

		if (!playEndLevelMusic) { 
			playEndLevelMusic = true;
		}

		walkingcastle = true;
	}
	if (walkingcastle)
	{

		timee--;
		if (timee <= 0) {
			MoveX();
		}
	}
	else {
		MoveX();
		ChangeColliderSize();
		UpdateTime(GetFrameTime());
	}
	MoveY();

	if (IsKeyDown(KEY_THREE)) {
		pos.x = 2575;
	}

	dynamic_cast<Sprite*>(render)->Update();
}
void Player::MoveX()
{
	AABB box;
	int prev_x = pos.x;
	int current_speed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) ? PLAYER_SPEED_BOOST : PLAYER_SPEED;

	if (state == State::CLIMBING) return;

	bool preventMovement = (state == State::CROUCHING);

	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
	{
		if (!preventMovement && (pos.x - current_speed) > 0) {
			pos.x += -current_speed;
		}

		if (state == State::IDLE && !preventMovement) StartWalkingLeft();
		else if (IsLookingRight()) {
			look = Look::LEFT;
		}

		box = GetHitbox();
		if (!preventMovement && map->TestCollisionWallLeft(box)) {
			pos.x = prev_x;
			if (state == State::WALKING) Stop();
		}
	}
	else if (IsKeyDown(KEY_RIGHT) || walkingcastle)
	{
		if (!preventMovement) {
			pos.x += current_speed;
		}

		if (state == State::IDLE && !preventMovement) StartWalkingRight();
		else if (IsLookingLeft()) {
			look = Look::RIGHT;
		}

		box = GetHitbox();
		if (!preventMovement && map->TestCollisionWallRight(box)) {
			pos.x = prev_x;
			if (state == State::WALKING) Stop();
		}

		if (!preventMovement && pos.x == 960 && pos.y == 447)
		{
			pos.x = 2614;
			pos.y = 175;
			teletransportation = false;
			Stop();
			return;
		}
	}
	else
	{
		if (state == State::WALKING) Stop();
	}

	SetAnimationByState();
}

void Player::MoveY()
{
	AABB box;
	if (state == State::JUMPING)
	{
		LogicJumping();

		box = GetHitbox();
		box.pos.y--;

		if (map->CollisionY(box.pos, box.width))
		{
			AABB hitbox = GetHitbox();
			Point tileHit;
			//tilemap->ActivateLaserAnimation(tileHit.x, tileHit.y);
			map->TestCollisionFromBelow(hitbox, &hitbox.pos.y, &tileHit);

			pos.y += 16;
			StartFalling(); // Stop jump and start falling

		}
	}
	else if (state == State::CLIMBING)
	{
		LogicClimbing();
	}
	else //idle, walking, falling
	{
		pos.y += PLAYER_SPEED*1.5f;
		box = GetHitbox();
		if (map->TestCollisionGround(box, &pos.y))
		{
			if (state == State::FALLING) Stop();

			if (IsKeyDown(KEY_UP))
			{
				box = GetHitbox();
				if (map->TestOnLadder(box, &pos.x))
					StartClimbingUp();
			}
			else if (IsKeyDown(KEY_DOWN))
			{
				// Check for special teleportation positions
				if (pos.x >= 734 && pos.x <= 754 && pos.y == 143)
				{
					pos.x = 770+16;
					pos.y = 255;
					teletransportation = true;
					PlaySound(pipeSound);
					Stop();
					return;
				}
				box = GetHitbox();
				box.pos.y++;
				if (map->TestOnLadderTop(box, &pos.x))
				{
					StartClimbingDown();
					pos.y += PLAYER_LADDER_SPEED;
				}
			}
			else if (IsKeyPressed(KEY_SPACE))
			{
				StartJumping();
			}
		}
		else
		{
			if (state != State::FALLING) StartFalling();
		}
	}
}
void Player::LogicJumping()
{
	AABB box, prev_box;
	int prev_y;

	jump_delay--;
	if (jump_delay == 0)
	{
		prev_y = pos.y;
		prev_box = GetHitbox();

		pos.y += dir.y;
		pos.y -= 5;  
		dir.y += GRAVITY_FORCE*1.25;  

		jump_delay = PLAYER_JUMP_DELAY;

		//Is the jump finished?
		if (dir.y > PLAYER_JUMP_FORCE)
		{
			dir.y = PLAYER_SPEED;
			StartFalling();
		}
		else
		{
			//Jumping is represented with 3 different states
			if (IsAscending())
			{
				if (IsLookingRight())	SetAnimationByState();
				else					SetAnimationByState();
			}
			else if (IsLevitating())
			{
				if (IsLookingRight())	SetAnimationByState();
				else					SetAnimationByState();
			}
			else if (IsDescending())
			{
				if (IsLookingRight())	SetAnimationByState();
				else					SetAnimationByState();
			}
		}
		//We check ground collision when jumping down
		if (dir.y >= 0)
		{
			box = GetHitbox();

			if (!map->TestCollisionGround(prev_box, &prev_y) &&
				map->TestCollisionGround(box, &pos.y))
			{
				Stop();
			}
		}
	}
}
void Player::LogicClimbing()
{
	AABB box;
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	int tmp;

	if (IsKeyDown(KEY_UP))
	{
		pos.y -= PLAYER_LADDER_SPEED;
		sprite->NextFrame();
	}
	else if (IsKeyDown(KEY_DOWN))
	{
		pos.y += PLAYER_LADDER_SPEED;
		sprite->PrevFrame();
	}

	box = GetHitbox();
	if (map->TestOnLadderTop(box, &tmp))
	{
		if (IsInSecondHalfTile())		SetAnimationByState();
		else if (IsInFirstHalfTile())	SetAnimationByState();
		else					LOG("Internal error, tile should be a LADDER_TOP, coord: (%d,%d)", box.pos.x, box.pos.y);
	}
	else if (map->TestCollisionGround(box, &pos.y))
	{
		//Case leaving the ladder descending.
		Stop();
		sprite->SetAutomaticMode();

		enemy->isDead();
	}
	else if (!map->TestOnLadder(box, &tmp))
	{
		Stop();
		sprite->SetAutomaticMode();
	}
	else
	{
		if (GetAnimation() != PlayerAnim::CLIMBING) SetAnimationByState();
	}
}
void Player::DeactivatePowerUps()
{
	if (isBigMario) {
		isBigMario = false;
	}
	if (isFireMario) {
		isFireMario = false;
	}
	if (isStarMario) {
		isStarMario = false;
	}
	map->canBreak = false;
}

void Player::StartDeath()
{
	if (!deathStarted)
	{
		state = State::DEAD;
		dir.y = -7.0f; 
		SetAnimation((int)PlayerAnim::DEAD);
		PlaySound(dieSound);
		DeactivatePowerUps();
		deathStarted = true;
		deathTimer = 0.0f;
	}
}

void Player::DrawDebug(const Color& col) const
{	
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
	
	DrawText(TextFormat("Pos: (%d,%d)\nSize: %dx%d", pos.x, pos.y, width, height), pos.x, pos.y - 30, 8, LIGHTGRAY);
	DrawPixel(pos.x, pos.y, WHITE);
}
void Player::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_PLAYER);
	render->Release();
}
void Player::Bounce() {
	dir.y = -BOUNCE_FORCE;
	state = State::JUMPING;
	SetAnimationByState();
}
void Player::MushroomPower() {
	map->canBreak = true;
	isBigMario = true;
}

void Player::FirePower() {
	map->canBreak = true;
	isFireMario = true;
		//TODO FIREBALL LOGIC
}

void Player::StarPower() {
	if (!isStarMario) {
		isStarMario = true;
		map->canBreak = true;
		starPowerTimer = 0.0f;
	}
	starPowerTimer += GetFrameTime();
	if (starPowerTimer >= maxStarPowerTime) {
		isStarMario = false; 
		PlaySound(pipeSound);
		if (isBigMario || isFireMario)
		{
			map->canBreak = true;
		}
		else {
			map->canBreak = false;
		}
		starPowerTimer = 0.0f;
	}
}

Vector2 Player::GetVelocity() const {
	return { (float)dir.x, (float)dir.y };
}
void Player::SetAnimationByState()
{
	useBigStar = useBigFire = useBig = useSmallStar = useSmallFire = false;
	// Prioridad: Star > Fire > Big > Small
	if (isStarMario && isBigMario) {
		useBigStar = true;
	}
	else if (isFireMario && isBigMario) {
		useBigFire = true;
	}
	else if (isBigMario) {
		useBig = true;
	}
	else if (isStarMario) {
		useSmallStar = true;
	}
	else if (!isBigMario && isFireMario && !isStarMario)
	{
		useSmallFire = true;
	}
	if (state == lastState &&
		look == lastLook &&
		useBigStar == lastUseBigStar &&
		useBigFire == lastUseBigFire &&
		useBig == lastUseBig &&
		useSmallStar == lastUseSmallStar &&
		useSmallFire == lastUseSmallFire)
	{
		return;
	}

	lastState = state;
	lastLook = look;
	lastUseBigStar = useBigStar;
	lastUseBigFire = useBigFire;
	lastUseBig = useBig;
	lastUseSmallStar = useSmallStar;
	lastUseSmallFire = useSmallFire;

	Sprite* sprite = dynamic_cast<Sprite*>(render);

	if (playerisDead) {
		sprite->SetAnimation((int)PlayerAnim::DEAD);
		return;
	}

	switch (state)
	{
	case State::IDLE:
		if (useBigStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::IDLE_RIGHT_STAR_BIG : PlayerAnim::IDLE_LEFT_STAR_BIG));
		else if (useBigFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::IDLE_RIGHT_FIRE : PlayerAnim::IDLE_LEFT_FIRE));
		else if (useBig)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::IDLE_RIGHT_BIG : PlayerAnim::IDLE_LEFT_BIG));
		else if (useSmallStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::IDLE_RIGHT_STAR : PlayerAnim::IDLE_LEFT_STAR));
		else if(useSmallFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::IDLE_RIGHT_FIRE_SMALL : PlayerAnim::IDLE_LEFT_FIRE_SMALL));
		else
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::IDLE_RIGHT : PlayerAnim::IDLE_LEFT));
		break;

	case State::WALKING:
		if (useBigStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::WALKING_RIGHT_STAR_BIG : PlayerAnim::WALKING_LEFT_STAR_BIG));
		else if (useBigFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::WALKING_RIGHT_FIRE : PlayerAnim::WALKING_LEFT_FIRE));
		else if (useBig)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::WALKING_RIGHT_BIG : PlayerAnim::WALKING_LEFT_BIG));
		else if (useSmallStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::WALKING_RIGHT_STAR : PlayerAnim::WALKING_LEFT_STAR));
		else if (useSmallFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::WALKING_RIGHT_FIRE_SMALL : PlayerAnim::WALKING_LEFT_FIRE_SMALL));
		else
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::WALKING_RIGHT : PlayerAnim::WALKING_LEFT));
		break;

	case State::JUMPING:
		if (useBigStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::JUMPING_RIGHT_STAR_BIG : PlayerAnim::JUMPING_LEFT_STAR_BIG));
		else if (useBigFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::JUMPING_RIGHT_FIRE : PlayerAnim::JUMPING_LEFT_FIRE));
		else if (useBig)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::JUMPING_RIGHT_BIG : PlayerAnim::JUMPING_LEFT_BIG));
		else if (useSmallStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::JUMPING_RIGHT_STAR : PlayerAnim::JUMPING_LEFT_STAR));
		else if (useSmallFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::JUMPING_RIGHT_FIRE_SMALL : PlayerAnim::JUMPING_LEFT_FIRE_SMALL));
		else
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::JUMPING_RIGHT : PlayerAnim::JUMPING_LEFT));
		break;

	case State::FALLING:
		if (useBigStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FALLING_RIGHT_STAR_BIG : PlayerAnim::FALLING_LEFT_STAR_BIG));
		else if (useBigFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FALLING_RIGHT_FIRE : PlayerAnim::FALLING_LEFT_FIRE));
		else if (useBig)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FALLING_RIGHT_BIG : PlayerAnim::FALLING_LEFT_BIG));
		else if (useSmallStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FALLING_RIGHT_STAR : PlayerAnim::FALLING_LEFT_STAR));
		else if (useSmallFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FALLING_RIGHT_FIRE_SMALL : PlayerAnim::FALLING_LEFT_FIRE_SMALL));
		else
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FALLING_RIGHT : PlayerAnim::FALLING_LEFT));
		break;

	case State::CROUCHING:
		if(useBigStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::CROUCH_STAR_RIGHT : PlayerAnim::CROUCH_STAR_LEFT));
		else if (useBigFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::CROUCH_FIRE_RIGHT : PlayerAnim::CROUCH_FIRE_LEFT));
		else if (useBig)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::CROUCH_BIG_RIGHT : PlayerAnim::CROUCH_BIG_LEFT));
		break;

	case State::FLAG:
		if (useBigStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FLAG_RIGHT_STAR_BIG : PlayerAnim::FLAG_LEFT_STAR_BIG));
		else if (useBigFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FLAG_RIGHT_FIRE : PlayerAnim::FLAG_LEFT_FIRE));
		else if (useBig)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FLAG_RIGHT_BIG : PlayerAnim::FLAG_LEFT_BIG));
		else if (useSmallStar)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FLAG_RIGHT_STAR : PlayerAnim::FLAG_LEFT_STAR));
		else if (useSmallFire)
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FLAG_RIGHT_FIRE_SMALL : PlayerAnim::FLAG_LEFT_FIRE_SMALL));
		else
			sprite->SetAnimation((int)(look == Look::RIGHT ? PlayerAnim::FLAG_RIGHT : PlayerAnim::FLAG_LEFT));
		break;
	default:
		break;
	}

	sprite->Update();
}

void Player::ChangeColliderSize()
{
	if (isBigMario)
	{
		height = 32;
	}
	else if ( (isBigMario&&isFireMario||isStarMario) && isCrouching || isBigMario && isCrouching)
	{
		height = 22;
	}
	else
	{
		height = 16;
	}
}
void Player::ShootFireball()
{
	if (!shotManager) return;

	PlaySound(fireballSound);

	Point fireballPos = { pos.x, pos.y + height / 2 };
	Point fireballDir;

	if (look == Look::RIGHT) {
		fireballDir = { 1, 0 };
		fireballPos.x += 12; // ajusta para que salga desde la mano derecha
	}
	else {
		fireballDir = { -1, 0 };
		fireballPos.x -= 12; // ajusta para que salga desde la mano izquierda
	}

	shotManager->Add(fireballPos, fireballDir);
	activeFireballs++; // Aumenta el contador
}
