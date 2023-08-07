#include "ObjectSpawnQueue.h"

vector<shared_ptr<Object>> ObjectSpawnQueue::newObjects;

void ObjectSpawnQueue::AddObject(shared_ptr<Object> newObject) {
	newObjects.push_back(newObject);
}


void ObjectSpawnQueue::SpawnFood(Vector2f pos, double energy, int type) {
	shared_ptr<Food> newFood = make_shared<Food>(Food(pos, energy, type));
	newObjects.push_back(newFood);
}

shared_ptr<Object> ObjectSpawnQueue::Pop() {
	shared_ptr<Object> obj = newObjects.back();
	newObjects.pop_back();
	return obj;
}

void ObjectSpawnQueue::Clear() {
	newObjects.clear();
}

bool ObjectSpawnQueue::IsEmpty() {
	return newObjects.size() == 0;
}
