#include "Eye.h"
#include "Agent.h"

bool Eye::Intersects(shared_ptr<Object> object, Vector2f startPos, Vector2f endPos) {
	// Check if eye is facing towards object
	float dirToObject = pos.GetAngleTo(object->GetPos());
	float diff = abs(fmod(dir - dirToObject, 2*M_PI));
	if (diff > M_PI / 2)
		return false;

	// Check if eye intersects object
	float a = startPos.y - endPos.y;
	float b = endPos.x - startPos.x;
	float c = (startPos.x - endPos.x) * startPos.y + startPos.x * (endPos.y - startPos.y);

	Vector2f objectPos = object->GetPos();
	float distance = abs(a * objectPos.x + b * objectPos.y + c) / sqrt(a * a + b * b);
	float radiusCheck = object->GetRadius() * 2;

	return (distance < radiusCheck);
}

Eye::Eye(weak_ptr<Agent> parent, float viewDistance, float deltaDir) {
	this->parentPtr = parent;
	this->viewDistance = viewDistance;
	this->deltaDir = deltaDir;
	this->dir = 0;
	this->movementPercent = 1.0;
}

void Eye::Update(vector<shared_ptr<Object>> nearbyObjects, float newMovementPercent) {
	shared_ptr<Agent> parent = parentPtr.lock();
	if (!parent)
		return;

	movementPercent = newMovementPercent;
	dir = parent->GetDir() + deltaDir * movementPercent;
	pos = parent->GetPos() + Vector2f::FromDir(dir, 1) * parent->GetRadius();
	endPos = pos + Vector2f::FromDir(dir, 1) * viewDistance;

	float minDistance = viewDistance;
	viewedObject.reset();
	viewedObjectDistance = viewDistance;
	viewedHue = 0;
	viewedLightness = 0;
	viewedR = 0;
	viewedG = 0;
	viewedB = 0;

	for (auto object : nearbyObjects) {
		if (object == parentPtr.lock())
			continue;

		if (Intersects(object, pos, endPos)) {
			float distance = pos.GetDistance(object->GetPos());
			if (distance < minDistance && distance < viewDistance) {
				viewedObject = object;
				minDistance = distance;
			}
		}
	}

	if (!viewedObject.expired()) {
		endPos = pos + Vector2f::FromDir(dir, minDistance);
		viewedObjectDistance = minDistance;
		shared_ptr<Object> obj = viewedObject.lock();
		viewedHue = obj->GetHue();
		viewedHue = obj->GetLightness();
		viewedR = obj->GetR();
		viewedG = obj->GetG();
		viewedB = obj->GetB();
	}
}

void Eye::Draw() {
	al_draw_filled_circle(pos.x, pos.y, 3, al_map_rgb(0, 255, 0));
	al_draw_line(pos.x, pos.y, endPos.x, endPos.y, al_map_rgb(100, 100, 100), 2);
}

float Eye::GetDistanceScaled() {
	return viewedObjectDistance / viewDistance;
}

float Eye::GetViewedHue() {
	return viewedHue;
}

float Eye::GetViewedLightness() {
	return viewedLightness;
}

float Eye::GetDir() {
	return deltaDir * movementPercent;
}

float Eye::GetViewedR() {
	return viewedR;
}

float Eye::GetViewedG() {
	return viewedG;
}

float Eye::GetViewedB() {
	return viewedB;
}

shared_ptr<Object> Eye::GetViewedObject() {
	return viewedObject.lock();
}
