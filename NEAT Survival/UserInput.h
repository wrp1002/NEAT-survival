#pragma once
#include <unordered_map>
#include "Vector2f.h"

using namespace std;

namespace UserInput {
	extern unordered_map<int, bool> keyStates;
	extern Vector2f dragStartPos;
	extern Vector2f mousePos;
	extern bool isDragging;
	extern int mouseWheel;

	void StartDragging(float x, float y);
	void StartDragging(Vector2f pos);
	void StopDragging();

	void SetPressed(int key, bool pressed);
	bool IsPressed(int key);
	void SetMousePos(Vector2f pos);
	void SetMousePos(float x, float y);
};

