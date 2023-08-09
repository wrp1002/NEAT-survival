#pragma once
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <vector>
#include <memory>

#include "Object.h"
#include "Vector2f.h"


class TriangleEye {
private:
	Vector2f pos;
	Vector2f posL;
	Vector2f posR;
	float dir;
	float offsetDir;
	float maxSpread;
	float spreadPercent;
	float viewDistance;
	vector<shared_ptr<Object>> seenObjects;
	weak_ptr<Object> parentPtr;
	float avgR;
	float avgG;
	float avgB;
	float minDist;

	float sign(Vector2f p1, Vector2f p2, Vector2f p3);
	bool PointInTriangle(Vector2f pt, Vector2f v1, Vector2f v2, Vector2f v3);

public:
	TriangleEye(float maxSpread, float viewDistance, float offset, weak_ptr<Object> parent);

	void DrawLines();
	void DrawTriangle();

	void UpdatePos();
	void Update(vector<shared_ptr<Object>> nearbyObjects);

	bool ObjectInView(shared_ptr<Object> object);

	void SetSpreadPercent(float percent);

	vector<shared_ptr<Object>> GetSeenObjects();
	int GetSeenObjectsSize();
	float GetAvgR();
	float GetAvgG();
	float GetAvgB();
	float GetMinDistPercent();
};

