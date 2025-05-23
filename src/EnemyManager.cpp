#include "EnemyManager.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Turret.h"
#include "Player.h"

GoombaLimits goombalimits[] = {
	
	{1136, 1360, 208},
	{1136, 1360, 208},

	{288, 416, 208},
	{646, 720, 208},
	
	
	{768, 896, 208},
	{768, 896, 208},

	{1440, 1632, 208},
	{1440, 1632, 208},

	{1824, 1920, 208},
	{1824, 1920, 208},

	{1952, 2144-32, 208},
	{1952, 2144-32, 208},
	{1952, 2144-32, 208},

	{2768-112, 2972- 160, 208},
	{2768- 112, 2972- 160, 208},
};
KoopaLimits koopalimits[] = {
	{1428, 1794, 208}, 
	
};


EnemyManager::EnemyManager()
{
	shots = nullptr;
}
EnemyManager::~EnemyManager()
{
	Release();
}
AppStatus EnemyManager::Initialise()
{
	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_ENEMIES, "images/enemies.png") != AppStatus::OK)
	{
		LOG("Failed to load enemies sprite texture");
		return AppStatus::ERROR;
	}

	if (data.LoadTexture(Resource::IMG_ENEMIES_KOOPA, "images/enemies_KOOPA.png") != AppStatus::OK)
	{
		LOG("Failed to load enemies sprite texture");
		return AppStatus::ERROR;
	}



	deadenemySound = LoadSound("Assets/Audio/Fx/Squish.wav");
	marioFont = LoadFont("Assets/Font/super-mario-bros-nes.ttf");


	return AppStatus::OK;
}
void EnemyManager::SetShotManager(ShotManager* shots)
{
	this->shots = shots;
}
void EnemyManager::Add(const Point& pos, EnemyType type, const AABB& area, Look look)
{
	Enemy *enemy;
	
	if(type == EnemyType::GOOMBA)
	{
		enemy = new Goomba(pos, GOOMBA_PHYSICAL_WIDTH, GOOMBA_PHYSICAL_HEIGHT, GOOMBA_FRAME_SIZE, GOOMBA_FRAME_SIZE);
		int slimeIndex = enemies.size();
		dynamic_cast<Goomba*>(enemy)->SetLimits(goombalimits[slimeIndex].left,
			goombalimits[slimeIndex].right,
			goombalimits[slimeIndex].floor);
	}
	else if(type == EnemyType::TURRET)
	{
		enemy = new Turret(pos, TURRET_PHYSICAL_WIDTH, TURRET_PHYSICAL_HEIGHT, TURRET_FRAME_SIZE, TURRET_FRAME_SIZE);
	}
	else if (type == EnemyType::KOOPA)
	{
		enemy = new Koopa(pos, KOOPA_PHYSICAL_WIDTH, KOOPA_PHYSICAL_HEIGHT, KOOPA_FRAME_SIZE, KOOPA_FRAME_SIZE);
		int koopaIndex = enemies.size();
		dynamic_cast<Koopa*>(enemy)->SetLimits(koopalimits[koopaIndex].left,
			koopalimits[koopaIndex].right,
			koopalimits[koopaIndex].floor);
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
	if (type == EnemyType::GOOMBA)
	{
		width = GOOMBA_PHYSICAL_WIDTH;
		height = GOOMBA_PHYSICAL_HEIGHT;
	}
	else if (type == EnemyType::TURRET)
	{
		width = TURRET_PHYSICAL_WIDTH;
		height = TURRET_PHYSICAL_HEIGHT;
	}
	else if (type == EnemyType::KOOPA)
	{
		width = KOOPA_PHYSICAL_WIDTH;
		height = KOOPA_PHYSICAL_HEIGHT;
	}

	Point p(pos.x, pos.y - (height - 1));
	AABB hitbox(p, width, height);
	return hitbox;
}
void EnemyManager::Update(const AABB& player_hitbox)
{
	for (auto& score : floatingScores) {
		score.y -= 0.5f; // Movimiento hacia arriba
		score.lifetime--;
	}

	// Eliminar los numeros
	floatingScores.erase(std::remove_if(floatingScores.begin(), floatingScores.end(), [](FloatingScore& s) {
		return s.lifetime <= 0;
		}), floatingScores.end());

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

	for (const auto& score : floatingScores) {
		DrawTextEx(marioFont, TextFormat("%d", 100), { score.x, score.y }, 9, 1, WHITE);
	}
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

void EnemyManager::CheckPlayerCollision(const AABB& playerHitbox, const Point& playerDir, Player* player)
{
	updatelist();

	for (auto it = enemies.begin(); it != enemies.end(); )
	{
		Enemy* enemy = *it;

		AABB enemyHitbox = enemy->GetHitbox();
		Rectangle playerRect = { (float)playerHitbox.pos.x, (float)playerHitbox.pos.y, (float)playerHitbox.width, (float)playerHitbox.height };
		Rectangle enemyRect = { (float)enemyHitbox.pos.x, (float)enemyHitbox.pos.y, (float)enemyHitbox.width, (float)enemyHitbox.height };

		if (CheckCollisionRecs(playerRect, enemyRect) && !player->deathStarted)
		{
			float playerBottom = playerHitbox.pos.y + playerHitbox.height;
			float enemyTop = enemyHitbox.pos.y;
			float verticalDifference = playerBottom - enemyTop;

			bool hitFromAbove = (verticalDifference >= 0 && verticalDifference < 10 && player->GetVelocity().y > 0);

			if (hitFromAbove || player->isStarMario)
			{
				if (!player->isStarMario)
					player->Bounce();

				FloatingScore score;
				score.x = enemy->GetHitbox().pos.x;
				score.y = enemy->GetHitbox().pos.y - 10;
				score.lifetime = 30;
				score.value = 100;
				floatingScores.push_back(score);

				player->IncrScore(100);

				if (enemy->StateGoomba() || enemy->StateKoopa())
				{
					enemy->isDead(); 
					PlaySound(deadenemySound);
					delete enemy;
					it = enemies.erase(it);
					continue;
				}
			}
			else if (!player->IsInvincible())
			{
				if (player->isFireMario) {
					PlaySound(player->pipeSound);
					player->isFireMario = false;
					player->isBigMario = true;
					player->SetInvincible(2.0f); 
				}
				else if (player->isBigMario) {
					player->isBigMario = false;
					PlaySound(player->pipeSound);
					player->SetInvincible(2.0f);
				}
				else {
					player->StartDeath();
				}
			}

		}
		++it;
	}
}
