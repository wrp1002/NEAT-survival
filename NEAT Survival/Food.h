#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>

#include "Object.h"

using namespace std;

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

	constexpr static int MAX_RADIUS = 50.0;
	constexpr static double MAX_ENERGY = 100.0;
	constexpr static double STARTING_ENERGY = 15.0;

	Food(Vector2f startingPos, double energy, int type = FOOD);

	void Update();
	void Draw();
	void CollisionEvent(shared_ptr<Object> other);

	void SetEnergy(double newEnergy);

	double GetEnergy();
	float GetRadiusForEnergy(double energy);
	bool IsFood();
	bool IsWaste();
	bool IsMeat();
};

