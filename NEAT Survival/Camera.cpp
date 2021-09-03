#include "Camera.h"

Vector2f Camera::pos(Globals::mapWidth / 2 - Globals::screenWidth / 2, Globals::mapHeight / 2 - Globals::screenHeight / 2);
float Camera::zoom = 1.0;
float Camera::zoomFactor = 0.2;
float Camera::minZoom = 0.15;
weak_ptr<Object> Camera::followObject;


void Camera::UpdateTransform() {
	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);

	//al_translate_transform(&t, -followObject->GetPos().x, -followObject->GetPos().y);

	Vector2f cameraPos = CalculatePos();

	al_translate_transform(&t, -cameraPos.x - Globals::screenWidth / 2, -cameraPos.y - Globals::screenHeight / 2);

	al_scale_transform(&t, zoom, zoom);
	al_translate_transform(&t, -cameraPos.x, -cameraPos.y);

	//if (followObject)
	//	al_translate_transform(&t, followObject->GetPos().x, followObject->GetPos().y);
	//else
	al_translate_transform(&t, cameraPos.x + Globals::screenWidth / 2, cameraPos.y + Globals::screenHeight / 2);
	
	al_use_transform(&t);
}

void Camera::UpdateZoom(int diff) {
	if (diff == 0)
		return;

	if (zoom <= zoomFactor && diff < 0)
		return;

	Vector2f mouseDiff = Vector2f(Globals::screenWidth / 2, Globals::screenHeight / 2) * zoomFactor;  // -UserInput::mousePos;

	if (diff > 0) {
		ZoomIn();
	}
	else {
		ZoomOut();
		mouseDiff *= -1;
	}

	//pos += mouseDiff;// *zoomFactor;

}

void Camera::ZoomIn() {
	zoom += zoomFactor;
}

void Camera::ZoomOut() {
	zoom -= zoomFactor;
	if (zoom < minZoom)
		zoom = minZoom;
}

void Camera::FollowObject(shared_ptr<Object> obj) {
	followObject = obj;
}

Vector2f Camera::CalculatePos() {
	if (!followObject.expired()) {
		pos = followObject.lock()->GetPos() - Vector2f(Globals::screenWidth, Globals::screenHeight) / 2;
		return pos;
	}
	else {
		if (!UserInput::isDragging)
			return pos;

		Vector2f calculatedPos = pos;
		calculatedPos = pos + (UserInput::dragStartPos - UserInput::mousePos) / zoom;
		return calculatedPos;
	}
}

Vector2f Camera::ScreenPos2WorldPos(Vector2f screenPos) {
	return Vector2f(
		pos.x + Globals::screenWidth  / 2 - Globals::screenWidth  / 2 / zoom + screenPos.x / zoom,
		pos.y + Globals::screenHeight / 2 - Globals::screenHeight / 2 / zoom + screenPos.y / zoom
	);
}


