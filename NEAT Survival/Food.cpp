#include "Food.h"

Food::Food(Vector2f startingPos, double startingEnergy) : Object(startingPos.x, startingPos.y, TARGET_RADIUS * (startingEnergy / MAX_ENERGY) + 1) {
	objectType = "food";
	energy = startingEnergy;
	
	pos.x += Globals::Random() * 5 - 2.5;
	pos.y += Globals::Random() * 5 - 2.5;

	float randVel = 10;
	vel.x += Globals::Random() * randVel - randVel / 2;
	vel.y += Globals::Random() * randVel - randVel / 2;

	//cout << "New food" << pos.x << pos.y << endl;
}

void Food::Draw() {
	al_draw_filled_circle(pos.x, pos.y, radius, al_map_rgb(0, 255, 0));
}

double Food::GetEnergy() {
	return energy;
}
