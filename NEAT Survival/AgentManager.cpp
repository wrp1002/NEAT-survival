#include "AgentManager.h"

vector<shared_ptr<Object>> GameManager::allObjects;
vector<shared_ptr<Agent>> GameManager::agents;
vector<shared_ptr<Food>> GameManager::allFood;
weak_ptr<Agent> GameManager::player;
CollisionGrid GameManager::collisionGrid;
unsigned GameManager::startingFood = 300;
unsigned GameManager::startingAgents = 200;

shared_ptr<Agent> GameManager::SpawnAgent(float x, float y) {
	vector<string> inputLabels = { 
		"const",
		"dir to food", 
		"sees food", 
		"energy",
		"sine", 
		"tick", 
		"dir to agent", 
		"sees agent",
		"age",
		"health" 
	};
	vector<string> outputLabels = { "forward speed", "turn", "want to reproduce", "wants to heal" };

	shared_ptr<Agent> agent = make_shared<Agent>(Agent(x, y, 10, inputLabels, outputLabels));
	agents.push_back(agent);
	allObjects.push_back(agent);
	return agent;
}

shared_ptr<Agent> GameManager::SpawnRandomAgent() {
	shared_ptr<Agent> newAgent = SpawnAgent(rand() % Globals::mapWidth, rand() % Globals::mapHeight);

	for (int i = 0; i < 150; i++)
		newAgent->Mutate();

	return newAgent;
}

void GameManager::TogglePlayer() {
	if (player.expired())
		SpawnPlayer();
	else
		DestroyPlayer();
}

shared_ptr<Agent> GameManager::SpawnAgent(float x, float y, shared_ptr<NEAT> nn) {
	shared_ptr<Agent> agent = make_shared<Agent>(Agent(x, y, 10, nn));
	agents.push_back(agent);
	allObjects.push_back(agent);
	return agent;
}

void GameManager::SpawnPlayer() {
	player = SpawnRandomAgent();
	player.lock()->SetUserControlled(true);
}

void GameManager::DestroyPlayer() {
	DestroyObject(player.lock());
}

void GameManager::SpawnFood() {
	SpawnFood(Vector2f(Globals::RandomInt(0, Globals::mapWidth), Globals::RandomInt(0, Globals::mapHeight)), Food::MAX_ENERGY);
}

void GameManager::SpawnFood(Vector2f pos, double energy) {
	shared_ptr<Food> newFood = make_shared<Food>(Food(pos, energy));
	allFood.push_back(newFood);
	allObjects.push_back(newFood);
}

void GameManager::Update() {
	bool timeOutput = false;

	
	auto start = high_resolution_clock::now();
	collisionGrid.Clear();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	if (timeOutput)
		cout << "Clearing collision grid took " << duration.count() << " microseconds" << endl;

	start = high_resolution_clock::now();
	for (auto object : allObjects)
		collisionGrid.AddObject(object);
	stop = high_resolution_clock::now();
	duration = duration_cast<microseconds>(stop - start);
	if (timeOutput)
	cout << "Adding objects to grid took " << duration.count() << " microseconds" << endl;

	start = high_resolution_clock::now();
	unsigned size = allObjects.size();
	for (unsigned i = 0; i < size; i++) {
		if (allObjects[i])
			allObjects[i]->Update();
	}
	stop = high_resolution_clock::now();
	duration = duration_cast<microseconds>(stop - start);
	if (timeOutput)
	cout << "Updating objects took " << duration.count() << " microseconds" << endl;


	start = high_resolution_clock::now();
	collisionGrid.HandleCollisions();
	stop = high_resolution_clock::now();
	duration = duration_cast<microseconds>(stop - start);
	if (timeOutput)
		cout << "handling collision took " << duration.count() << " microseconds" << endl;

	
	if (agents.size() == 0)
		Reset();

	start = high_resolution_clock::now();
	CleanupObjects();
	stop = high_resolution_clock::now();
	duration = duration_cast<microseconds>(stop - start);
	if (timeOutput)
		cout << "cleanup took " << duration.count() << " microseconds" << endl << endl;
}

void GameManager::DestroyObject(shared_ptr<Object> object) {
	for (unsigned i = 0; i < agents.size(); i++) {
		if (agents[i] == object) {
			agents.erase(agents.begin() + i);
			break;
		}
	}
	for (unsigned i = 0; i < allObjects.size(); i++) {
		if (allObjects[i] == object) {
			allObjects.erase(allObjects.begin() + i);
			break;
		}
	}
	for (unsigned i = 0; i < allFood.size(); i++) {
		if (allFood[i] == object) {
			allFood.erase(allFood.begin() + i);
			break;
		}
	}
}

void GameManager::CleanupObjects() {
	for (int i = allObjects.size() - 1; i >= 0; i--) {
		if (!allObjects[i]->IsAlive())
			DestroyObject(allObjects[i]);
	}
}

void GameManager::Draw() {
	collisionGrid.Draw();

	for (unsigned i = 0; i < allObjects.size(); i++)
		allObjects[i]->Draw();
}


void GameManager::Reset() {
	allObjects.clear();
	agents.clear();
	allFood.clear();

	while (allFood.size() < startingFood)
		SpawnFood();

	while (agents.size() < startingAgents)
		SpawnRandomAgent();

}

