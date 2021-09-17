#include "AgentManager.h"
#include "Agent.h"
#include "Food.h"

vector<shared_ptr<Object>> GameManager::allObjects;
vector<shared_ptr<Agent>> GameManager::agents;
vector<shared_ptr<Food>> GameManager::allFood;
weak_ptr<Agent> GameManager::player;
CollisionGrid GameManager::collisionGrid;
unsigned GameManager::startingFood = 300;
unsigned GameManager::startingAgents = 200;
unsigned GameManager::speed = 1;
bool GameManager::paused = false;
double GameManager::simStartTime = 0;
int GameManager::startingMutations = 15;

shared_ptr<Agent> GameManager::SpawnAgent(float x, float y) {
	vector<string> inputLabels = { 
		//"dir to food",
		//"sees food",
		//"dir to agent",
		//"sees agent",
		//"food dist",
		//"agent dist",
		"const",
		"sin",
		"tick",
		"age",
		"health",
		"energy",
		"mem1",
		"# nearby food",
		"# nearby waste",
		"# nearby agents",

		"eye1 dist",
		"eye1 r",
		"eye1 g",
		"eye1 b",
		"eye2 dist",
		"eye2 r",
		"eye2 g",
		"eye2 b",
	};
	vector<string> outputLabels = {
		"forward speed",
		"turn",
		"want to reproduce",
		"wants to heal",
		"mem1",
		"mem1 en",
	};

	shared_ptr<Agent> agent = make_shared<Agent>(Agent(x, y, 10, inputLabels, outputLabels));
	agent->Init();
	agents.push_back(agent);
	allObjects.push_back(agent);
	return agent;
}

shared_ptr<Agent> GameManager::SpawnAgent(float x, float y, shared_ptr<NEAT> nn) {
	shared_ptr<Agent> agent = make_shared<Agent>(Agent(x, y, 10, nn));
	agent->Init();
	agents.push_back(agent);
	allObjects.push_back(agent);
	return agent;
}

shared_ptr<Agent> GameManager::SpawnRandomAgent() {
	shared_ptr<Agent> agent = SpawnAgent(rand() % Globals::mapWidth, rand() % Globals::mapHeight);

	for (int i = 0; i < startingMutations; i++)
		agent->MutateAddConnection();

	return agent;
}


void GameManager::SpawnFood(Vector2f pos, double energy, int type) {
	shared_ptr<Food> newFood = make_shared<Food>(Food(pos, energy, type));
	allFood.push_back(newFood);
	allObjects.push_back(newFood);
}

void GameManager::SpawnFood() {
	SpawnFood(Vector2f(Globals::RandomInt(0, Globals::mapWidth), Globals::RandomInt(0, Globals::mapHeight)), Food::MAX_ENERGY);
}


void GameManager::TogglePlayer() {
	if (player.expired())
		SpawnPlayer();
	else
		DestroyPlayer();
}

void GameManager::SpawnPlayer() {
	player = SpawnRandomAgent();
	player.lock()->SetUserControlled(true);
}

void GameManager::DestroyPlayer() {
	DestroyObject(player.lock());
}


void GameManager::Update() {
	if (paused)
		return;

	bool timeOutput = false;
	bool useThreads = false;
	
	for (unsigned i = 0; i < speed; i++) {
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


		if (useThreads) {
			// using threads
			/*
			vector<thread> updateThreads;


			unsigned size = allObjects.size();
			for (unsigned i = 0; i < size; i++)
				updateThreads.push_back(thread(UpdateObject, allObjects[i]));
			for (unsigned i = 0; i < size; i++)
				updateThreads[i].join();
				*/
		}
		else {
			// not using threads
			unsigned size = allObjects.size();
			for (unsigned i = 0; i < size; i++) {
				if (allObjects[i])
					allObjects[i]->Update();
			}
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

double GameManager::GetTotalEnergy() {
	double total = 0;
	for (auto object : allObjects) {
		total += object->GetEnergy();
		if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(object))
			total += agent->GetWaste();
	}
	return total;
}

double GameManager::GetSimTime() {
	return al_get_time() - simStartTime;
}

string GameManager::GetSimTimeStr() {
	double simTime = GameManager::GetSimTime();
	string simTimeLabel = "sec";

	if (simTime > 3600) {
		simTime /= 3600;
		simTimeLabel = "hr";
	}
	else if (simTime > 60) {
		simTime /= 60;
		simTimeLabel = "min";
	}

	return format("{:.2f} {}", simTime, simTimeLabel);
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

	cout << "Reset simulation after " << GetSimTimeStr() << endl;
	simStartTime = al_get_time();
}


void GameManager::ResetSpeed() {
	speed = 1;
}

void GameManager::IncreaseSpeed() {
	speed++;
}

void GameManager::DecreaseSpeed() {
	if (speed > 1)
		speed--;
}

int GameManager::GetSpeed() {
	return speed;
}


void GameManager::TogglePaused() {
	paused = !paused;
}

bool GameManager::IsPaused() {
	return paused;
}
