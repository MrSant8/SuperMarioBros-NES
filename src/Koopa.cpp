#include "Koopa.h"
#include "Sprite.h"

Koopa::Koopa(const Point& p, int width, int height, int frame_width, int frame_height) :
	Enemy(p, width, height, frame_width, frame_height)
{
	attack_delay = 0;
	state = KoopaState::ROAMING;

	current_step = 0;
	current_frames = 0;
}

Koopa::~Koopa()
{}

AppStatus Koopa::Initialise(Look look, const AABB& area)
{
	int i;
	const int n = KOOPA_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	render = new Sprite(data.GetTexture(Resource::IMG_ENEMIES));

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)KoopaAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)KoopaAnim::IDLE_RIGHT, KOOPA_ANIM_DELAY);
	sprite->AddKeyFrame((int)KoopaAnim::IDLE_RIGHT, { 0, 2 * n, n, n });
	sprite->SetAnimationDelay((int)KoopaAnim::IDLE_LEFT, KOOPA_ANIM_DELAY);
	sprite->AddKeyFrame((int)KoopaAnim::IDLE_LEFT, { 0, 2 * n, -n, n });

	sprite->SetAnimationDelay((int)KoopaAnim::WALKING_RIGHT, KOOPA_ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)KoopaAnim::WALKING_RIGHT, { (float)i * n, 2 * n, n, n });
	sprite->SetAnimationDelay((int)KoopaAnim::WALKING_LEFT, KOOPA_ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)KoopaAnim::WALKING_LEFT, { (float)i * n, 2 * n, -n, n });
	sprite->AddKeyFrame((int)KoopaAnim::DEAD, { (float)i * n, 3 * n, n, n });

	this->look = look;
	if (look == Look::LEFT)        sprite->SetAnimation((int)KoopaAnim::IDLE_LEFT);
	else if (look == Look::RIGHT) sprite->SetAnimation((int)KoopaAnim::IDLE_RIGHT);

	visibility_area = area;

	InitPattern();

	return AppStatus::OK;
}
void Koopa::InitPattern()
{
	const int n = KOOPA_ANIM_DELAY * 3;

	pattern.push_back({ {0, 0}, 2 * n, (int)KoopaAnim::IDLE_RIGHT });
	pattern.push_back({ {KOOPA_SPEED, 0}, n, (int)KoopaAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)KoopaAnim::IDLE_RIGHT });
	pattern.push_back({ {KOOPA_SPEED, 0}, n, (int)KoopaAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)KoopaAnim::IDLE_RIGHT });

	pattern.push_back({ {0, 0}, 2 * n, (int)KoopaAnim::IDLE_LEFT });
	pattern.push_back({ {-KOOPA_SPEED, 0}, n, (int)KoopaAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)KoopaAnim::IDLE_LEFT });
	pattern.push_back({ {-KOOPA_SPEED, 0}, n, (int)KoopaAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)KoopaAnim::IDLE_LEFT });

	current_step = 0;
	current_frames = 0;
}
bool Koopa::Update(const AABB& box)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	int anim_id;
	switch (state)
	{
	case KoopaState::ROAMING:
		if (IsVisible(box))
		{
			if (look == Look::LEFT)	sprite->SetAnimation((int)KoopaAnim::WALKING_LEFT);
			else					sprite->SetAnimation((int)KoopaAnim::WALKING_RIGHT);
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
			sprite->SetAnimation((int)KoopaAnim::WALKING_RIGHT);
		}
		else if (pos.x >= limit_right) {
			look = Look::LEFT;
			sprite->SetAnimation((int)KoopaAnim::WALKING_LEFT);
		}
		break;
	case KoopaState::SHELL:
		break;
	case KoopaState::MOVING_SHELL:
		break;
	case KoopaState::DEAD:
		if (isDead())
		{
			sprite->SetAnimation((int)KoopaAnim::DEAD);
			return false;
		}
		break;
	}

	sprite->Update();

	return shoot;

}

void Koopa::GetShootingPosDir(Point* pos, Point* dir) const
{
}

void Koopa::SetLimits(int left, int right, float floor)
{
	limit_left = left;
	limit_right = right;
	position_foor = floor;
}
void Koopa::UpdateLook(int anim_id)
{
	KoopaAnim anim = (KoopaAnim)anim_id;
	look = (anim == KoopaAnim::IDLE_LEFT ||
		anim == KoopaAnim::WALKING_LEFT ||
		anim == KoopaAnim::WALKING_LEFT) ? Look::LEFT : Look::RIGHT;
}
