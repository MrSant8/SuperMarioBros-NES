#include "Goomba.h"
#include "Sprite.h"


Goomba::Goomba(const Point& p, int width, int height, int frame_width, int frame_height) :
	Enemy(p, width, height, frame_width, frame_height)
{
	attack_delay = 0;
	state = GoombaState::ROAMING;

	current_step = 0;
	current_frames = 0;
}
Goomba::~Goomba()
{
}
AppStatus Goomba::Initialise(Look look, const AABB& area)
{
	int i;
	const int n = GOOMBA_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	render = new Sprite(data.GetTexture(Resource::IMG_ENEMIES));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for slime sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)GoombaAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)GoombaAnim::IDLE_RIGHT, GOOMBA_ANIM_DELAY);
	sprite->AddKeyFrame((int)GoombaAnim::IDLE_RIGHT, { 0, 2 * n, n, n });
	sprite->SetAnimationDelay((int)GoombaAnim::IDLE_LEFT, GOOMBA_ANIM_DELAY);
	sprite->AddKeyFrame((int)GoombaAnim::IDLE_LEFT, { 0, 2 * n, -n, n });

	sprite->SetAnimationDelay((int)GoombaAnim::WALKING_RIGHT, GOOMBA_ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)GoombaAnim::WALKING_RIGHT, { (float)i * n, 2 * n, n, n });
	sprite->SetAnimationDelay((int)GoombaAnim::WALKING_LEFT, GOOMBA_ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)GoombaAnim::WALKING_LEFT, { (float)i * n, 2 * n, -n, n });
	sprite->AddKeyFrame((int)GoombaAnim::DEAD, { 2 * n, 3 * n, n, n });

	this->look = look;
	if (look == Look::LEFT)        sprite->SetAnimation((int)GoombaAnim::IDLE_LEFT);
	else if (look == Look::RIGHT) sprite->SetAnimation((int)GoombaAnim::IDLE_RIGHT);

	visibility_area = area;

	InitPattern();

	return AppStatus::OK;
}
void Goomba::InitPattern()
{
	//Multiplying by 3 ensures sufficient time for displaying all 3 frames of the
	//walking animation, resulting in smoother transitions and preventing the animation
	//from appearing rushed or incomplete
	const int n = GOOMBA_ANIM_DELAY * 3;

	pattern.push_back({ {0, 0}, 2 * n, (int)GoombaAnim::IDLE_RIGHT });
	pattern.push_back({ {GOOMBA_SPEED, 0}, n, (int)GoombaAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)GoombaAnim::IDLE_RIGHT });
	pattern.push_back({ {GOOMBA_SPEED, 0}, n, (int)GoombaAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)GoombaAnim::IDLE_RIGHT });

	pattern.push_back({ {0, 0}, 2 * n, (int)GoombaAnim::IDLE_LEFT });
	pattern.push_back({ {-GOOMBA_SPEED, 0}, n, (int)GoombaAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)GoombaAnim::IDLE_LEFT });
	pattern.push_back({ {-GOOMBA_SPEED, 0}, n, (int)GoombaAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)GoombaAnim::IDLE_LEFT });

	current_step = 0;
	current_frames = 0;
}

void Goomba::SetLimits(int left, int right, float floor)
{
	limit_left = left;
	limit_right = right;
	position_foor = floor;
}

bool Goomba::Update(const AABB& box)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	int anim_id;
	switch (state)
	{
	case GoombaState::ROAMING:
		if (IsVisible(box))
		{
			if (look == Look::LEFT)	sprite->SetAnimation((int)GoombaAnim::WALKING_LEFT);
			else					sprite->SetAnimation((int)GoombaAnim::WALKING_RIGHT);
		}

		if (pos.y == position_foor) {
			pos.y = position_foor; 
			if (look == Look::LEFT)
			{
				pos.x -= speed;
			}
			else {
				pos.x += speed;
			}
		}
		else {
			pos.y++;
		}
		if (pos.x <= limit_left) {
			look = Look::RIGHT;
			sprite->SetAnimation((int)GoombaAnim::WALKING_RIGHT);
		}
		else if (pos.x >= limit_right) {
			look = Look::LEFT;
			sprite->SetAnimation((int)GoombaAnim::WALKING_LEFT);
		}
		break;
	case GoombaState::ATTACK:
		break;
	}
	if (isDead())
		{
			sprite->SetAnimation((int)GoombaAnim::DEAD);
			return false;
		}
		sprite->Update();
		return shoot;
	
}
void Goomba::UpdateLook(int anim_id)
{
	GoombaAnim anim = (GoombaAnim)anim_id;
	look = (anim == GoombaAnim::IDLE_LEFT ||
		anim == GoombaAnim::WALKING_LEFT ||
		anim == GoombaAnim::WALKING_LEFT) ? Look::LEFT : Look::RIGHT;
}
void Goomba::GetShootingPosDir(Point* p, Point* d) const
{
	/*if (look == Look::LEFT)
	{
		p->x = pos.x + SLIME_SHOT_OFFSET_X_LEFT;
		*d = { -SLIME_SHOT_SPEED, 0 };
	}
	else
	{
		p->x = pos.x + SLIME_SHOT_OFFSET_X_RIGHT;
		*d = { SLIME_SHOT_SPEED, 0 };
	}
	p->y = pos.y + SLIME_SHOT_OFFSET_Y;*/
}