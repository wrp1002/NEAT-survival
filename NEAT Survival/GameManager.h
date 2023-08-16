#pragma once

#include <vector>
#include <iostream>
#include <chrono>
#include <functional>

#include "CollisionGrid.h"
#include "ThreadPool.h"

class Object;
class NEAT;
class Agent;
class Egg;
class Food;


using namespace std;
using namespace std::chrono;

namespace GameManager {
    extern vector<shared_ptr<Object>> allObjects;
    extern vector<shared_ptr<Agent>> agents;
    extern vector<shared_ptr<Food>> allFood;
    extern vector<shared_ptr<Egg>> eggs;
    extern weak_ptr<Agent> player;
    extern CollisionGrid collisionGrid;
    extern ThreadPool threadPool;
    extern unsigned startingFood;
    extern unsigned startingAgents;
    extern unsigned speed;
    extern bool paused;
    extern double simStartTime;
    extern double simTicks;
    extern int startingMutations;

    shared_ptr<Agent> SpawnRandomAgent();
    shared_ptr<Agent> SpawnAgent(float x, float y);
    shared_ptr<Agent> SpawnAgent(float x, float y, shared_ptr<NEAT> nn);

    void TogglePlayer();
    void SpawnPlayer();
    void DestroyPlayer();

    void SpawnFood();
    void SpawnFood(Vector2f pos, double energy, int type=0);

    void DestroyObject(shared_ptr<Object> object);


    double GetTotalEnergy();
    double GetSimTime();
    string GetSimTimeStr();
    string GetSimTicksStr();
    shared_ptr<Agent> GetRandomAgent();
    shared_ptr<Egg> GetEggFromAgent(shared_ptr<Agent> agent);
    shared_ptr<Agent> GetAgentFromEgg(shared_ptr<Egg> egg);

    void ResetSpeed();
    void IncreaseSpeed();
    void DecreaseSpeed();
    int GetSpeed();

    void TogglePaused();
    bool IsPaused();

    void Update();
    void UpdateObject(shared_ptr<Object> obj);
    void HandleCollision(int x, int y);
    void Draw();
    void CleanupObjects();
    void Reset();

    void Shutdown();
};
