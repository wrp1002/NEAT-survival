#pragma once

#include <allegro5/allegro.h>

#include <vector>

#include "Globals.h"

using namespace std;

class AgentStats {
private:
	float rGene;
	float gGene;
	float bGene;
	float healthGene;
	float speedGene;
	float sizeGene;
	float dietGene;

public:
	double health, maxHealth;
	double energy, maxEnergy;
	double waste, maxWaste;
	double maxSpeed;
	double damage;
	double healAmount;

	float age, maxAge;

	ALLEGRO_COLOR color;

	AgentStats();
	AgentStats(vector<float> genes);

	double GetAgePercent();
	double GetHealthPercent();
	double GetEnergyPercent();
	double GetWastePercent();
};

