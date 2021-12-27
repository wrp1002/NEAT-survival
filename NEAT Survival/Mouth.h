#pragma once
#include <allegro5/allegro_primitives.h>

#include <memory>
#include <iostream>

#include "Globals.h"
#include "Object.h"
#include "Food.h"
#include "GameRules.h"

class Agent;

using namespace std;

class Mouth : public Object {
private:
	weak_ptr<Agent> parentPtr;
	weak_ptr<Object> collidingObjectPtr;
	float deltaPos;
	float dir;
	int cooldownTimer, cooldownTimerStart;
	bool wantsToBite;

public:
	Mouth(shared_ptr<Agent> parent, float radius);

	void UpdatePosition();
	void Update(vector<shared_ptr<Object>> nearbyObjects);
	void Draw();

	bool CanBite();
	void Bite();

	void SetWantsToBite(bool val);

	bool ObjectInMouth();

	weak_ptr<Object> GetObjectInMouth();
};

