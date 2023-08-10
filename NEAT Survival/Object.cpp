#include "Object.h"

#include "Globals.h"
#include "GameManager.h"

Object::Object() : pos(0,0), vel(0,0), acc(0,0) {
	this->radius = 0;
	this->prevPos = pos;
	alive = true;

	SetColor(al_map_rgb(255, 0, 255));
}

Object::Object(float x, float y, float radius) : pos(x, y), vel(0, 0), acc(0, 0) {
	this->radius = radius;
	this->prevPos = pos;
	alive = true;
	SetColor(al_map_rgb(255, 0, 255));
	//cout << "Object init()!" << endl;
}

Object::~Object()
{
}

void Object::Draw() {
	al_draw_filled_circle(pos.x, pos.y, radius, color);
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

vector<shared_ptr<Object>> Object::GetNearbyObjects(int range, string type) {
	return GameManager::collisionGrid.GetObjects(pos, range, type);
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

void Object::SetColor(ALLEGRO_COLOR newColor) {
	color = newColor;
	al_unmap_rgb_f(color, &colorRGB[0], &colorRGB[1], &colorRGB[2]);
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

float Object::GetDir() {
	return dir;
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

float Object::GetHue() {
	float r, g, b;
	float hue, saturation, lightness;
	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &hue, &saturation, &lightness);
	return hue;
}

float Object::GetLightness() {
	float r, g, b;
	float hue, saturation, lightness;
	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &hue, &saturation, &lightness);
	return lightness;
}

float* Object::GetRGB() {
	return colorRGB;
}

float Object::GetR() {
	return colorRGB[0];
}

float Object::GetG() {
	return colorRGB[1];
}

float Object::GetB() {
	return colorRGB[2];
}

void Object::SetAlive(bool a) {
	alive = a;
}

void Object::SetRadius(float newRadius) {
	radius = newRadius;
}

void Object::SetX(float x) {
	pos.x = x;
}

void Object::SetY(float y) {
	pos.y = y;
}
