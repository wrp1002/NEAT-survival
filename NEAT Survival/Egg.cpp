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
	if (!alive)
		return;

	if (hatchTimer > 0)
		hatchTimer--;

	// release energy if dead
	if (this->health <= 0) {
		alive = false;
		ReleaseEnergy();
	}
}

void Egg::Draw() {
	Object::Draw();
}

void Egg::Print() {
	cout << "Egg X:" << pos.x << " Y:" << pos.y << endl;
}

void Egg::ReleaseEnergy() {
	double total = energy + health;
	cout << "Egg died releaseing: " << total << endl;

	while (total >= Food::MAX_ENERGY) {
		double size = Globals::RandomInt(Food::MAX_ENERGY / 2, Food::MAX_ENERGY);
		ObjectSpawnQueue::SpawnFood(pos, size, Food::MEAT);
		total -= size;
	}
	if (total > 0)
		ObjectSpawnQueue::SpawnFood(pos, total, Food::MEAT);
}

void Egg::SetHealth(double newHealth) {
	this->health = newHealth;
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
