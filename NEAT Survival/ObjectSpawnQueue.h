#pragma once
#include <vector>

#include "Object.h"
#include "Food.h"

using std::vector;

namespace  ObjectSpawnQueue {
	extern vector<shared_ptr<Object>> newObjects;

	void AddObject(shared_ptr<Object> newObject);
	void SpawnFood(Vector2f pos, double energy, int type = Food::FOOD);
	
	shared_ptr<Object> Pop();
	void Clear();
	
	bool IsEmpty();

};

