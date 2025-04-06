#pragma once
#include "Enemy.h"

#define SLIME_SPEED			1 // Asegúrate de que esto sea un valor flotante (float)
#define SLIME_ANIM_DELAY	(4*ANIM_DELAY)

#define SLIME_SHOT_OFFSET_X_LEFT	-14
#define SLIME_SHOT_OFFSET_X_RIGHT	 26
#define SLIME_SHOT_OFFSET_Y			-22

#define SLIME_SHOT_SPEED	4

enum class SlimeState { ROAMING, ATTACK };
enum class SlimeAnim {
	IDLE_LEFT, IDLE_RIGHT, WALKING_LEFT, WALKING_RIGHT,
	ATTACK_LEFT, ATTACK_RIGHT, NUM_ANIMATIONS
};

struct Step
{
	Point speed;	// Dirección de movimiento
	int frames;		// Duración en número de frames
	int anim;		// Representación gráfica
};

class Slime : public Enemy
{
public:
	Slime(const Point& p, int width, int height, int frame_width, int frame_height);
	~Slime();

	// Inicializa al enemigo con la dirección y área especificada
	AppStatus Initialise(Look look, const AABB& area) override;

	// Actualiza la lógica del enemigo, devuelve true si el enemigo debe disparar
	bool Update(const AABB& box) override;

	// Recupera la posición y dirección del disparo
	void GetShootingPosDir(Point* pos, Point* dir) const override;

private:
	// Crea el patrón de comportamiento
	void InitPattern();

	// Actualiza la dirección de la vista según el paso actual del patrón
	void UpdateLook(int anim_id);

	int attack_delay;	// Retraso entre ataques
	SlimeState state;

	int current_step;	// Paso actual del patrón
	int current_frames;	// Número de frames en el paso actual
	std::vector<Step> pattern;
};
