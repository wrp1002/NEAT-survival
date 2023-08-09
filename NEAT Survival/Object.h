#pragma once

#define _USE_MATH_DEFINES
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>


#include "Vector2f.h"

using namespace std;


class Object {
protected:
	Vector2f pos;
	Vector2f prevPos;
	Vector2f vel;
	Vector2f acc;
	ALLEGRO_COLOR color;
	float colorRGB[3];
	float radius;
	float velX = 0;
	float velY = 0;
	float dir;
	const float dragCoef = 0.85;
	string objectType = "object";
	bool alive;

public:

	Object();
	Object(float x, float y, float radius);
	virtual ~Object();

	virtual void Draw();
	virtual void Update();
	virtual void HandleCollision(shared_ptr<Object> other);
	virtual void CollisionEvent(shared_ptr<Object> object);
	virtual void Print();

	void SetColor(ALLEGRO_COLOR newColor);

	bool CollidesWith(shared_ptr<Object> other);
	bool CollidesWith(Vector2f point);
	float GetAngleTo(shared_ptr<Object> other);
	float GetAngleTo(Vector2f other);
	bool IsAlive();

	Vector2f GetPos();
	Vector2f GetPrevPos();
	float GetDir();
	string GetType();
	float GetRadius();
	virtual double GetEnergy();
	float GetHue();
	float GetLightness();
	float* GetRGB();
	float GetR();
	float GetG();
	float GetB();

	void SetAlive(bool a);
	void SetX(float x);
	void SetY(float y);
};

