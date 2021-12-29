#include "Agent.h"


void Agent::Init() {
	forwardSpeed = 0;
	rotationSpeed = 0;
	dir = Globals::RandomDir();
	objectType = "agent";
	
	generation = 1;

	digestTimeStart = 500;
	digestTime = digestTimeStart;
	healing = false;
	
	// random color for now
	SetColor(al_map_rgb(Globals::RandomInt(0, 255), Globals::RandomInt(0, 255), Globals::RandomInt(0, 255)));

	// create eyes
	viewDistance = 250.0;
	eyeSpreadMax = M_PI / 8;
	float offsetInterval = M_PI / 5;
	eyes.clear();
	for (float offset = -offsetInterval; offset <= offsetInterval; offset += offsetInterval)
		eyes.push_back(make_shared<TriangleEye>(TriangleEye(eyeSpreadMax, viewDistance, offset, shared_from_this())));


	// create mouth
	mouth = make_shared<Mouth>(Mouth(shared_from_this(), 7));

	// create memory
	memory.clear();
	memory.push_back(0.0);

	// create genes
	SetGenes(GenerateRandomGenes());
}

void Agent::Print() {
	cout << "Agent energy:" << stats.energy << " waste:" << stats.waste << endl;
	nn->PrintNN();
}

Agent::Agent(float x, float y, float radius, vector<string> inputLabels, vector<string> outputLabels) : Object(x, y, radius) {
	//cout << "Agent init!" << endl;
	nn = make_shared<NEAT>(NEAT(inputLabels, outputLabels));
}

Agent::Agent(float x, float y, float radius, shared_ptr<NEAT> newNN) : Object(x, y, radius) {
	nn = newNN;
}


Agent::~Agent() {
	
}

void Agent::Update() {
	if (!alive) {
		//cout <<" tried update but not alive'" << endl;
		return;
	}


	vector<shared_ptr<Object>> nearbyObjects = GetNearbyObjects();
	int nearbyFood = 0;
	int nearbyWaste = 0;
	int nearbyAgents = 0;
	for (auto obj : nearbyObjects) {
		if (obj.get() == this || pos.GetDistance(obj->GetPos()) > viewDistance)
			continue;
		if (obj->GetType() == "agent")
			nearbyAgents++;
		else if (obj->GetType() == "food")
			nearbyFood++;
		else if (obj->GetType() == "waste")
			nearbyWaste++;
	}


	for (auto eye : eyes)
		eye->Update(nearbyObjects);
	mouth->Update(nearbyObjects);

	
	// Get dir to bit object
	bitObjDeltaDir = 0;
	bitObjDir = 0;
	bitObjDist = 0;
	bool bitObjIsFood = false;
	bool bitObjIsWaste = false;
	shared_ptr<Object> bitObj = bitObjectPtr.lock();
	if (bitObj) {
		bitObjDir = pos.GetAngleTo(bitObj->GetPos());
		bitObjDeltaDir = atan2(sin(bitObjDir - dir), cos(bitObjDir - dir));
		bitObjDist = pos.GetDistance(bitObj->GetPos());
		if (bitObj->GetType() == "agent" || bitObj->GetType() == "food")
			bitObjIsFood = true;
		if (bitObj->GetType() == "waste")
			bitObjIsWaste = true;

		if (bitObjDist >= viewDistance)
			bitObjectPtr.reset();
	}


	// Get output from NN
	vector<double> inputs = {
		1.0,								// const
		sin(stats.age),						// sin
		double(sin(stats.age) > 0),			// tick
		stats.GetAgePercent(),				// age
		stats.GetHealthPercent(),			// health
		stats.GetEnergyPercent(),			// energy
		memory[0],							// mem1
		nearbyFood   / 10.0,				// nearby food
		nearbyWaste  / 10.0,				// nearby waste
		nearbyAgents / 10.0,				// nearby objects
		(double)mouth->CanBite(),			// can bite
		(double)mouth->ObjectInMouth(),		// object in mouth
		bitObjDeltaDir,						// dir to bit object
		1.0 - bitObjDist / viewDistance,	// dist to bit object
		(double)bitObjIsFood,				// bit object is food
		(double)bitObjIsWaste,				// bit object is waste
		eyeSpreadPercent,					// eye spread
	};
	for (auto eye : eyes) {
		inputs.push_back(1.0 - eye->GetMinDistPercent());
		inputs.push_back(eye->GetSeenObjectsSize() / 5.0);
		inputs.push_back(eye->GetAvgR());
		inputs.push_back(eye->GetAvgG());
		inputs.push_back(eye->GetAvgB());
	}
	vector<double> outputs = nn->Calculate(inputs);


	// extract output from NN
	forwardSpeed = outputs[0] * stats.accSpeed;
	rotationSpeed = ( (outputs[1] + 1.0) / 2 - (outputs[2] + 1.0) / 2 ) / 2 * maxRotationSpeed;
	bool wantToReproduce = outputs[3] > 0.5 || stats.energy > 0.80;
	bool wantsToHeal = outputs[4] > 0.5;
	double mem1 = outputs[5];
	bool mem1Overwrite = outputs[6] > 0.5;
	bool wantsToEat = outputs[7] > 0.5;
	bool wantsToBoost = outputs[8] > 0.5;
	eyeSpreadPercent = (outputs[9] + 1.0) / 2;


	// handle user input
	if (userControlled) {
		forwardSpeed = stats.accSpeed * (UserInput::IsPressed(ALLEGRO_KEY_UP) - UserInput::IsPressed(ALLEGRO_KEY_DOWN));
		rotationSpeed = maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_LEFT) - maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_RIGHT);
		wantsToEat = UserInput::IsPressed(ALLEGRO_KEY_E);
		wantsToBoost = UserInput::IsPressed(ALLEGRO_KEY_B);
		wantToReproduce = UserInput::IsPressed(ALLEGRO_KEY_R);
		wantsToHeal = UserInput::IsPressed(ALLEGRO_KEY_H);
	}


	shouldReproduce = wantToReproduce && stats.energy > stats.maxEnergy * 0.5;


	if (mem1Overwrite)
		memory[0] = mem1;


	if (GameRules::IsRuleEnabled("EyeMovement")) {
		// update eye angles
		for (auto eye : eyes)
			eye->SetSpreadPercent(eyeSpreadPercent);
		
	}
	else {
		for (auto eye : eyes)
			eye->SetSpreadPercent(1);
	}


	double energyUsage = 0.005;
	
	//if (shouldReproduce)
		//Reproduce();

	// healing
	healing = false;
	if (wantsToHeal && stats.energy > stats.healAmount) {
		healing = true;
		stats.health += stats.healAmount;
		stats.energy -= stats.healAmount;
		if (stats.health > stats.maxHealth) {
			double diff = stats.health - stats.maxHealth;
			stats.health -= diff;
			stats.waste += diff;
		}

		forwardSpeed = 0;
		rotationSpeed = 0;
		wantsToBoost = false;
		wantsToEat = false;
		wantToReproduce = false;
	}

	energyUsage += abs(outputs[0]) * 0.005 * stats.sizeGene * (1.0 + wantsToBoost * 5.0);
	energyUsage += abs(outputs[1]) * 0.005 * stats.sizeGene;
	

	// energy usage
	if (energyUsage < stats.energy) {
		stats.energy -= energyUsage;
		stats.waste += energyUsage;
	}
	else {
		stats.health -= 0.1;
		stats.waste += 0.1;
	}

	// movement
	dir += rotationSpeed;
	dir = fmod(dir, 2 * M_PI);
	acc = Vector2f(cos(dir) * forwardSpeed, -sin(dir) * forwardSpeed);
	acc *= (1 + wantsToBoost);
	vel += acc;
	vel *= dragCoef;


	// mouth control
	if (!GameRules::IsRuleEnabled("MouthControl"))
		wantsToEat = true;
	
	mouth->SetWantsToBite(wantsToEat);
	if (wantsToEat && mouth->CanBite() && mouth->ObjectInMouth()) {
		mouth->Bite();
		bitObjectPtr = mouth->GetObjectInMouth();
	}


	// digestion
	digestTime--;
	if (digestTime <= 0) {
		digestTime = digestTimeStart;
		int amount = Globals::RandomInt(1, 3);
		for (int i = 0; i < amount; i++) {
			if (stats.waste > 10.0) {
				GameManager::SpawnFood(pos + Vector2f::FromDir(dir + M_PI, radius), 10.0, Food::WASTE);
				stats.waste -= 10.0;
			}
			else
				break;
		}
	}

	Object::Update();

	// update eye and mouth positions
	for (auto eye : eyes)
		eye->UpdatePos();
	mouth->UpdatePosition();

	// aging
	stats.age += 1.0 / 60;
	if (stats.age > stats.maxAge) {
		stats.health -= 0.1;
		stats.waste += 0.1;
	}


	// release energy if dead
	if (stats.health <= 0 && alive) {
		alive = false;

		double total = stats.energy + stats.waste + stats.health;
		
		while (total >= Food::MAX_ENERGY) {
			double size = Globals::RandomInt(Food::MAX_ENERGY / 2, Food::MAX_ENERGY);
			if (Globals::RandomInt(0, 1) == 0)
				GameManager::SpawnFood(pos, size, Food::FOOD);
			else
				GameManager::SpawnFood(pos, size, Food::WASTE);
			total -= size;
		}
		if (total > 0)
			GameManager::SpawnFood(pos, total);

	}
	
}

void Agent::Draw() {
	//al_draw_circle(pos.x, pos.y, viewDistance, al_map_rgba(50, 0, 50, 0.5), 2);

	for (auto eye : eyes)
		eye->DrawTriangle();
	for (auto eye : eyes)
		eye->DrawLines();

	mouth->Draw();

	al_draw_filled_circle(pos.x, pos.y, radius, color);
	
	int lineLen = radius*2 + forwardSpeed;

	if (healing) {
		al_draw_circle(pos.x, pos.y, radius + 5, al_map_rgba(100, 0, 0, 50), 2);
	}
	//al_draw_line(pos.x, pos.y, pos.x + cos(dir) * lineLen, pos.y - sin(dir) * lineLen, al_map_rgb(255, 0, 255), 2);

	//al_draw_line(pos.x, pos.y, pos.x + cos(dirToFood) * 20, pos.y - sin(dirToFood) * 20, al_map_rgb(0, 255, 0), 2);
	//al_draw_line(pos.x, pos.y, pos.x + cos(dirToAgent) * 20, pos.y - sin(dirToAgent) * 20, al_map_rgb(255, 0, 0), 2);
	al_draw_line(pos.x, pos.y, pos.x + cos(bitObjDir) * 20, pos.y - sin(bitObjDir) * 20, al_map_rgb(255, 0, 0), 2);
	al_draw_line(pos.x, pos.y, pos.x + cos(bitObjDeltaDir) * 20, pos.y - sin(bitObjDeltaDir) * 20, al_map_rgb(255, 0, 255), 2);

}

void Agent::CollisionEvent(shared_ptr<Object> other) {
	if (shared_ptr<Food> food = dynamic_pointer_cast<Food>(other)) {
		if (!food->IsAlive() || !IsAlive())
			return;

		/*
		else if (food->IsFood()) {
			energy += food->GetEnergy();
			food->SetAlive(false);
		}
		*/
	}
}

void Agent::Reproduce() {
	shared_ptr<NEAT> newNN = nn->Copy();
	shared_ptr<Agent> child = GameManager::SpawnAgent(pos.x, pos.y, newNN);
	child->Mutate();
	child->SetGeneration(generation + 1);
	child->SetGenes(stats.genes);
	child->MutateGenes();

	double childEnergy = stats.energy / 2;
	child->SetEnergy(childEnergy / 2);
	child->SetHealth(childEnergy / 2);
	stats.energy /= 2;
}

void Agent::SetGenes(vector<float> newGenes) {
	stats.SetGenes(newGenes);
	SetColor(al_map_rgb_f(stats.rGene, stats.gGene, stats.bGene));
	radius = stats.size;
}

void Agent::MutateGenes() {
	for (unsigned i = 0; i < stats.genes.size(); i++) {
		stats.genes[i] += Globals::RandomSign() * Globals::Random() * 0.1;
		stats.genes[i] = Globals::Constrain(stats.genes[i], 0.0, 1.0);
	}
}

vector<float> Agent::GenerateRandomGenes() {
	vector<float> randomGenes;
	for (unsigned i = 0; i < 20; i++) {
		randomGenes.push_back(Globals::Random());
	}
	return randomGenes;
}

vector<float> Agent::GetGenes() {
	return stats.genes;
}

void Agent::Mutate() {
	nn->Mutate();
}

void Agent::MutateAddConnection() {
	nn->MutateAddConnection();
}

void Agent::MutateAddNode() {
	nn->MutateAddNode();
}

void Agent::MutateRemoveNode() {
	nn->MutateRemoveNode();
}

vector<shared_ptr<Object>> Agent::GetNearbyObjects() {
	vector<shared_ptr<Object>> foundObjects = GameManager::collisionGrid.GetObjects(pos, 1);
	return foundObjects;
}

shared_ptr<Object> Agent::GetClosestObjectOfType(vector<shared_ptr<Object>> nearbyObjects, string type) {
	float minDist = 100000;
	shared_ptr<Object> closest = nullptr;

	for (auto object : nearbyObjects) {
		if (object->GetType() != type || object.get() == this)
			continue;

		float distCheck = object->GetPos().GetDistance(pos);
		if (distCheck < minDist && distCheck <= viewDistance) {
			closest = object;
			minDist = distCheck;
		}
	}

	return closest;
}

float Agent::GetX() {
	return pos.x;
}

float Agent::GetY() {
	return pos.y;
}

shared_ptr<NEAT> Agent::GetNN() {
	return nn;
}

void Agent::SetEnergy(double newEnergy) {
	stats.energy = newEnergy;
}

void Agent::SetGeneration(int newGeneration) {
	generation = newGeneration;
}

void Agent::AddEnergy(double amount) {
	stats.energy += amount;
	if (stats.energy > stats.maxEnergy) {
		double overflow = stats.energy - stats.maxEnergy;
		stats.energy -= overflow;
		stats.waste += overflow;
	}
}

void Agent::SetHealth(double newHealth) {
	stats.health = newHealth;
}

void Agent::AddWaste(double amount) {
	stats.waste += amount;
}

void Agent::HealthToWaste(double amount) {
	stats.HealthToWaste(amount);
}

int Agent::GetGeneration() {
	return generation;
}

double Agent::GetEnergy() {
	return stats.energy;
}

double Agent::GetWaste() {
	return stats.waste;
}

double Agent::GetHealth() {
	return stats.health;
}

float Agent::GetEnergyPercent() {
	return stats.energy / stats.maxEnergy;
}

float Agent::GetAge() {
	return stats.age;
}

float Agent::GetDamage() {
	return stats.damage;
}

bool Agent::ShouldReproduce() {
	return shouldReproduce;
}

AgentStats Agent::GetAgentStats() {
	return stats;
}

shared_ptr<NEAT> Agent::CopyNN() {
	return nn->Copy();
}

float Agent::GetHealthPercent() {
	return stats.health / stats.maxHealth;
}


