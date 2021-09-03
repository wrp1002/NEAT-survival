#pragma once
#include <math.h>

class Vector2f {
public:
	float x, y;

	Vector2f();
	Vector2f(float x, float y);

	static Vector2f FromDir(float dir, float mag);

	float GetDistance(Vector2f other);
	float GetAngleTo(Vector2f other);

	Vector2f operator+(const Vector2f other);
	Vector2f operator-(const Vector2f other);
	Vector2f operator*(float val);
	Vector2f operator/(float val);
	void operator+=(const Vector2f other);
	void operator-=(const Vector2f other);
	void operator*=(const Vector2f other);
	void operator*=(float val);
	void operator/=(const Vector2f other);
	void operator/=(float val);
};

