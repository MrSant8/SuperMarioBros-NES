#include "Object.h"
#include "StaticImage.h"

Object::Object(const Point& p, ObjectType t) :
	Entity(p, OBJECT_PHYSICAL_SIZE, OBJECT_PHYSICAL_SIZE, OBJECT_FRAME_SIZE, OBJECT_FRAME_SIZE)
{
	type = t; 
	
	Rectangle rc;
	const int n = TILE_SIZE;
	switch (type)
	{
		case ObjectType::COIN: rc = {4*n, 3*n, n, n}; break;
		case ObjectType::MUSHROOM: rc = {5* n, 3*n, n, n}; break;
		case ObjectType::FLOWER: rc = { 6 * n, 3 * n, n, n }; break;
		case ObjectType::STAR: rc = { 7 * n, 3 * n, n, n }; break;

		default: LOG("Internal error: object creation of invalid type");
	}

	ResourceManager& data = ResourceManager::Instance();
	render = new StaticImage(data.GetTexture(Resource::IMG_TILES), rc);
}
Object::~Object()
{
}
void Object::DrawDebug(const Color& col) const
{
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
}
int Object::Points() const
{
	if (type == ObjectType::COIN)		return POINTS_COIN;
	else if (type == ObjectType::MUSHROOM)	return POINTS_POWER_UP;
	else if (type == ObjectType::FLOWER)	return POINTS_POWER_UP;
	else if (type == ObjectType::STAR)	return POINTS_POWER_UP;
}
