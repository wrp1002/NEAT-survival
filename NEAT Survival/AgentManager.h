#pragma once

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Globals.h"
#include "NEAT.h"
#include "Object.h"
#include "CollisionGrid.h"
#include "Food.h"

class Agent;

using namespace std;
using namespace std::chrono;

namespace GameManager {
    extern vector<shared_ptr<Object>> allObjects;
    extern vector<shared_ptr<Agent>> agents;
    extern vector<shared_ptr<Food>> allFood;
    extern weak_ptr<Agent> player;
    extern CollisionGrid collisionGrid;
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
    void SpawnFood(Vector2f pos, double energy, int type=Food::FOOD);

    void DestroyObject(shared_ptr<Object> object);


    double GetTotalEnergy();
    double GetSimTime();
    string GetSimTimeStr();
    string GetSimTicksStr();

    void ResetSpeed();
    void IncreaseSpeed();
    void DecreaseSpeed();
    int GetSpeed();

    void TogglePaused();
    bool IsPaused();

    void Update();
    void Draw();
    void CleanupObjects();
    void Reset();

};
