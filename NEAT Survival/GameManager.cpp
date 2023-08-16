#include "GameManager.h"

#include "Globals.h"
#include "Object.h"
#include "Agent.h"
#include "Food.h"
#include "Egg.h"
#include "NEAT.h"
#include "Camera.h"
#include "InfoDisplay.h"

#include <fmt/core.h>

vector<shared_ptr<Object>> GameManager::allObjects;
vector<shared_ptr<Agent>> GameManager::agents;
vector<shared_ptr<Food>> GameManager::allFood;
vector<shared_ptr<Egg>> GameManager::eggs;
weak_ptr<Agent> GameManager::player;
CollisionGrid GameManager::collisionGrid;
ThreadPool GameManager::threadPool;
unsigned GameManager::startingFood = 500;
unsigned GameManager::startingAgents = 200;
unsigned GameManager::speed = 1;
bool GameManager::paused = false;
double GameManager::simStartTime = 0;
double GameManager::simTicks = 0;
int GameManager::startingMutations = 20;

shared_ptr<Agent> GameManager::SpawnAgent(float x, float y) {
	vector<string> inputLabels = {
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
		"can bite",
		"object in mouth",
		"dir to bit obj",
		"dist to bit obj",
		"bit obj is food",
		"bit obj is waste",
		"eye spread",
	};
	for (int i = 1; i <= 3; i++) {
		inputLabels.push_back(fmt::format("eye{} min dist", i));
		inputLabels.push_back(fmt::format("eye{} # seen", i));
		inputLabels.push_back(fmt::format("eye{} avg r", i));
		inputLabels.push_back(fmt::format("eye{} avg g", i));
		inputLabels.push_back(fmt::format("eye{} avg b", i));
	}

	vector<string> outputLabels = {
		"forward speed",
		"turn",
		"unused",
		"want to reproduce",
		"wants to heal",
		"mem1",
		"mem1 en",
		"wants to eat",
		"wants to boost",
		"eye movement",
		"r",
		"g",
		"b",
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


shared_ptr<Egg> GameManager::GetEggFromAgent(shared_ptr<Agent> agent) {
	shared_ptr<NEAT> newNN = agent->CopyNN();
	newNN->Mutate();

	AgentStats stats = AgentStats(agent->GetAgentStats());
	stats.Mutate();

	vector<float> genes = stats.genes;
	double eggEnergy = agent->GetEnergy() / 2.0;
	int generation = agent->GetGeneration() + 1;
	float radius = agent->GetRadius() / 2;

	shared_ptr<Egg> newEgg = make_shared<Egg>(Egg(genes, newNN, eggEnergy / 2, eggEnergy / 2, generation, agent->GetPos(), radius));
	agent->SetEnergy(eggEnergy);

	return newEgg;
}


shared_ptr<Agent> GameManager::GetAgentFromEgg(shared_ptr<Egg> egg) {
	Vector2f pos = egg->GetPos();
	vector<float> genes = egg->GetGenes();
	shared_ptr<NEAT> nn = egg->GetNN();
	int generation = egg->GetGeneration();
	double health = egg->GetHealth();
	double energy = egg->GetEnergy();

	shared_ptr<Agent> newAgent = make_shared<Agent>(Agent(pos.x, pos.y, 10, nn));
	newAgent->Init();
	newAgent->SetGenes(genes);
	newAgent->SetGeneration(generation);
	newAgent->SetEnergy(energy);
	newAgent->SetHealth(health);

	return newAgent;
}


void GameManager::SpawnFood(Vector2f pos, double energy, int type) {
	shared_ptr<Food> newFood = make_shared<Food>(Food(pos, energy, type));
	allFood.push_back(newFood);
	allObjects.push_back(newFood);
}

void GameManager::SpawnFood() {
	SpawnFood(Vector2f(Globals::RandomInt(0, Globals::mapWidth), Globals::RandomInt(0, Globals::mapHeight)), Food::STARTING_ENERGY);
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
	Camera::FollowObject(player);
    InfoDisplay::SelectObject(player);
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

		// add objects to collision grid
		for (auto object : allObjects)
			collisionGrid.AddObject(object);

		stop = high_resolution_clock::now();
		duration = duration_cast<microseconds>(stop - start);
		if (timeOutput)
			cout << "Adding objects to grid took " << duration.count() << " microseconds" << endl;

		start = high_resolution_clock::now();

		vector<shared_ptr<Object>> newObjects;

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

			vector <function<void()>> jobs;

			for (unsigned i = 0; i < allObjects.size(); i++)
				jobs.push_back(bind(&GameManager::UpdateObject, allObjects[i]));

			//cout << jobs.size() << " update jobs" << endl;

			threadPool.AddJobs(jobs);

			//while (threadPool.HasJobs()) {}
		}
		else {
			// not using threads
			unsigned size = allObjects.size();
			for (unsigned i = 0; i < size; i++) {
				if (allObjects[i]) {
					allObjects[i]->Update();

					if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(allObjects[i])) {
						if (agent->ShouldReproduce()) {
							newObjects.push_back(GetEggFromAgent(agent));
						}
					}

					if (shared_ptr<Egg> egg = dynamic_pointer_cast<Egg>(allObjects[i])) {
						if (egg->ReadyToHatch()) {
							newObjects.push_back(GetAgentFromEgg(egg));
							egg->SetAlive(false);
						}
					}

				}
			}
		}

		// add new objects
		for (unsigned i = 0; i < newObjects.size(); i++) {
			allObjects.push_back(newObjects[i]);

			if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(newObjects[i]))
				agents.push_back(agent);
			if (shared_ptr<Food> food = dynamic_pointer_cast<Food>(newObjects[i]))
				allFood.push_back(food);
			if (shared_ptr<Egg> egg = dynamic_pointer_cast<Egg>(newObjects[i]))
				eggs.push_back(egg);
		}


		stop = high_resolution_clock::now();
		duration = duration_cast<microseconds>(stop - start);
		if (timeOutput)
			cout << "Updating objects took " << duration.count() << " microseconds" << endl;


		start = high_resolution_clock::now();

		if (useThreads) {
			vector <function<void()>> jobs;

			for (int x = 0; x < collisionGrid.GetWidth(); x++) {
				for (int y = 0; y < collisionGrid.GetHeight(); y++) {
					jobs.push_back(bind(&GameManager::HandleCollision, x, y));
				}
			}

			//cout << jobs.size() << " collision jobs" << endl;

			threadPool.AddJobs(jobs);
			//while (threadPool.HasJobs()) {}
		}
		else
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

		simTicks++;
	}
}

void GameManager::UpdateObject(shared_ptr<Object> obj) {
	obj->Update();
}

void GameManager::HandleCollision(int x, int y) {
	collisionGrid.HandleCollisionAt(x, y);
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

		if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(object)) {
			total += agent->GetWaste();
			total += agent->GetHealth();
		}

		if (shared_ptr<Egg> egg= dynamic_pointer_cast<Egg>(object)) {
			total += egg->GetHealth();
		}
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

	return fmt::format("{:.2f} {}", simTime, simTimeLabel);
}

string GameManager::GetSimTicksStr() {
	double simTime = simTicks / 60;
	string simTimeLabel = "sec";

	if (simTime > 3600) {
		simTime /= 3600;
		simTimeLabel = "hr";
	}
	else if (simTime > 60) {
		simTime /= 60;
		simTimeLabel = "min";
	}

	return fmt::format("{:.2f} {}", simTime, simTimeLabel);
}

shared_ptr<Agent> GameManager::GetRandomAgent() {
	if (agents.size() == 0)
		return nullptr;

	return agents[rand() % agents.size()];
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
	simTicks = 0;
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

void GameManager::Shutdown() {
	threadPool.Stop();
}
