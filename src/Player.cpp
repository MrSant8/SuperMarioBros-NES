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
	state(s), look(view), jump_delay(PLAYER_JUMP_DELAY), map(nullptr), score(0) {}

Player::~Player() {}

// Inicialización
AppStatus Player::Initialise()
{
	const int n = PLAYER_FRAME_SIZE;
	ResourceManager& data = ResourceManager::Instance();

	if (data.LoadTexture(Resource::IMG_PLAYER, "images/eric.png") != AppStatus::OK)
	{
		LOG("Failed to load player sprite texture");
		return AppStatus::ERROR;
	}

	render = new Sprite(data.GetTexture(Resource::IMG_PLAYER));
	if (!render)
	{
		LOG("Failed to allocate memory for player sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)PlayerAnim::NUM_ANIMATIONS);

	// Animaciones básicas
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { 0, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { 0, 0, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { (float)i * n, 4 * n, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
	for (int i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { (float)i * n, 4 * n, -n, n });

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

	sprite->SetAnimation((int)PlayerAnim::IDLE_RIGHT);

	jumpSound = LoadSound("Assets/Audio/Fx/Jump.wav");
	dieSound = LoadSound("Assets/Audio/Fx/Die.wav");

	return AppStatus::OK;
}

// Score y tiempo
void Player::InitScore() { score = 0; }
void Player::IncrScore(int n) { score += n; }
int Player::GetScore() { return score; }

int Player::GetTime()
{
	timeCounter += GetFrameTime();
	if (timeCounter >= 1.0f)
	{
		time = std::max(0, time - 1);
		timeCounter = 0.0f;
	}
	return time;
}

// Getters y Setters
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

// Cambios de estado y animación
void Player::Stop()
{
	dir = { 0, 0 };
	state = State::IDLE;
	SetAnimation(IsLookingRight() ? (int)PlayerAnim::IDLE_RIGHT : (int)PlayerAnim::IDLE_LEFT);
}

void Player::StartWalkingLeft()
{
	state = State::WALKING;
	look = Look::LEFT;
	SetAnimation((int)PlayerAnim::WALKING_LEFT);
}

void Player::StartWalkingRight()
{
	state = State::WALKING;
	look = Look::RIGHT;
	SetAnimation((int)PlayerAnim::WALKING_RIGHT);
}

void Player::StartFalling()
{
	
	dir.y = PLAYER_SPEED * 1.5f;
	state = State::FALLING;
	SetAnimation(IsLookingRight() ? (int)PlayerAnim::FALLING_RIGHT : (int)PlayerAnim::FALLING_LEFT);
}

void Player::StartJumping()
{
	dir.y = -PLAYER_JUMP_FORCE;
	state = State::JUMPING;
	jump_delay = PLAYER_JUMP_DELAY;
	SetAnimation(IsLookingRight() ? (int)PlayerAnim::JUMPING_RIGHT : (int)PlayerAnim::JUMPING_LEFT);
	PlaySound(jumpSound);

}

void Player::StartClimbingUp()
{
	state = State::CLIMBING;
	SetAnimation((int)PlayerAnim::CLIMBING);
	dynamic_cast<Sprite*>(render)->SetManualMode();
}

void Player::StartClimbingDown()
{
	state = State::CLIMBING;
	SetAnimation((int)PlayerAnim::CLIMBING_TOP);
	dynamic_cast<Sprite*>(render)->SetManualMode();
}
void Player::ChangeAnimRight()
{
	look = Look::RIGHT;
	switch (state)
	{
	case State::IDLE:	SetAnimation((int)PlayerAnim::IDLE_RIGHT); break;
	case State::WALKING:	SetAnimation((int)PlayerAnim::WALKING_RIGHT); break;
	case State::JUMPING:	SetAnimation((int)PlayerAnim::JUMPING_RIGHT); break;
	case State::FALLING:	SetAnimation((int)PlayerAnim::FALLING_RIGHT); break;
	}
}

void Player::ChangeAnimLeft()
{
	look = Look::LEFT;
	switch (state)
	{
	case State::IDLE:	SetAnimation((int)PlayerAnim::IDLE_LEFT); break;
	case State::WALKING:	SetAnimation((int)PlayerAnim::WALKING_LEFT); break;
	case State::JUMPING:	SetAnimation((int)PlayerAnim::JUMPING_LEFT); break;
	case State::FALLING:	SetAnimation((int)PlayerAnim::FALLING_LEFT); break;
	}
}
void Player::Update()
{
	if (state == State::DEAD)
	{
		pos.y += dir.y;
		dir.y += GRAVITY_FORCE;
		dynamic_cast<Sprite*>(render)->Update();
		playerisDead = true;
		return;
	}
	MoveX();
	MoveY();



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
				if (IsLookingRight()) ChangeAnimLeft();
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
	else if (IsKeyDown(KEY_RIGHT))
	{
		pos.x += current_speed;
		if (state == State::IDLE) StartWalkingRight();
		else
		{
			if (IsLookingLeft()) ChangeAnimRight();
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
			pos.x = 922;
			pos.y = 143;
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
			pos.y += 16;
			StartFalling(); // Stop jump and start falling

			AABB hitbox = GetHitbox();
			Point tileHit;
			tilemap->ActivateLaserAnimation(tileHit.x, tileHit.y);

			/*if (tilemap->TestCollisionFromBelow(hitbox, &hitbox.pos.y, &tileHit))
			{
			}*/

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
				if (IsLookingRight())	SetAnimation((int)PlayerAnim::JUMPING_RIGHT);
				else					SetAnimation((int)PlayerAnim::JUMPING_LEFT);
			}
			else if (IsLevitating())
			{
				if (IsLookingRight())	SetAnimation((int)PlayerAnim::LEVITATING_RIGHT);
				else					SetAnimation((int)PlayerAnim::LEVITATING_LEFT);
			}
			else if (IsDescending())
			{
				if (IsLookingRight())	SetAnimation((int)PlayerAnim::FALLING_RIGHT);
				else					SetAnimation((int)PlayerAnim::FALLING_LEFT);
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
		if (IsInSecondHalfTile())		SetAnimation((int)PlayerAnim::CLIMBING_PRE_TOP);
		else if (IsInFirstHalfTile())	SetAnimation((int)PlayerAnim::CLIMBING_TOP);
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
		if (GetAnimation() != PlayerAnim::CLIMBING)	SetAnimation((int)PlayerAnim::CLIMBING);
	}
}
void Player::Die()
{
	state = State::DEAD;
	SetAnimation((int)PlayerAnim::DEAD);
	dir = { 0, -PLAYER_JUMP_FORCE }; 
	PlaySound(dieSound);
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