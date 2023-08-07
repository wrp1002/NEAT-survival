#pragma once

#include <allegro5/allegro.h>

#include <vector>
#include <iostream>
#include <format>

#include "Globals.h"

using namespace std;

class AgentStats {
private:
	float healthGene;
	float speedGene;
	float dietGene;

public:
	vector<float> genes;
	float rGene;
	float gGene;
	float bGene;
	float sizeGene;
	float dietMeatCoef;
	float dietPlantCoef;

	double health, maxHealth;
	double energy, maxEnergy;
	double waste, maxWaste;
	double maxSpeed;
	double damage;
	double healAmount;

	float size;
	float accSpeed;
	float age, maxAge;

	int hurtTimer;
	int hurtTimerStart;

	ALLEGRO_COLOR color;


	AgentStats();
	void SetGenes(vector<float> newGenes);
	void Mutate();
	void Print();

	double GetAgePercent();
	double GetHealthPercent();
	double GetEnergyPercent();
	double GetWastePercent();

	void HealthToWaste(double amount);
	void HealthToEnergy(double amount);
	void WasteToHealth(double amount);
	void WasteToEnergy(double amount);
	void EnergyToWaste(double amount);
	void EnergyToHealth(double amount);
};

