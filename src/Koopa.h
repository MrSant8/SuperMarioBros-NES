#pragma once
#include "Enemy.h"

#define KOOPA_SPEED			1 // Asegúrate de que esto sea un valor flotante (float)
#define KOOPA_ANIM_DELAY	(4*ANIM_DELAY)

#define KOOPA_SHOT_OFFSET_X_LEFT	-14
#define KOOPA_SHOT_OFFSET_X_RIGHT	 26
#define KOOPA_SHOT_OFFSET_Y			-22

#define KOOPA_SHOT_SPEED	4

#define GRAVITY_FORCE  1


enum class KoopaState { ROAMING, SHELL, MOVING_SHELL, DEAD };

enum class KoopaAnim {
	IDLE_LEFT, IDLE_RIGHT,
	WALKING_LEFT, WALKING_RIGHT,
	SHELL, MOVING_SHELL,
	DEAD,
	NUM_ANIMATIONS
};

struct KoopaStep
{
	Point speed;	// Dirección de movimiento
	int frames;		// Duración en número de frames
	int anim;		// Representación gráfica
};

struct KoopaLimits {
	int left;      // Limite izquierdo
	int right;     // Limite derecho
	float floor;   // El nivel de piso
};

extern KoopaLimits koopalimits[];

class Koopa : public Enemy
{
public:
	Koopa(const Point& p, int width, int height, int frame_width, int frame_height);
	~Koopa();

	// Inicializa al enemigo con la dirección y área especificada
	AppStatus Initialise(Look look, const AABB& area) override;

	// Actualiza la lógica del enemigo, devuelve true si el enemigo debe disparar
	bool Update(const AABB& box) override;

	// Recupera la posición y dirección del disparo
	void GetShootingPosDir(Point* pos, Point* dir) const override;

	void SetLimits(int left, int right, float floor);

private:
	// Crea el patrón de comportamiento
	void InitPattern();

	// Actualiza la dirección de la vista según el paso actual del patrón
	void UpdateLook(int anim_id);

	int attack_delay;	// Retraso entre ataques
	KoopaState state;

	int current_step;	// Paso actual del patrón
	int current_frames;	// Número de frames en el paso actual
	std::vector<KoopaStep> pattern;
	int limit_left;
	int limit_right;
	float speed = 1;
	float vertical_speed = 0;
	float MAX_FALL_SPEED = 5.0f;
	float position_foor = 208;
};
