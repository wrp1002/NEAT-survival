#pragma once
#include <allegro5/allegro.h>
#include "UserInput.h"
#include "Vector2f.h"
#include "Globals.h"
#include "Object.h"


namespace Camera {
	extern Vector2f pos;
	extern float zoom;
	extern float zoomFactor;
	extern float minZoom;
	extern weak_ptr<Object> followObject;

	void UpdateTransform();
	void UpdateZoom(int diff);
	void ZoomIn();
	void ZoomOut();
	void FollowObject(shared_ptr<Object> obj);

	Vector2f CalculatePos();
	Vector2f ScreenPos2WorldPos(Vector2f screenPos);
};

