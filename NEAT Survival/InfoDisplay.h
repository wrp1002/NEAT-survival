#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>

#include <iostream>
#include <format>
#include <string>
#include <typeinfo>

#include "Globals.h"
#include "Object.h"
#include "Font.h"
#include "AgentManager.h"
#include "Vector2f.h"
#include "Agent.h"
#include "NEAT.h"

using namespace std;

namespace InfoDisplay {
	extern ALLEGRO_DISPLAY* display;
	extern ALLEGRO_EVENT_QUEUE* event_queue;
	extern weak_ptr<Object> selectedObject;
	extern Vector2f screenSize;
	extern Vector2f mousePos;

	void Toggle();
	void Show();
	void Hide();
	void Draw();
	void SelectObject(weak_ptr<Object>);
	string DrawNN(shared_ptr<NEAT> nn);

	Vector2f CalculateNodePos(shared_ptr<Node> node);
};

