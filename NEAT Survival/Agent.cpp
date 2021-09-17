#include "Agent.h"


void Agent::Init() {
	forwardSpeed = 0;
	rotationSpeed = 0;
	dir = Globals::RandomDir();
	objectType = "agent";
	
	maxEnergy = 100.0;
	energy = maxEnergy / 2;
	
	maxHealth = 100.0;
	health = maxHealth;

	waste = 0.0;
	maxWaste = 100.0;

	generation = 1;
	age = 0.0;
	maxAge = 60.0 * 5;

	digestTimeStart = 500;
	digestTime = digestTimeStart;

	// random color for now
	SetColor(al_map_rgb(Globals::RandomInt(0, 255), Globals::RandomInt(0, 255), Globals::RandomInt(0, 255)));

	// create eyes
	viewDistance = 250.0;
	eyes.clear();
	eyeSpreadMax = M_PI / 6;
	eyeSpreadPercent = 1.0;
	eyes.push_back(make_shared<Eye>(Eye(shared_from_this(), viewDistance, -eyeSpreadMax)));
	eyes.push_back(make_shared<Eye>(Eye(shared_from_this(), viewDistance,  eyeSpreadMax)));

	// create memory
	memory.clear();
	memory.push_back(0.0);

	// create genes
	SetGenes(GenerateRandomGenes());
}

void Agent::Print() {
	cout << "Agent energy:" << energy << " waste:" << waste << endl;
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
		cout <<" tried update but not alive'" << endl;
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
	//shared_ptr<Object> closestFood = GetClosestObjectOfType(nearbyObjects, "food");
	//shared_ptr<Object> closestAgent = GetClosestObjectOfType(nearbyObjects, "agent");


	for (auto eye : eyes)
		eye->Update(nearbyObjects, eyeSpreadPercent);



	// dir to closest objects 
	/*
	float distanceToFood = viewDistance;
	float foodDeltaDir = 0;
	dirToFood = dir;
	if (closestFood != nullptr) {
		distanceToFood = pos.GetDistance(closestFood->GetPos());
		dirToFood = GetAngleTo(closestFood);
		foodDeltaDir = atan2(sin(dirToFood - dir), cos(dirToFood - dir));
		//cout << "dir: " << dir << " Food dir:" << foodDir << " delta:" << foodDeltaDir << endl;
	}

	float distanceToAgent = viewDistance;
	float agentDeltaDir = 0;
	dirToAgent = dir;
	if (closestAgent != nullptr) {
		distanceToAgent = pos.GetDistance(closestAgent->GetPos());
		dirToAgent = GetAngleTo(closestAgent);
		agentDeltaDir = atan2(sin(dirToAgent - dir), cos(dirToAgent - dir));
		//cout << "dir: " << dir << " Food dir:" << foodDir << " delta:" << foodDeltaDir << endl;
	}
	*/

	// Get output from NN
	vector<double> inputs = { 
		//foodDeltaDir,						// direction to closest food
		//double(closestFood != nullptr),	// sees food
		//agentDeltaDir,					// dir to closest agent
		//double(closestAgent != nullptr),	// sees agent
		//distanceToFood / viewDistance,
		//distanceToAgent / viewDistance,
		1.0,								// const
		sin(al_get_time()),					// sin
		double(sin(al_get_time()) > 0),		// tick
		age / maxAge,						// age
		health / maxHealth,					// health
		energy / maxEnergy,					// energy
		memory[0],							// mem1
		nearbyFood   / 10.0,				// nearby food
		nearbyWaste  / 10.0,				// nearby waste
		nearbyAgents / 10.0,				// nearby objects
	};
	for (auto eye : eyes) {
		inputs.push_back(eye->GetDistanceScaled());
		inputs.push_back(eye->GetViewedR());
		inputs.push_back(eye->GetViewedG());
		inputs.push_back(eye->GetViewedB());
	}
	
	vector<double> outputs = nn->Calculate(inputs);
	
	// extract output from NN
	forwardSpeed = outputs[0] * accSpeed;
	rotationSpeed = (outputs[1] * 2.0 - 1.0) * maxRotationSpeed;
	bool wantToReproduce = outputs[2] > 0.5 || energy > 0.95;
	bool wantsToHeal = outputs[3] > 0.5;
	double mem1 = outputs[4];
	bool mem1Overwrite = outputs[5] > 0.5;

	bool shouldReproduce = wantToReproduce && energy > maxEnergy * 0.5;


	if (mem1Overwrite) {
		memory.clear();
		memory.push_back(mem1);
	}


	if (userControlled) {
		forwardSpeed = accSpeed * UserInput::IsPressed(ALLEGRO_KEY_UP);
		rotationSpeed = maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_LEFT) - maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_RIGHT);
	}
	
	double energyUsage = 0.005;
	
	if (shouldReproduce)
		Reproduce();
	

	if (wantsToHeal) {
		if (energy > healAmount)
			energyUsage += healAmount;
	}

	energyUsage += outputs[0] * 0.01;
	energyUsage += outputs[1] * 0.01;


	if (energyUsage < energy) {
		// healing
		if (wantsToHeal) {
			health += healAmount;
			if (health > maxHealth)
				health = maxHealth;
		}

		energy -= energyUsage;
		waste += energyUsage;
	}
	else {
		health -= 0.1;
	}

	// movement
	dir += rotationSpeed;
	dir = fmod(dir, 2 * M_PI);

	acc = Vector2f(cos(dir) * forwardSpeed, -sin(dir) * forwardSpeed);
	vel += acc;
	vel *= dragCoef;

	// digestion
	digestTime--;
	if (digestTime <= 0) {
		digestTime = digestTimeStart;
		int amount = Globals::RandomInt(1, 3);
		for (int i = 0; i < amount; i++) {
			if (waste >= 10) {
				GameManager::SpawnFood(pos, 10.0, Food::WASTE);
				waste -= 10;
			}
			else
				break;
		}
	}

	Object::Update();


	// aging
	age += 1.0 / 60;
	if (age > maxAge)
		health -= 0.1;

	// release energy if dead
	if (health <= 0 && alive) {
		alive = false;

		while (energy >= Food::MAX_ENERGY) {
			int size = Globals::RandomInt(Food::MAX_ENERGY / 2, Food::MAX_ENERGY);
			GameManager::SpawnFood(pos, size);
			energy -= size;
		}
		if (energy)
			GameManager::SpawnFood(pos, energy);

		while (waste >= Food::MAX_ENERGY) {
			int size = Globals::RandomInt(Food::MAX_ENERGY / 2, Food::MAX_ENERGY);
			if (Globals::Random() > 0.5)
				GameManager::SpawnFood(pos, size, Food::FOOD);
			else
				GameManager::SpawnFood(pos, size, Food::WASTE);
			waste -= 10;
		}
		if (waste)
			GameManager::SpawnFood(pos, waste, Food::WASTE);
	}
}

void Agent::Draw() {
	al_draw_filled_circle(pos.x, pos.y, radius, color);
	int lineLen = radius*2 + forwardSpeed;
	al_draw_line(pos.x, pos.y, pos.x + cos(dir) * lineLen, pos.y - sin(dir) * lineLen, al_map_rgb(255, 0, 255), 2);

	al_draw_line(pos.x, pos.y, pos.x + cos(dirToFood) * 20, pos.y - sin(dirToFood) * 20, al_map_rgb(0, 255, 0), 2);
	al_draw_line(pos.x, pos.y, pos.x + cos(dirToAgent) * 20, pos.y - sin(dirToAgent) * 20, al_map_rgb(255, 0, 0), 2);

	for (auto eye : eyes)
		eye->Draw();
}

void Agent::CollisionEvent(shared_ptr<Object> other) {
	if (shared_ptr<Food> food = dynamic_pointer_cast<Food>(other)) {
		if (!food->IsAlive() || !IsAlive())
			return;

		if (food->IsWaste()) {
			health -= 0.1;
		}

		else if (food->IsFood()) {
			food->SetAlive(false);
			energy += food->GetEnergy();
			if (energy > maxEnergy) {
				double overflow = energy - maxEnergy;
				energy -= overflow;
				waste += overflow;
			}
		}
	}
}

void Agent::Reproduce() {
	shared_ptr<NEAT> newNN = nn->Copy();
	shared_ptr<Agent> child = GameManager::SpawnAgent(pos.x, pos.y, newNN);
	child->Mutate();
	child->SetEnergy(energy / 2);
	child->SetGeneration(generation + 1);
	child->SetGenes(genes);
	child->MutateGenes();
	energy /= 2;
}

void Agent::SetGenes(vector<float> newGenes) {
	genes = newGenes;

	// Apply color genes 0-2
	SetColor(al_map_rgb_f(genes[0], genes[1], genes[2]));
}

void Agent::MutateGenes() {
	for (unsigned i = 0; i < genes.size(); i++) {
		genes[i] += Globals::RandomSign() * Globals::Random() * 0.1;
		genes[i] = Globals::Constrain(genes[i], 0.0, 1.0);
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
	return genes;
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

float Agent::GetDir() {
	return dir;
}

shared_ptr<NEAT> Agent::GetNN() {
	return nn;
}

void Agent::SetEnergy(double newEnergy) {
	energy = newEnergy;
}

void Agent::SetGeneration(int newGeneration) {
	generation = newGeneration;
}

int Agent::GetGeneration() {
	return generation;
}

double Agent::GetEnergy() {
	return energy;
}

double Agent::GetWaste() {
	return waste;
}

float Agent::GetEnergyPercent() {
	return energy / maxEnergy;
}

float Agent::GetAge() {
	return age;
}

float Agent::GetHealthPercent() {
	return health / maxHealth;
}


