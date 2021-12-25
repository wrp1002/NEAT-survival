#pragma once
#include <allegro5/allegro_primitives.h>

#include <memory>
#include <vector>

#include "Vector2f.h"
#include "Object.h"

class Agent;

using namespace std;

class Eye {
private:
	weak_ptr<Agent> parentPtr;
	weak_ptr<Object> viewedObject;
	Vector2f pos;
	float viewedObjectDistance;
	float viewDistance;
	float dir;
	float deltaDir;
	float viewedHue;
	float viewedLightness;
	float viewedR;
	float viewedG;
	float viewedB;

	bool Intersects(shared_ptr<Object> object, Vector2f startPos, Vector2f endPos);

public:
	Eye(weak_ptr<Agent> parent, float viewDistance, float dir);

	void UpdatePosition();
	void UpdateAngle(float movementPercent);
	void Update(vector<shared_ptr<Object>> nearbyObjects);
	void Draw();

	float GetDistanceScaled();
	float GetViewedHue();
	float GetViewedLightness();
	float GetDir();
	float GetViewedR();
	float GetViewedG();
	float GetViewedB();

	shared_ptr<Object> GetViewedObject();
};

