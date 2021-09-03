#include "Object.h"

Object::Object() : pos(0,0), vel(0,0), acc(0,0) {
	this->radius = 0;
	this->prevPos = pos;
	alive = true;
}

Object::Object(float x, float y, float radius) : pos(x, y), vel(0, 0), acc(0, 0) {
	this->radius = radius;
	this->prevPos = pos;
	alive = true;
	//cout << "Object init()!" << endl;
}

Object::~Object()
{
}

void Object::Draw() {
	al_draw_filled_circle(pos.x, pos.y, radius, al_map_rgb(255, 0, 255));
}

void Object::Update() {
	prevPos = pos;
	pos += vel;
	vel *= dragCoef;

	// screen wrap
	int borderLimit = -50;
	if (pos.x < -borderLimit)
		pos.x += Globals::mapWidth + borderLimit * 2;
	else if (pos.x > Globals::mapWidth + borderLimit)
		pos.x -= Globals::mapWidth + borderLimit * 2;
	if (pos.y < -borderLimit)
		pos.y += Globals::mapHeight + borderLimit * 2;
	else if (pos.y > Globals::mapHeight + borderLimit)
		pos.y -= Globals::mapHeight + borderLimit * 2;
}

bool Object::CollidesWith(shared_ptr<Object> other) {
	return (pos.GetDistance(other->GetPos()) < (radius + other->GetRadius()));
}

bool Object::CollidesWith(Vector2f point) {
	return (pos.GetDistance(point) < radius);
}

void Object::HandleCollision(shared_ptr<Object> other) {
	Vector2f otherPos = other->GetPos();
	float overlapDistance = pos.GetDistance(otherPos) - radius - other->GetRadius();
	float angle = this->GetAngleTo(other);
	pos.x += cos(angle) * overlapDistance / 2;
	pos.y -= sin(angle) * overlapDistance / 2;

	other->SetX(otherPos.x + cos(angle + M_PI) * overlapDistance / 2);
	other->SetY(otherPos.y - sin(angle + M_PI) * overlapDistance / 2);
}
void Object::CollisionEvent(shared_ptr<Object> object) {}

void Object::Print() {
	cout << "OBject" << endl;
}


float Object::GetAngleTo(shared_ptr<Object> other) {
	return GetAngleTo(other->GetPos());
}

float Object::GetAngleTo(Vector2f other) {
	return pos.GetAngleTo(other);
}

bool Object::IsAlive() {
	return alive;
}

Vector2f Object::GetPos() {
	return pos;
}

Vector2f Object::GetPrevPos() {
	return prevPos;
}

string Object::GetType()
{
	return objectType;
}

float Object::GetRadius() {
	return radius;
}

double Object::GetEnergy() {
	return 0.0;
}

void Object::SetAlive(bool a) {
	alive = a;
}

void Object::SetX(float x) {
	pos.x = x;
}

void Object::SetY(float y) {
	pos.y = y;
}
