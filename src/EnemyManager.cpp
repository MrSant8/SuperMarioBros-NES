	#include "EnemyManager.h"
#include "Slime.h"
#include "Turret.h"

SlimeLimits slime_limits[] = {
	{288, 416, 208},
};


EnemyManager::EnemyManager()
{
	shots = nullptr;
}
EnemyManager::~EnemyManager()
{
	Release();
	UnloadSound(squishSound);
}
AppStatus EnemyManager::Initialise()
{
	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_ENEMIES, "images/enemies.png") != AppStatus::OK)
	{
		LOG("Failed to load enemies sprite texture");
		return AppStatus::ERROR;
	}

	//Load sound effects
	squishSound = LoadSound("Assets/Audio/Fx/Squish.wav");
	if (squishSound.stream.buffer == nullptr)
	{
		LOG("Failed to load squish sound");
		return AppStatus::ERROR;
	}

	return AppStatus::OK;
}
void EnemyManager::SetShotManager(ShotManager* shots)
{
	this->shots = shots;
}
void EnemyManager::Add(const Point& pos, EnemyType type, const AABB& area, Look look)
{
	Enemy *enemy;
	
	if(type == EnemyType::SLIME)
	{
		enemy = new Slime(pos, SLIME_PHYSICAL_WIDTH, SLIME_PHYSICAL_HEIGHT, SLIME_FRAME_SIZE, SLIME_FRAME_SIZE);
		int slimeIndex = enemies.size();
		dynamic_cast<Slime*>(enemy)->SetLimits(slime_limits[slimeIndex].left,
			slime_limits[slimeIndex].right,
			slime_limits[slimeIndex].floor);
	}
	else if(type == EnemyType::TURRET)
	{
		enemy = new Turret(pos, TURRET_PHYSICAL_WIDTH, TURRET_PHYSICAL_HEIGHT, TURRET_FRAME_SIZE, TURRET_FRAME_SIZE);
	}
	else
	{
		LOG("Internal error: trying to add a new enemy with invalid type");
		return;
	}
		
	enemy->Initialise(look, area);
	enemies.push_back(enemy);
	updatelist();
}
AABB EnemyManager::GetEnemyHitBox(const Point& pos, EnemyType type) const
{
	int width, height;
	if (type == EnemyType::SLIME)
	{
		width = SLIME_PHYSICAL_WIDTH;
		height = SLIME_PHYSICAL_HEIGHT;
	}
	else if (type == EnemyType::TURRET)
	{
		width = TURRET_PHYSICAL_WIDTH;
		height = TURRET_PHYSICAL_HEIGHT;
	}
	else
	{
		LOG("Internal error while computing hitbox for an invalid enemy type");
		return {};
	}
	Point p(pos.x, pos.y - (height - 1));
	AABB hitbox(p, width, height);
	return hitbox;
}
void EnemyManager::Update(const AABB& player_hitbox)
{
	bool shoot;

	for (auto it = enemies.begin(); it != enemies.end(); )
	{
		Enemy* enemy = *it;

		// Si est� muerto, lo eliminamos del vector y de memoria
		if (enemy->isDead()) {
			delete enemy;
			it = enemies.erase(it);
			continue;
		}

		// Si no est� muerto, lo actualizamos
		shoot = enemy->Update(player_hitbox);

		++it;
	}
	
}
void EnemyManager::Draw() const
{
	for (const Enemy* enemy : enemies)
		enemy->Draw();
}
void EnemyManager::DrawDebug() const
{
	for (const Enemy* enemy : enemies)
	{	
		enemy->DrawVisibilityArea(DARKGRAY);
		enemy->DrawHitbox(RED);
	}
}
void EnemyManager::Release()
{
 	for (Enemy* enemy : enemies)
		delete enemy;
	enemies.clear();
}

void EnemyManager::CheckPlayerCollision(const AABB& playerHitbox, const Point& playerDir)
{
	updatelist();

	for (auto it = enemies.begin(); it != enemies.end(); )
	{
		Enemy* enemy = *it;
		AABB enemyHitbox = enemy->GetHitbox();
		Rectangle playerRect = { (float)playerHitbox.pos.x, (float)playerHitbox.pos.y, (float)playerHitbox.width, (float)playerHitbox.height };
		Rectangle enemyRect = { (float)enemyHitbox.pos.x, (float)enemyHitbox.pos.y, (float)enemyHitbox.width, (float)enemyHitbox.height };

		// Check if there is a collision
		if (CheckCollisionRecs(playerRect, enemyRect))
		{
			// If player is above the enemy and moving downward, kill the enemy
  			if (playerHitbox.pos.y + playerHitbox.height < (enemyHitbox.pos.y-1) + enemyHitbox.height)
			{
				if (enemy->StateSlime())
				{
					PlaySound(squishSound);
					enemy->isDead();
    				delete enemy;
					it = enemies.erase(it);
					continue;
				}
			}
			else
			{
				playerDead = true;
				// Player collided with enemy from sides or below - player dies
				// You'll need to implement player death logic here
				LOG("Player died from enemy collision");
			}
		}
		++it;
	}
}