#include "Egg.h"

Egg::Egg(vector<float> genes, shared_ptr<NEAT> nn, double health, double energy, int generation, Vector2f pos, float radius) : Object(pos.x, pos.y, radius) {
	this->genes = genes;
	this->nn = nn;
	this->health = health;
	this->energy = energy;
	this->generation = generation;
	this->pos = pos;
	this->radius = radius;
	this->hatchTimer = 500;
	this->objectType = "egg";
	SetColor(al_map_rgb(255, 255, 255));
}

void Egg::Update() {
	if (hatchTimer > 0)
		hatchTimer--;
}

void Egg::Draw() {
	Object::Draw();
}

void Egg::Print() {
	cout << "Egg X:" << pos.x << " Y:" << pos.y << endl;
}

shared_ptr<NEAT> Egg::GetNN() {
	return nn;
}

vector<float> Egg::GetGenes() {
	return genes;
}

bool Egg::ReadyToHatch() {
	return hatchTimer <= 0 && alive;
}

int Egg::GetHatchTimer() {
	return hatchTimer;
}

int Egg::GetGeneration() {
	return generation;
}

double Egg::GetHealth() {
	return health;
}

double Egg::GetEnergy() {
	return energy;
}
