#include "UserInput.h"

unordered_map<int, bool> UserInput::keyStates;
Vector2f UserInput::dragStartPos;
Vector2f UserInput::mousePos;
bool UserInput::isDragging = false;
int UserInput::mouseWheel = 0;

void UserInput::StartDragging(float x, float y) {
	StartDragging(Vector2f(x, y));
}

void UserInput::StartDragging(Vector2f pos) {
	isDragging = true;
	dragStartPos = pos;
}

void UserInput::StopDragging() {
	isDragging = false;
}

void UserInput::SetPressed(int key, bool pressed) {
	UserInput::keyStates[key] = pressed;
}

bool UserInput::IsPressed(int key) {
	return UserInput::keyStates[key];
}

void UserInput::SetMousePos(Vector2f pos) {
	mousePos = pos;
}

void UserInput::SetMousePos(float x, float y) {
	mousePos = Vector2f(x, y);
}

