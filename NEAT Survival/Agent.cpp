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

	color = al_map_rgb(Globals::RandomInt(0, 255), Globals::RandomInt(0, 255), Globals::RandomInt(0, 255));
}

void Agent::Print() {
	cout << "Agent energy:" << energy << " waste:" << waste << endl;
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

float Agent::GetEnergyPercent() {
	return energy / maxEnergy;
}

float Agent::GetAge() {
	return age;
}

float Agent::GetHealthPercent() {
	return health / maxHealth;
}

Agent::Agent(float x, float y, float radius, vector<string> inputLabels, vector<string> outputLabels) : Object(x, y, radius) {
	Init();
	//cout << "Agent init!" << endl;
	nn = make_shared<NEAT>(NEAT(inputLabels, outputLabels));
}

Agent::Agent(float x, float y, float radius, shared_ptr<NEAT> newNN) : Object(x, y, radius) {
	nn = newNN;
	Init();
}


Agent::~Agent() {
	
}

void Agent::Update() {
	if (!alive) {
		cout <<" tried update but not alive'" << endl;
		return;
	}


	vector<shared_ptr<Object>> nearbyObjects = GetNearbyObjects();
	shared_ptr<Object> closestFood = GetClosestObjectOfType(nearbyObjects, "food");
	shared_ptr<Object> closestAgent = GetClosestObjectOfType(nearbyObjects, "agent");

	float foodDeltaDir = 0;
	if (closestFood != nullptr) {
		dirToFood = GetAngleTo(closestFood);
		foodDeltaDir = atan2(sin(dirToFood - dir), cos(dirToFood - dir));
		//cout << "dir: " << dir << " Food dir:" << foodDir << " delta:" << foodDeltaDir << endl;
	}

	float agentDeltaDir = 0;
	if (closestAgent != nullptr) {
		dirToAgent = GetAngleTo(closestAgent);
		agentDeltaDir = atan2(sin(dirToAgent - dir), cos(dirToAgent - dir));
		//cout << "dir: " << dir << " Food dir:" << foodDir << " delta:" << foodDeltaDir << endl;
	}

	// Get output from NN
	vector<double> inputs = { 
		1.0,							// const
		foodDeltaDir,					// direction to closest food
		double(closestFood != nullptr), // sees food
		energy / maxEnergy,				// energy
		sin(al_get_time()),				// sin
		double(sin(al_get_time()) > 0),	// tick
		agentDeltaDir,
		double(closestAgent != nullptr),
		age / maxAge,
		health / maxHealth,

	};

	vector<double> outputs = nn->Calculate(inputs);


	forwardSpeed = outputs[0] * accSpeed;
	rotationSpeed = (outputs[1] * 2.0 - 1.0) * maxRotationSpeed;

	bool wantToReproduce = outputs[2] > 0.5;
	bool wantsToHeal = outputs[3] > 0.5;
	bool shouldReproduce = wantToReproduce && energy > maxEnergy * 0.5;// || energy > maxEnergy * 0.95;


	if (userControlled) {
		forwardSpeed = accSpeed * UserInput::IsPressed(ALLEGRO_KEY_UP);
		rotationSpeed = 0;
		rotationSpeed += maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_LEFT);
		rotationSpeed -= maxRotationSpeed * UserInput::IsPressed(ALLEGRO_KEY_RIGHT);
	}


	Object::Update();
	age += 1.0/60;

	
	double energyUsage = 0.01;

	
	if (shouldReproduce) {
		Reproduce();
	}
	

	if (wantsToHeal) {
		double healAmout = 0.2;
		if (energy > healAmout)
			energyUsage += healAmout;
	}

	energyUsage += outputs[0] * 0.01;
	energyUsage += outputs[1] * 0.01;


	if (energyUsage < energy) {
		// movement
		dir += rotationSpeed;
		dir = fmod(dir, 2*M_PI);

		acc = Vector2f(cos(dir) * forwardSpeed, -sin(dir) * forwardSpeed);
		vel += acc;
		vel *= dragCoef;

		if (wantsToHeal) {
			health += healAmount;
			if (health > maxHealth)
				health = maxHealth;
		}

		energy -= energyUsage;
		waste += energyUsage;
		//if (waste > maxWaste)
		//	waste = maxWaste;
	}
	else {
		health -= 0.1;

		dir += rotationSpeed * 0.5;
		dir = fmod(dir, 2 * M_PI);

		acc = Vector2f(cos(dir) * forwardSpeed, -sin(dir) * forwardSpeed) * 0.5;
		vel += acc;
		vel *= dragCoef;
	}


	if (age > maxAge)
		health -= 0.1;


	// release energy if dead
	if (health <= 0) {
		alive = false;

		while (energy >= 10) {
			GameManager::SpawnFood(pos, 10.0);
			energy -= 10;
		}
		if (energy)
			GameManager::SpawnFood(pos, energy);

		while (waste >= 10) {
			GameManager::SpawnFood(pos, 10.0);
			waste -= 10;
		}
		if (waste)
			GameManager::SpawnFood(pos, waste);
	}
}

void Agent::Draw() {
	al_draw_filled_circle(pos.x, pos.y, radius, color);
	int lineLen = radius*2 + forwardSpeed;
	al_draw_line(pos.x, pos.y, pos.x + cos(dir) * lineLen, pos.y - sin(dir) * lineLen, al_map_rgb(255, 0, 255), 2);

	al_draw_line(pos.x, pos.y, pos.x + cos(dirToFood) * 20, pos.y - sin(dirToFood) * 20, al_map_rgb(0, 255, 0), 2);
	al_draw_line(pos.x, pos.y, pos.x + cos(dirToAgent) * 20, pos.y - sin(dirToAgent) * 20, al_map_rgb(255, 0, 0), 2);
}

void Agent::CollisionEvent(shared_ptr<Object> other) {
	if (other->GetType() == "food") {
		if (!other->IsAlive() || !IsAlive())
			return;

		other->SetAlive(false);
		energy += other->GetEnergy();
		if (energy > maxEnergy) {
			double overflow = double((long double)energy - maxEnergy);
			energy -= overflow;
			waste += overflow;
		}
	}
}

void Agent::Mutate() {
	nn->Mutate();
}

void Agent::MutateAddConnection() {
	nn->MutateAddConnection();
}

float Agent::GetX() {
	return pos.x;
}

vector<shared_ptr<Object>> Agent::GetNearbyObjects() {
	vector<shared_ptr<Object>> foundObjects = GameManager::collisionGrid.GetObjects(pos, 2);
	return foundObjects;
}

shared_ptr<Object> Agent::GetClosestObjectOfType(vector<shared_ptr<Object>> nearbyObjects, string type) {
	float minDist = 100000;
	shared_ptr<Object> closest = nullptr;

	for (auto object : nearbyObjects) {
		if (object->GetType() != type || object.get() == this)
			continue;

		float distCheck = object->GetPos().GetDistance(pos);
		if (distCheck < minDist) {
			closest = object;
			minDist = distCheck;
		}
	}

	return closest;
}

float Agent::GetY() {
	return pos.y;
}

shared_ptr<NEAT> Agent::GetNN() {
	return nn;
}

void Agent::Reproduce() {
	shared_ptr<NEAT> newNN = nn->Copy();
	shared_ptr<Agent> child = GameManager::SpawnAgent(pos.x, pos.y, newNN);
	child->Mutate();
	child->SetEnergy(energy / 2);
	child->SetGeneration(generation + 1);
	energy /= 2;
}


