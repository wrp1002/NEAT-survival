#pragma once
#include <allegro5/allegro_primitives.h>

#include <iostream>
#include <memory>

#include "Object.h"

class NEAT;
class Vector2f;

using std::cout, std::endl;

class Egg : public Object {
private:
	vector<float> genes;
	shared_ptr<NEAT> nn;

	double health, energy;
	float radius;
	int hatchTimer;
	int generation;

public:
	Egg(vector<float> genes, shared_ptr<NEAT> nn, double health, double energy, int generation, Vector2f pos, float radius);

	void Update();
	void Draw();
	void Print();

	shared_ptr<NEAT> GetNN();
	vector<float> GetGenes();
	bool ReadyToHatch();
	int GetHatchTimer();
	int GetGeneration();
	double GetHealth();
	double GetEnergy();
};

