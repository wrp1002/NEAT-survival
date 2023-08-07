#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Object.h"
#include "Globals.h"

class Food : public Object {
private:
	int foodType;
	double energy;
	int life;

	void ConvertToFood();
	void ConvertToWaste();
	void ConvertToMeat();

public:
	enum FOOD_TYPES { FOOD, WASTE, MEAT };

	inline static double TARGET_RADIUS = 5;
	inline static double MAX_ENERGY = 15.0;

	Food(Vector2f startingPos, double energy, int type = FOOD);

	void Update();
	void Draw();

	void SetEnergy(double newEnergy);

	double GetEnergy();
	bool IsFood();
	bool IsWaste();
	bool IsMeat();
	int GetLife();
};

