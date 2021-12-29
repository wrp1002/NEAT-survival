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
	healAmount = 0.1;
	accSpeed = 0;
	size = 0;

	age = 0;
	maxAge = 60.0 * 5;
	color = al_map_rgb_f(rGene, gGene, bGene);
}

void AgentStats::SetGenes(vector<float> newGenes) {
	genes = newGenes;

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
	healAmount = 0.1;

	age = 0;
	maxAge = 60.0 * 5;


	// Apply color genes: 0-2
	rGene = genes[0];
	gGene = genes[1];
	bGene = genes[2];

	// Size gene: 3
	sizeGene = genes[3];
	maxHealth = sizeGene * 200.0;
	maxEnergy = sizeGene * 200.0;
	size = sizeGene * 15.0 + 5;
	damage = sizeGene * 20.0;
	
	health = maxHealth / 2;
	energy = maxEnergy / 2;


	// Speed gene: 4
	speedGene = genes[4];
	accSpeed = speedGene + 0.5;
}

void AgentStats::Mutate() {
	for (unsigned i = 0; i < genes.size(); i++) {
		genes[i] += Globals::RandomSign() * Globals::Random() * 0.1;
		genes[i] = Globals::Constrain(genes[i], 0.0, 1.0);
	}
	SetGenes(genes);
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

void AgentStats::HealthToWaste(double amount) {
	health -= amount;
	waste += amount;
	if (health < 0) {
		double diff = abs(health);
		health += diff;
		waste -= diff;
	}
}

void AgentStats::HealthToEnergy(double amount) {
	health -= amount;
	energy += amount;
}

void AgentStats::WasteToHealth(double amount) {
	waste -= amount;
	health += amount;
}

void AgentStats::WasteToEnergy(double amount) {
	waste -= amount;
	energy += amount;
}

void AgentStats::EnergyToWaste(double amount) {
	energy -= amount;
	waste += amount;
}

void AgentStats::EnergyToHealth(double amount) {
	energy -= amount;
	health += amount;
}

