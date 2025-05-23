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

	// Animaciones básicas
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT_BIG, { 0, 32, n, n }); // Big Mario is 32 pixels below small Mario
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT_BIG, { 0, 32, -n, n }); // Negative width for left-facing

	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { 0, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { 0, 0, -n, n });

// Walk mario low
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { (float)i * n, 4 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { (float)i * n, 4 * n, -n, n });


// i * n = 0  --- 4*n = 0
// 
// 
// 
// 0 ------------------ 0
// Walk mario big
	// Walk mario low

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT_BIG, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT_BIG, { (float)i * n, 6 * n, n, n });


	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT_LEFT, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT_LEFT, { (float)i * n, 6 * n, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT, { 2 * n, 5 * n, n, n });
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_RIGHT, { 3 * n, 5 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::FALLING_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT, { 2 * n, 5 * n, -n, n });
	sprite->AddKeyFrame((int)PlayerAnim::FALLING_LEFT, { 3 * n, 5 * n, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_RIGHT, { 0, 5 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::JUMPING_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::JUMPING_LEFT, { 0, 5 * n, -n, n });

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

	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_LEFT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_LEFT_BIG, { n * 2, n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::FLAG_RIGHT_BIG, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::FLAG_RIGHT_BIG, { n * 3, n, n, n });

	sprite->SetAnimation((int)PlayerAnim::IDLE_RIGHT);

	jumpSound = LoadSound("Assets/Audio/Fx/Jump.wav");
	dieSound = LoadSound("Assets/Audio/Fx/Die.wav");
	coinSound = LoadSound("Assets/Audio/Fx/Coin.wav");
	flagSound = LoadSound("Assets/Audio/Fx/Flagpole.wav");
	powerUpSound = LoadSound("Assets/Audio/Fx/Powerup.wav");
	powerUpStar = LoadSound("Assets/Audio/Fx/Powerup.wav");
	powerUpFlower = LoadSound("Assets/Audio/Fx/Powerup.wav");

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


// Getters & Setters
void Player::SetTileMap(TileMap* tilemap) { map = tilemap; }
bool Player::IsLookingRight() const { return look == Look::RIGHT; }
bool Player::IsLookingLeft() const { return look == Look::LEFT; }
bool Player::IsAscending() const { return dir.y < -PLAYER_LEVITATING_SPEED; }
bool Player::IsLevitating() const { return fabs(dir.y) <= PLAYER_LEVITATING_SPEED; }
bool Player::IsDescending() const { return dir.y > PLAYER_LEVITATING_SPEED; }
bool Player::IsInFirstHalfTile() const { return pos.y % TILE_SIZE < TILE_SIZE / 2; }
bool Player::IsInSecondHalfTile() const { return pos.y % TILE_SIZE >= TILE_SIZE / 2; }

void Player::SetAnimation(int id) { dynamic_cast<Sprite*>(render)->SetAnimation(id); }
PlayerAnim Player::GetAnimation() { return (PlayerAnim)dynamic_cast<Sprite*>(render)->GetAnimation(); }

// State Change & Anim
void Player::Stop()
{
	dir = { 0, 0 };
	state = State::IDLE;
	SetAnimationByState();
}

void Player::StartWalkingLeft()
{
	state = State::WALKING;
	look = Look::LEFT;
	SetAnimationByState();
}

void Player::StartWalkingRight()
{
	state = State::WALKING;
	look = Look::RIGHT;
	SetAnimationByState();
}

void Player::StartFalling()
{
	
	dir.y = PLAYER_SPEED * 1.5f;
	state = State::FALLING;
	SetAnimationByState();
}

void Player::StartJumping()
{
	dir.y = -PLAYER_JUMP_FORCE;
	state = State::JUMPING;
	jump_delay = PLAYER_JUMP_DELAY;
	SetAnimationByState();
	PlaySound(jumpSound);

}

void Player::StartClimbingUp()
{
	state = State::CLIMBING;
	SetAnimationByState();
	dynamic_cast<Sprite*>(render)->SetManualMode();
}

void Player::StartClimbingDown()
{
	state = State::CLIMBING;
	SetAnimationByState();
	dynamic_cast<Sprite*>(render)->SetManualMode();
}
void Player::Update()
{
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
		pos.x = 3153;
		if (pos.y > 191) {
			pos.y = 191;
		}

		if (state != State::FLAG) // Change to FLAG state only once
		{
			state = State::FLAG;
			if (IsLookingRight())
				SetAnimationByState();
			else
				SetAnimationByState();
		}

		// Play sound only once
		if (!flagSoundPlayed) {
			PlaySound(flagSound);
			flagSoundPlayed = true;
		}
	}
	else {
		if (state == State::FLAG)
		{
			pos.x = 3157;
			if (pos.y > 191) {
				pos.y = 191;
			}

			// Fix: assign state properly instead of comparing
			state = State::IDLE;
			PlayerAnim anim;
			
			if (IsLookingRight()) {
				if (isBigMario) {
					SetAnimation((int)PlayerAnim::FLAG_LEFT_BIG);
				}
				else {
					SetAnimation((int)PlayerAnim::FLAG_LEFT);
				}

			}
				


			else {
				if (isBigMario) {
					SetAnimation((int)PlayerAnim::FLAG_RIGHT_BIG);
				}
				else {
					SetAnimation((int)PlayerAnim::FLAG_RIGHT);
				}
			}


			walkingcastle = true;
		}
		
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
		//SetAnimationByState();
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

	//We can only go up and down while climbing
	if (state == State::CLIMBING)			return;

	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
	{
		if ((pos.x - current_speed) > 0) {
			pos.x += -current_speed;
			if (state == State::IDLE) StartWalkingLeft();
			else
			{
				if (IsLookingRight()) SetAnimationByState();
			}

			box = GetHitbox();
			if (map->TestCollisionWallLeft(box))
			{
				pos.x = prev_x;
				if (state == State::WALKING) Stop();
			}
		}
		else {
			Stop();
		}

	}
	else if (IsKeyDown(KEY_RIGHT)|| walkingcastle)
	{
		pos.x += current_speed;
		if (state == State::IDLE) StartWalkingRight();
		else
		{
			if (IsLookingLeft()) SetAnimationByState();
		}

		box = GetHitbox();
		if (map->TestCollisionWallRight(box))
		{
			pos.x = prev_x;
			if (state == State::WALKING) Stop();
		}

		if ((pos.x == 960 && pos.y == 447))
		{
			// Teleport to the specified position
			pos.x = 2614;
			pos.y = 175;
			teletransportation = false;
			Stop(); // Reset player state
			return;
		}

	}
	else
	{
		if (state == State::WALKING) Stop();
	}


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
					// Teleport to the specified position
					pos.x = 770+16;
					pos.y = 255;
					teletransportation = true;
					Stop(); // Reset player state
					return;
				}
				
				
				//To climb up the ladder, we need to check the control point (x, y)
				//To climb down the ladder, we need to check pixel below (x, y+1) instead
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

	//It is important to first check LadderTop due to its condition as a collision ground.
	//By doing so, we ensure that we don't stop climbing down immediately after starting the descent.
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
		LOG("Mushroom Power-Up Deactivated");
		isBigMario = false;
	}
	if (isFireMario) {
		LOG("Flower Power-Up Deactivated");
		isFireMario = false;
	}
	if (isStarMario) {
		LOG("Star Power-Up Deactivated");
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
	PlaySound(powerUpSound);
	LOG("Mushroom Power-Up Activated - Playing powerUpSound");
}

void Player::FlowerPower() {
	map->canBreak = true;
	isFireMario = true;
	PlaySound(powerUpFlower);
	LOG("Flower Power-Up Activated - Playing powerUpSound");
}

void Player::StarPower() {
	map->canBreak = true;
	isStarMario = true;
	PlaySound(powerUpStar);
	LOG("Star Power-Up Activated - Playing powerUpSound");
}

Vector2 Player::GetVelocity() const {
	return { (float)dir.x, (float)dir.y };
}
void Player::SetAnimationByState()
{
	PlayerAnim anim;
	
	if (look == Look::RIGHT)
	{
		switch (state)
		{
		case State::IDLE: anim = isBigMario ? PlayerAnim::IDLE_RIGHT_BIG : PlayerAnim::IDLE_RIGHT; break;
		case State::WALKING: anim = PlayerAnim::WALKING_RIGHT; break;
		case State::JUMPING: anim = PlayerAnim::JUMPING_RIGHT; break;
		case State::FALLING: anim = PlayerAnim::FALLING_RIGHT; break;
		case State::FLAG: anim = isBigMario ? PlayerAnim::FLAG_RIGHT_BIG : PlayerAnim::FLAG_RIGHT; break;
		default: return;
		}
	}
	else
	{
		switch (state)
		{
		case State::IDLE: anim = isBigMario ? PlayerAnim::IDLE_LEFT_BIG : PlayerAnim::IDLE_LEFT; break;
		case State::WALKING: anim = PlayerAnim::WALKING_LEFT; break;
		case State::JUMPING: anim = PlayerAnim::JUMPING_LEFT; break;
		case State::FALLING: anim = PlayerAnim::FALLING_LEFT; break;
		case State::FLAG: anim = isBigMario ? PlayerAnim::FLAG_LEFT_BIG : PlayerAnim::FLAG_LEFT; break;

		default: return;
		}
	}

	SetAnimation((int)anim);

	
	

}
void Player::ChangeColliderSize()
{
	if (isBigMario || isFireMario)
	{
		height = 32;
	}
	else
	{
		height = 16;
	}
}