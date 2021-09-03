#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Object.h"
#include "Globals.h"

class Food : public Object {
private:
	double energy;

public:
	inline static double TARGET_RADIUS = 5;
	inline static double MAX_ENERGY = 10.0;

	Food(Vector2f startingPos, double energy);

	void Draw();

	double GetEnergy();
};

