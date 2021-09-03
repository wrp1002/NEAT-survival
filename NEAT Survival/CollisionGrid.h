#pragma once
#include <allegro5/allegro_primitives.h>

#include <vector>
#include <iostream>

#include "Object.h"
#include "Globals.h"
#include "Font.h"
#include "Camera.h"
#include "Vector2f.h"

using namespace std;

class CollisionGrid {
private:
	static const int cellSize = (Globals::mapWidth + Globals::mapHeight) / 2 / 50;
	static const int width = Globals::mapWidth / cellSize;
	static const int height = Globals::mapHeight / cellSize;
	vector<shared_ptr<Object>> objects[width][height];

	void _RemoveObject(shared_ptr<Object> object, int x, int y);

public:
	CollisionGrid();

	void Draw();
	void HandleCollisions();
	void Clear();
	void AddObject(shared_ptr<Object> object);
	void UpdateObjectPosition(shared_ptr<Object> object);

	bool InBounds(int x, int y);

	shared_ptr<Object> GetCollidingObject(Vector2f pos);
	vector<shared_ptr<Object>> GetObjects(int xPos, int yPos, int r);
	vector<shared_ptr<Object>> GetObjects(Vector2f pos, int r);

};

