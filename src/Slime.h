#pragma once
#include "Enemy.h"

#define SLIME_SPEED			1 // Aseg�rate de que esto sea un valor flotante (float)
#define SLIME_ANIM_DELAY	(4*ANIM_DELAY)

#define SLIME_SHOT_OFFSET_X_LEFT	-14
#define SLIME_SHOT_OFFSET_X_RIGHT	 26
#define SLIME_SHOT_OFFSET_Y			-22

#define SLIME_SHOT_SPEED	4

#define GRAVITY_FORCE  1


enum class SlimeState { ROAMING, ATTACK };
enum class SlimeAnim {
	IDLE_LEFT, IDLE_RIGHT, WALKING_LEFT, WALKING_RIGHT,
	ATTACK_LEFT, ATTACK_RIGHT, NUM_ANIMATIONS
};

struct Step
{
	Point speed;	// Direcci�n de movimiento
	int frames;		// Duraci�n en n�mero de frames
	int anim;		// Representaci�n gr�fica
};

struct SlimeLimits {
	int left;      // Limite izquierdo
	int right;     // Limite derecho
	float floor;   // El nivel de piso
};

extern SlimeLimits slime_limits[];

class Slime : public Enemy
{
public:
	Slime(const Point& p, int width, int height, int frame_width, int frame_height);
	~Slime();

	// Inicializa al enemigo con la direcci�n y �rea especificada
	AppStatus Initialise(Look look, const AABB& area) override;

	// Actualiza la l�gica del enemigo, devuelve true si el enemigo debe disparar
	bool Update(const AABB& box) override;

	// Recupera la posici�n y direcci�n del disparo
	void GetShootingPosDir(Point* pos, Point* dir) const override;

	void SetLimits(int left, int right, float floor);

private:
	// Crea el patr�n de comportamiento
	void InitPattern();

	// Actualiza la direcci�n de la vista seg�n el paso actual del patr�n
	void UpdateLook(int anim_id);

	int attack_delay;	// Retraso entre ataques
	SlimeState state;

	int current_step;	// Paso actual del patr�n
	int current_frames;	// N�mero de frames en el paso actual
	std::vector<Step> pattern;
	int limit_left;
	int limit_right;

	float vertical_speed = 0;
	float MAX_FALL_SPEED = 5.0f;
	float position_foor = 208;
};
