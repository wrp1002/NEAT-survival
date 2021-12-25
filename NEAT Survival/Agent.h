#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "Object.h"
#include "NEAT.h"
#include "UserInput.h"
#include "AgentManager.h"
#include "Food.h"
#include "Eye.h"
#include "Mouth.h"
#include "AgentStats.h"
#include "GameRules.h"
#include "TriangleEye.h"

#include <iostream>
#include <memory>

using namespace std;

class Agent : public Object, public std::enable_shared_from_this<Agent> {
private:
	const float maxRotationSpeed = 0.1;
	bool userControlled = false;
	bool shouldReproduce;

	int digestTime, digestTimeStart;

	AgentStats stats;
	shared_ptr<NEAT> nn;
	vector<double> memory;
	//vector<shared_ptr<Eye>> eyes;
	vector<shared_ptr<TriangleEye>> eyes;
	shared_ptr<Mouth> mouth;
	float eyeSpreadPercent, eyeSpreadMax;
	float viewDistance;
	
	float dirToFood, dirToAgent;
	float forwardSpeed, rotationSpeed;
	int generation;

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

	void SetGenes(vector<float> newGenes);
	void MutateGenes();
	vector<float> GenerateRandomGenes();
	vector<float> GetGenes();

	void Mutate();
	void MutateAddConnection();
	void MutateAddNode();
	void MutateRemoveNode();

	void SetUserControlled(bool user) { this->userControlled = user; }
	void SetEnergy(double newEnergy);
	void SetGeneration(int newGeneration);
	void AddEnergy(double amount);
	void SetHealth(double newHealth);
	void AddWaste(double amount);

	float GetX();
	float GetY();
	shared_ptr<NEAT> GetNN();
	int GetGeneration();

	double GetEnergy();
	double GetWaste();
	double GetHealth();
	float GetEnergyPercent();
	float GetHealthPercent();
	float GetAge();
	float GetDamage();
	
};

