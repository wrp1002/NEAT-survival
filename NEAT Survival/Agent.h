#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "Object.h"
#include "NEAT.h"
#include "UserInput.h"
#include "Food.h"
#include "Eye.h"
#include "Mouth.h"
#include "AgentStats.h"
#include "GameRules.h"
#include "TriangleEye.h"
#include "ObjectSpawnQueue.h"
#include "Egg.h"

#include <iostream>
#include <memory>

using namespace std;

class Agent : public Object, public std::enable_shared_from_this<Agent> {
private:
	const float maxRotationSpeed = 0.1;
	bool userControlled = false;
	bool shouldReproduce;
	bool healing;

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

	weak_ptr<Object> bitObjectPtr;
	float bitObjDir, bitObjDeltaDir, bitObjDist;

	weak_ptr<Object> damageObjectPtr;
	float damageObjDir, damageObjDeltaDir, damageObjDist;

	weak_ptr<Object> closestAgentPtr;
	float closestAgentDir, closestAgentDeltaDir, closestAgentDist;

	vector<weak_ptr<Object>> nearbyObjects;

	shared_ptr<Object> GetClosestObjectOfType(vector<weak_ptr<Object>> nearbyObjects, string type);

public:
	Agent(float x, float y, float radius, vector<string> inputLabels, vector<string> outputLabels);
	Agent(float x, float y, float radius, shared_ptr<NEAT> nn);
	~Agent();

	void Print();
	void Init();
	void Update();
	void Draw();
	void CollisionEvent(shared_ptr<Object> other);
	void SetNearbyObjects(vector<weak_ptr<Object>> nearbyObjects);
	void Reproduce();
	void EatPlant(double amount);
	void EatMeat(double amount);

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
	void HealthToWaste(double amount);
	void TakeDamage(double amount);
	void SetWaste(double amount);

	bool ShouldReproduce();
	bool IsHurt();
	bool IsDead();
	int GetGeneration();
	float GetX();
	float GetY();
	float GetEnergyPercent();
	float GetHealthPercent();
	float GetAge();
	float GetDamage();
	double GetEnergy();
	double GetWaste();
	double GetHealth();
	shared_ptr<NEAT> GetNN();
	AgentStats GetAgentStats();
	shared_ptr<NEAT> CopyNN();

};

