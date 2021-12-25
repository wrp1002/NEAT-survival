#include "TriangleEye.h"

float TriangleEye::sign(Vector2f p1, Vector2f p2, Vector2f p3) {
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool TriangleEye::PointInTriangle(Vector2f pt, Vector2f v1, Vector2f v2, Vector2f v3) {
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

TriangleEye::TriangleEye(float maxSpread, float viewDistance, float offset, weak_ptr<Object> parent) {
	this->maxSpread = maxSpread;
	this->viewDistance = viewDistance;
	this->offsetDir = offset;
	this->parentPtr = parent;
	dir = 0;
	spreadPercent = 0;
	avgR = 0;
	avgG = 0;
	avgB = 0;
	minDist = viewDistance;
}

void TriangleEye::DrawLines() {
	al_draw_line(
		pos.x,
		pos.y,
		posL.x,
		posL.y,
		al_map_rgb(50, 50, 50),
		2
	);
	al_draw_line(
		pos.x,
		pos.y,
		posR.x,
		posR.y,
		al_map_rgb(50, 50, 50),
		2
	);

	Vector2f minDistPos = pos + Vector2f::FromDir(dir, minDist);
	al_draw_line(
		pos.x,
		pos.y,
		minDistPos.x,
		minDistPos.y,
		al_map_rgb(150, 150, 150),
		2
	);
}

void TriangleEye::DrawTriangle() {
	ALLEGRO_COLOR color = al_map_rgba_f(0.1, 0.1, 0.1, 0.1);
	if (seenObjects.size() > 0)
		color = al_map_rgba_f(avgR / 2, avgG / 2, avgB / 2, 0.1);
	al_draw_filled_triangle(pos.x, pos.y, posL.x, posL.y, posR.x, posR.y, color);
}

void TriangleEye::UpdatePos() {
	shared_ptr<Object> parent = parentPtr.lock();
	if (!parent) {
		cout << "TriangleEye tried to update position with null parent" << endl;
		return;
	}

	this->pos = parent->GetPos(); //+ Vector2f::FromDir(parent->GetDir(), parent->GetRadius());
	this->dir = parent->GetDir() - offsetDir * spreadPercent;

	float dir1 = dir + maxSpread * spreadPercent;
	float dir2 = dir - maxSpread * spreadPercent;
	posL = pos + Vector2f::FromDir(dir1, viewDistance);
	posR = pos + Vector2f::FromDir(dir2, viewDistance);
}

void TriangleEye::Update(vector<shared_ptr<Object>> nearbyObjects) {
	seenObjects.clear();
	avgR = avgG = avgB = 0;
	minDist = viewDistance;

	shared_ptr<Object> parent = parentPtr.lock();
	if (!parent) {
		cout << "TriangleEye tried to update with null parent" << endl;
		return;
	}

	for (shared_ptr<Object> object : nearbyObjects) {
		if (object == parent)
			continue;

		if (ObjectInView(object)) {
			seenObjects.push_back(object);
			avgR += object->GetR();
			avgG += object->GetG();
			avgB += object->GetB();
			float dist = parent->GetPos().GetDistance(object->GetPos());
			if (dist < minDist)
				minDist = dist;
		}
	}

	if (seenObjects.size() > 0) {
		avgR /= seenObjects.size();
		avgG /= seenObjects.size();
		avgB /= seenObjects.size();
	}
}

vector<shared_ptr<Object>> TriangleEye::GetSeenObjects() {
	return seenObjects;
}

bool TriangleEye::ObjectInView(shared_ptr<Object> object) {
	return PointInTriangle(object->GetPos(), pos, posL, posR);
}

void TriangleEye::SetSpreadPercent(float percent) {
	this->spreadPercent = percent;
}

int TriangleEye::GetSeenObjectsSize() {
	return seenObjects.size();
}

float TriangleEye::GetAvgR() {
	return avgR;
}

float TriangleEye::GetAvgG() {
	return avgG;
}

float TriangleEye::GetAvgB() {
	return avgB;
}

float TriangleEye::GetMinDistPercent() {
	return minDist / viewDistance;
}
