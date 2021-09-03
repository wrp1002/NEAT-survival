#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "Object.h"
#include "UserInput.h"
#include "NEAT.h"
#include "AgentManager.h"

#include <iostream>

using namespace std;

class Agent : public Object {
private:
	const float accSpeed = 1;
	const float maxRotationSpeed = 0.1;
	const double healAmount = 0.2;
	bool userControlled = false;
	bool shouldReproduce;

	double energy, maxEnergy;
	double waste, maxWaste;
	float health, maxHealth;
	ALLEGRO_COLOR color;

	shared_ptr<NEAT> nn;
	
	float dir;
	float dirToFood, dirToAgent;
	float forwardSpeed, rotationSpeed;
	int generation;
	double age;
	double maxAge;

	vector<shared_ptr<Object>> GetNearbyObjects();
	shared_ptr<Object> GetClosestObjectOfType(vector<shared_ptr<Object>> nearbyObjects, string type);

public:
	Agent(float x, float y, float radius, vector<string> inputLabels, vector<string> outputLabels);
	Agent(float x, float y, float radius, shared_ptr<NEAT> nn);
	~Agent();

	void Print();
	void Init();
	void Update();
	void Draw();
	void CollisionEvent(shared_ptr<Object> other);
	void Reproduce();
	void Mutate();
	void MutateAddConnection();

	void SetUserControlled(bool user) { this->userControlled = user; }
	void SetEnergy(double newEnergy);
	void SetGeneration(int newGeneration);

	float GetX();
	float GetY();
	shared_ptr<NEAT> GetNN();
	int GetGeneration();
	bool ShouldReproduce();

	double GetEnergy();
	double GetWaste();
	float GetEnergyPercent();
	float GetHealthPercent();
	float GetAge();
};

