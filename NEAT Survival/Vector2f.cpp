#include "Vector2f.h"


Vector2f::Vector2f() {
	this->x = 0;
	this->y = 0;
}

Vector2f::Vector2f(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector2f Vector2f::FromDir(float dir, float mag) {
	return Vector2f(cos(dir) * mag, -sin(dir) * mag);
}

float Vector2f::GetDistance(Vector2f other) {
	double diffX = (double)x - other.x;
	double diffY = (double)y - other.y;

	return sqrt(diffX * diffX + diffY * diffY);
}

float Vector2f::GetAngleTo(Vector2f other) {
	return atan2(-double(other.y) + y, double(other.x) - x);
}

Vector2f Vector2f::operator+(const Vector2f other) {
	Vector2f newPos(x, y);
	newPos += other;
	return newPos;
}

Vector2f Vector2f::operator-(const Vector2f other) {
	Vector2f newPos(x, y);
	newPos -= other;
	return newPos;
}

Vector2f Vector2f::operator*(float val) {
	Vector2f newPos(x, y);
	newPos *= val;
	return newPos;
}

Vector2f Vector2f::operator/(float val) {
	Vector2f newPos(x, y);
	newPos /= val;
	return newPos;
}

void Vector2f::operator+=(const Vector2f other) {
	x += other.x;
	y += other.y;
}

void Vector2f::operator-=(const Vector2f other) {
	x -= other.x;
	y -= other.y;
}

void Vector2f::operator*=(const Vector2f other) {
	x *= other.x;
	y += other.y;
}

void Vector2f::operator*=(float val) {
	x *= val;
	y *= val;
}

void Vector2f::operator/=(const Vector2f other) {
	x /= other.x;
	y /= other.y;
}

void Vector2f::operator/=(float val) {
	x /= val;
	y /= val;
}
