#pragma once
#include "Entity.h"

#define OBJECT_PHYSICAL_SIZE	14
#define OBJECT_FRAME_SIZE		16

#define POINTS_COIN	10
#define POINTS_POWER_UP	50


enum class ObjectType { COIN, MUSHROOM, FLOWER, STAR };

class Object : public Entity
{
public:
	Object(const Point& p, ObjectType t);
	~Object();

	void DrawDebug(const Color& col) const;

	int Points() const;

	ObjectType GetType() const { return type; }
private:
	ObjectType type;
};

