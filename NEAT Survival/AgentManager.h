#pragma once

#include <vector>
#include <iostream>
#include <chrono>

#include "Object.h"
#include "Agent.h"
#include "Food.h"
#include "Globals.h"
#include "QuadTree.h"
#include "CollisionGrid.h"
#include "NEAT.h"

class Agent;
class Food;

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

    shared_ptr<Agent> SpawnRandomAgent();
    shared_ptr<Agent> SpawnAgent(float x, float y);
    shared_ptr<Agent> SpawnAgent(float x, float y, shared_ptr<NEAT> nn);

    void TogglePlayer();
    void SpawnPlayer();
    void DestroyPlayer();

    void SpawnFood();
    void SpawnFood(Vector2f pos, double energy);

    void DestroyObject(shared_ptr<Object> object);


    void Update();
    void Draw();
    void CleanupObjects();
    void Reset();

};
