#pragma once
#include <raylib.h>
#include "Sprite.h"
#include "Point.h"
#include "AABB.h"
#include "Globals.h"
#include <unordered_map>
#include <vector>

struct BlockFragment {
	float x, y, vx, vy;
	int lifetime;
};

enum class Tile {

	// -1: area covered by entity
	EMPTY = -1,
	//  0: air tile
	AIR = 0,

	// 0 < id < 50: static tiles
	BLOCK_SQUARE1_TL = 1, BLOCK_SQUARE1_TR, BLOCK_SQUARE1_BL, BLOCK_SQUARE1_BR,
	BLOCK_SQUARE2_TL, BLOCK_SQUARE2_TR, BLOCK_SQUARE2_BL, BLOCK_SQUARE2_BR,
	BLOCK_VERT2_T, BLOCK_VERT2_B, BLOCK_HORIZ2_L, BLOCK_HORIZ2_R, BLOCK_BLUE,
	BLOCK_HORIZ3_L, BLOCK_HORIZ3_M, BLOCK_HORIZ3_R,
	BLOCK_BEAM_L, BLOCK_BEAM_R,
	
	LADDER_L = 20, LADDER_R, LADDER_TOP_L, LADDER_TOP_R,
	LOCK_RED = 30, LOCK_YELLOW, 
	LASER_L = 40, LASER_R,

	// 50 <= id < 100: special tiles
	DOOR = 50,
	KEY_RED = 60, YELLOW_KEY, ITEM_APPLE, ITEM_CHILI,
	LASER = 70, LASER_FRAME0, LASER_FRAME1, LASER_FRAME2,

	// id >= 100: entities' initial locations
	PLAYER = 100,
	GOOMBA_TILE = 200,
	KOOPA_TILE = 300,
	TURRET_LEFT = 301, TURRET_RIGHT = 302,

	//Intervals
	SOLID_FIRST = BLOCK_SQUARE1_TR,
	SOLID_LAST = BLOCK_BEAM_R,
	OBJECT_FIRST = KEY_RED,
	OBJECT_LAST = ITEM_CHILI,
	ENTITY_FIRST = PLAYER,
	ENTITY_LAST = TURRET_RIGHT
};

class TileMap
{
public:
	TileMap();
	~TileMap();

	AppStatus Initialise();
	AppStatus Load(int data[], int w, int h);
	void ClearObjectEntityPositions();

	void Update();
	void Render();
	void Blocksorprise();
	void Blockone();
	void Release();

	bool IsTileObject(Tile tile) const;
	bool IsTileEntity(Tile tile) const;

	//Test for collisions with walls
	bool TestCollisionWallLeft(const AABB& box) const;
	bool TestCollisionWallRight(const AABB& box) const;
	
	//Test collision with the ground and update 'py' with the maximum y-position to prevent
	//penetration of the grounded tile, that is, the pixel y-position above the grounded tile.
	//Grounded tile = solid tile (blocks) or ladder tops.
	bool TestCollisionGround(const AABB& box, int *py) const;

	//Test if there is a ground tile one pixel below the given box
	bool TestFalling(const AABB& box) const;
	
	//Test if box is on ladder and update 'px' with the x-center position of the ladder
	bool TestOnLadder(const AABB& box, int* px) const;
	
	//Test if box is on ladder top and update 'px' with the x-center position of the ladder
	bool TestOnLadderTop(const AABB& box, int* px) const;

	//Given a hitbox, computes the maximum swept box model along the X-axis without solid tiles
	AABB GetSweptAreaX(const AABB& hitboxbox) const;

	bool CollisionY(const Point& p, int distance) const;

	bool TestCollisionFromBelow(const AABB& box, int* py, Point* collisionTilePos);

private:
	void InitTileDictionary();

	Tile GetTileIndex(int x, int y) const;
	bool IsTileSolid(Tile tile) const;
	bool IsTileLadderTop(Tile tile) const;
	bool IsTileLadder(Tile tile) const;
	bool IsTileSorprise_block(Tile tile) const;
	bool CollisionX(const Point& p, int distance) const;
	int GetLadderCenterPos(int pixel_x, int pixel_y) const;
	//Tile map
	Tile *map;

	//Size of the tile map
	int size, width, height;
	
	//Dictionary of tile frames
	std::unordered_map<int, Rectangle> dict_rect;

	Sprite * surpriseBlock;
	
	//Tile sheet
	const Texture2D *img_tiles;
	Vector2 laserPos;

	bool laserActive;
	bool BlockActive;
	int laserX, laserY;
	Vector2 Laser;

	float laserYOffset = 0.0f;
	bool dropLaser = false;
	bool dropBlock = false;
	int laserTimer = 15;

	int surpriseBlockTimer = 5;



	float bajarOffset = 0.0f;
	const float bajarVelocidad = 2.0f;  // píxeles por frame (ajustá esto para más o menos rapidez)


	float dropSpeed = 2.0f;  // Velocidad de caída en píxeles/frame
	float dropYOffset = 0.0f; // Offset temporal Y para el bloque que cae
	bool isDropping = false; // Si está cayendo
	bool romperse;

	std::vector<BlockFragment> fragments;

public:
	bool changeBlock = true;
	int laserTileX, laserTileY;

	bool LowerFlag = false;
	bool flagFullyLowered = false;
	bool bigMario = false;

};

