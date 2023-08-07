#include "Food.h"

void Food::ConvertToFood() {
	objectType = "food";
	SetColor(al_map_rgb(0, 255, 0));
	foodType = FOOD;
}

void Food::ConvertToWaste() {
	objectType = "waste";
	SetColor(al_map_rgb(168, 111, 50));
	foodType = WASTE;
	life = 1000 + rand() % 500;
}

void Food::ConvertToMeat() {
	objectType = "meat";
	SetColor(al_map_rgb(255, 0, 0));
	foodType = MEAT;
	life = 1000 + rand() % 500;
}

Food::Food(Vector2f startingPos, double startingEnergy, int newType) : Object(startingPos.x, startingPos.y, TARGET_RADIUS * (startingEnergy / MAX_ENERGY) + 1) {
	energy = startingEnergy;
	foodType = newType;
	life = 0;
	
	pos.x += Globals::Random() * 5 - 2.5;
	pos.y += Globals::Random() * 5 - 2.5;

	float randVel = 20;
	vel.x += Globals::Random() * randVel - randVel / 2;
	vel.y += Globals::Random() * randVel - randVel / 2;

	if (foodType == FOOD)
		ConvertToFood();
	if (foodType == WASTE)
		ConvertToWaste();
	else if (foodType == MEAT)
		ConvertToMeat();
	
	//cout << "New food" << pos.x << pos.y << endl;
}

void Food::Update() {
	if (life > 0) {
		if (rand() % 2 == 0)
			life--;

		if (life == 0) {
			if (IsWaste())
				ConvertToFood();
			else if (IsMeat())
				ConvertToWaste();
		}
	}
	if (energy <= 0)
		alive = false;

	radius = TARGET_RADIUS * (energy / MAX_ENERGY);

	Object::Update();
}

void Food::Draw() {
	al_draw_filled_circle(pos.x, pos.y, radius, color);
}

double Food::GetEnergy() {
	return energy;
}

bool Food::IsFood() {
	return foodType == FOOD;
}

bool Food::IsWaste() {
	return foodType == WASTE;
}

bool Food::IsMeat() {
	return foodType == MEAT;
}

void Food::SetEnergy(double newEnergy) {
	energy = newEnergy;
	if (energy <= 0)
		alive = false;
}

int Food::GetLife() {
	return life;
}
