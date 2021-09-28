#include "AgentStats.h"

AgentStats::AgentStats() {
	rGene = 0;
	gGene = 0;
	bGene = 0;
	healthGene = 0;
	speedGene = 0;
	sizeGene = 0;
	dietGene = 0;

	health = 0;
	maxHealth = 0;
	energy = 0;
	maxEnergy = 0;
	waste = 0;
	maxWaste = 0;
	maxSpeed = 0;
	damage = 15;
	healAmount = 0;

	age = 0;
	maxAge = 60.0 * 5;
	color = al_map_rgb_f(rGene, gGene, bGene);
}

AgentStats::AgentStats(vector<float> genes) {
	rGene = 0;
	gGene = 0;
	bGene = 0;
	healthGene = 0;
	speedGene = 0;
	sizeGene = 0;
	dietGene = 0;

	health = 0;
	maxHealth = 0;
	energy = 0;
	maxEnergy = 0;
	waste = 0;
	maxWaste = 0;
	maxSpeed = 0;
	damage = 15;
	healAmount = 0;

	age = 0;
	maxAge = 60.0 * 5;
	color = al_map_rgb_f(rGene, gGene, bGene);
}


double AgentStats::GetAgePercent() {
	return age / maxAge;
}

double AgentStats::GetHealthPercent() {
	return health / maxHealth;
}

double AgentStats::GetEnergyPercent() {
	return energy / maxEnergy;
}

double AgentStats::GetWastePercent() {
	return waste / maxWaste;
}

