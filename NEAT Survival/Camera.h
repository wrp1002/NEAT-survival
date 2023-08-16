#pragma once
#include <allegro5/allegro.h>
#include "UserInput.h"

#include <memory>

class Vector2f;
class Object;

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
	void FollowObject(weak_ptr<Object> obj);

	Vector2f CalculatePos();
	Vector2f ScreenPos2WorldPos(Vector2f screenPos);
};

