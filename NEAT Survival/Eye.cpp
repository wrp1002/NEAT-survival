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
}

void Eye::UpdatePosition() {
	shared_ptr<Agent> parent = parentPtr.lock();
	if (!parent) {
		cout << "eye tried to update position with null parent" << endl;
		return;
	}

	pos = parent->GetPos() + Vector2f::FromDir(dir, parent->GetRadius());
}

void Eye::UpdateAngle(float movementPercent) {
	shared_ptr<Agent> parent = parentPtr.lock();
	if (!parent) {
		cout << "eye tried to update dir with null parent" << endl;
		return;
	}
	dir = parent->GetDir() + deltaDir * movementPercent;
}

void Eye::Update(vector<weak_ptr<Object>> nearbyObjects) {
	//movementPercent = newMovementPercent;
	float minDistance = viewDistance;
	Vector2f endPos = pos + Vector2f::FromDir(dir, viewDistance);
	viewedObject.reset();
	viewedObjectDistance = viewDistance;
	viewedHue = 0;
	viewedLightness = 0;
	viewedR = 0;
	viewedG = 0;
	viewedB = 0;

	for (auto objectPtr : nearbyObjects) {
		if (objectPtr.expired())
			continue;

		shared_ptr<Object> object = objectPtr.lock();

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
	Vector2f endPos = pos + Vector2f::FromDir(dir, viewedObjectDistance);
	al_draw_line(pos.x, pos.y, endPos.x, endPos.y, al_map_rgba(100, 100, 100, 0.5), 2);
	al_draw_filled_circle(pos.x, pos.y, 2, al_map_rgb(0, 255, 0));
	Vector2f pupilPos = pos + Vector2f::FromDir(dir, 0.5);
	al_draw_filled_circle(pupilPos.x, pupilPos.y, 1, al_map_rgb(50, 50, 50));
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
	return dir;
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
