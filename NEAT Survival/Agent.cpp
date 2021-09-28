#include "Agent.h"


void Agent::Init() {
	forwardSpeed = 0;
	rotationSpeed = 0;
	dir = Globals::RandomDir();
	objectType = "agent";
	
	generation = 1;

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
	//eyes.push_back(make_shared<Eye>(Eye(shared_from_this(), viewDistance, 0)));
	eyes.push_back(make_shared<Eye>(Eye(shared_from_this(), viewDistance,  eyeSpreadMax)));

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


	// dir to closest objects 
	//shared_ptr<Object> closestFood = GetClosestObjectOfType(nearbyObjects, "food");
	//shared_ptr<Object> closestAgent = GetClosestObjectOfType(nearbyObjects, "agent");

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



	for (auto eye : eyes)
		eye->Update(nearbyObjects);
	mouth->Update(nearbyObjects);

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
		stats.GetAgePercent(),				// age
		stats.GetHealthPercent(),			// health
		stats.GetEnergyPercent(),			// energy
		memory[0],							// mem1
		nearbyFood   / 10.0,				// nearby food
		nearbyWaste  / 10.0,				// nearby waste
		nearbyAgents / 10.0,				// nearby objects
		(double)mouth->CanBite(),			// can bite
		(double)mouth->ObjectInMouth()		// object in mouth
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
	rotationSpeed = (outputs[1]) * maxRotationSpeed;
	bool wantToReproduce = outputs[2] > 0.5 || stats.energy > 0.80;
	bool wantsToHeal = outputs[3] > 0.5;
	double mem1 = outputs[4];
	bool mem1Overwrite = outputs[5] > 0.5;
	bool wantsToEat = outputs[6] > 0.5;

	bool shouldReproduce = wantToReproduce && stats.energy > stats.maxEnergy * 0.5;


	if (mem1Overwrite) {
		memory.clear();
		memory.push_back(mem1);
	}


	if (userControlled) {
		forwardSpeed = accSpeed * (UserInput::IsPressed(ALLEGRO_KEY_UP) - UserInput::IsPressed(ALLEGRO_KEY_DOWN));
		rotationSpeed = maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_LEFT) - maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_RIGHT);
		wantsToEat = UserInput::IsPressed(ALLEGRO_KEY_E);
	}
	
	double energyUsage = 0.005;
	
	if (shouldReproduce)
		Reproduce();

	// healing
	if (wantsToHeal && stats.energy > stats.healAmount) {
		stats.health += stats.healAmount;
		stats.energy -= stats.healAmount;
		if (stats.health > stats.maxHealth) {
			double diff = stats.health - stats.maxHealth;
			stats.health -= diff;
			stats.waste += diff;
		}
	}

	energyUsage += abs(outputs[0]) * 0.01;
	energyUsage += abs(outputs[1]) * 0.01;


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
	vel += acc;
	vel *= dragCoef;


	//wantsToEat = true;
	// mouth 
	mouth->SetWantsToBite(wantsToEat);
	if (wantsToEat && mouth->CanBite() && mouth->ObjectInMouth()) {
		mouth->Bite();
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
	for (auto eye : eyes)
		eye->UpdatePosition();
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

	mouth->Draw();

	al_draw_filled_circle(pos.x, pos.y, radius, color);
	
	
	int lineLen = radius*2 + forwardSpeed;
	//al_draw_line(pos.x, pos.y, pos.x + cos(dir) * lineLen, pos.y - sin(dir) * lineLen, al_map_rgb(255, 0, 255), 2);

	//al_draw_line(pos.x, pos.y, pos.x + cos(dirToFood) * 20, pos.y - sin(dirToFood) * 20, al_map_rgb(0, 255, 0), 2);
	//al_draw_line(pos.x, pos.y, pos.x + cos(dirToAgent) * 20, pos.y - sin(dirToAgent) * 20, al_map_rgb(255, 0, 0), 2);

	for (auto eye : eyes)
		eye->Draw();
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
	child->SetGenes(genes);
	child->MutateGenes();

	double childEnergy = stats.energy / 2;
	child->SetEnergy(childEnergy / 2);
	child->SetHealth(childEnergy / 2);
	stats.energy /= 2;
}

void Agent::SetGenes(vector<float> newGenes) {
	genes = newGenes;

	// Apply color genes 0-2
	SetColor(al_map_rgb_f(genes[0], genes[1], genes[2]));

	stats = AgentStats(genes);
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

float Agent::GetHealthPercent() {
	return stats.health / stats.maxHealth;
}


