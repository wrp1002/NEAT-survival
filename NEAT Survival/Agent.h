#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "Object.h"
#include "AgentStats.h"

#include <iostream>
#include <memory>
#include <set>

class NEAT;
class Eye;
class TriangleEye;
class Mouth;
class AgentStats;

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
	set<int> damagedInputs;
	float eyeSpreadPercent, eyeSpreadMax;
	float viewDistance;

	float dirToFood, dirToAgent;
	float forwardSpeed, rotationSpeed;
	int generation;

	weak_ptr<Object> bitObjectPtr;
	float bitObjDir, bitObjDeltaDir, bitObjDist;

	weak_ptr<Object> damageObjectPtr;
	float damageObjDir, damageObjDeltaDir, damageObjDist;

	shared_ptr<Object> GetClosestObjectOfType(vector<shared_ptr<Object>> nearbyObjects, string type);

public:
	Agent(float x, float y, float radius, vector<string> inputLabels, vector<string> outputLabels);
	Agent(float x, float y, float radius, shared_ptr<NEAT> nn);
	~Agent();

	void Print();
	void Init();
	void Update();
	void Draw();
	void DrawHealthBar();
	void DrawEnergyBar();
	void CollisionEvent(shared_ptr<Object> other);
	void Reproduce();
	void DamageRandomInput();
	void RepairRandomInput();

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
	void SetWaste(double newWaste);
	void SetGeneration(int newGeneration);
	void AddEnergy(double amount);
	void SetHealth(double newHealth);
	void AddWaste(double amount);
	void HealthToWaste(double amount);
	void IncrementKills();

	float GetX();
	float GetY();
	shared_ptr<NEAT> GetNN();
	int GetGeneration();
	int GetKills();
	int GetDamagedInputsCount();

	double GetEnergy();
	double GetEnergyUsage();
	double GetWaste();
	double GetHealth();
	double GetMaxHealth();
	float GetEnergyPercent();
	float GetHealthPercent();
	float GetAge();
	float GetMaxAge();
	float GetDamage();
	bool ShouldReproduce();
	bool InputNodeDisabled(int index);
	AgentStats GetAgentStats();
	shared_ptr<NEAT> CopyNN();

};

