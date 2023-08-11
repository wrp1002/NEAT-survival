#include "CollisionGrid.h"

#include "Object.h"
#include "Font.h"
#include "Camera.h"
#include "Vector2f.h"

void CollisionGrid::_RemoveObject(shared_ptr<Object> object, int x, int y) {
	if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
		return;

	for (unsigned int i = 0; i < objects[x][y].size(); i++) {
		if (objects[x][y][i] == object) {
			objects[x][y].erase(objects[x][y].begin() + i);
			return;
		}
	}
}

CollisionGrid::CollisionGrid()
{
}

void CollisionGrid::Draw() {
	al_draw_filled_rectangle(0, 0, width * cellSize, height * cellSize, al_map_rgb(0, 10, 0));
	al_draw_rectangle(0, 0, width * cellSize, height * cellSize, al_map_rgb(100, 100, 100), 3 / Camera::zoom);

	if (!Globals::drawCollisionGrid)
		return;

	for (unsigned int y = 0; y < height; y++)
		al_draw_line(0, y * cellSize, width * cellSize, y * cellSize, al_map_rgb(50, 50, 50), 2 / Camera::zoom);
	for (unsigned int x = 0; x < width; x++)
		al_draw_line(x * cellSize, 0, x * cellSize, height * cellSize, al_map_rgb(50, 50, 50), 2 / Camera::zoom);

	for (unsigned x = 0; x < width; x++) {
		for (unsigned y = 0; y < height; y++) {
			int drawX = x * cellSize;
			int drawY = y * cellSize;
			al_draw_textf(Font::GetFont("Minecraft.ttf", 14), al_map_rgb(255, 255, 255), drawX + cellSize / 2, drawY + cellSize / 2, ALLEGRO_ALIGN_CENTER, "%li", objects[x][y].size());
		}
	}
}

void CollisionGrid::HandleCollisions() {
	for (unsigned x = 0; x < width; x++) {
		for (unsigned y = 0; y < height; y++) {
			HandleCollisionAt(x, y);
		}
	}
}

void CollisionGrid::HandleCollisionAt(int x, int y) {
	vector<shared_ptr<Object>> cellObjects = GetObjects(x, y, 1);

	if (cellObjects.size() < 2)
		return;

	for (unsigned i = 0; i < cellObjects.size() - 1; i++) {
		for (unsigned j = i + 1; j < cellObjects.size(); j++) {
			if (cellObjects[i]->CollidesWith(cellObjects[j])) {
				cellObjects[i]->HandleCollision(cellObjects[j]);
				cellObjects[i]->CollisionEvent(cellObjects[j]);
				cellObjects[j]->CollisionEvent(cellObjects[i]);
			}
		}
	}
}

void CollisionGrid::Clear() {
	for (unsigned x = 0; x < width; x++) {
		for (unsigned y = 0; y < height; y++) {
			objects[x][y].clear();
		}
	}
}

void CollisionGrid::AddObject(shared_ptr<Object> object) {
	Vector2f pos = object->GetPos();
	int x = pos.x / cellSize;
	int y = pos.y / cellSize;
	if (!InBounds(x, y))
		return;

	objects[x][y].push_back(object);
}

void CollisionGrid::UpdateObjectPosition(shared_ptr<Object> object) {
	Vector2f pos = object->GetPos();
	Vector2f prevPos = object->GetPrevPos();
	int x = pos.x / cellSize;
	int y = pos.y / cellSize;
	int prevX = prevPos.x / cellSize;
	int prevY = prevPos.y / cellSize;

	if (x != prevX || y != prevY) {
		_RemoveObject(object, prevX, prevY);
		AddObject(object);
	}
}

bool CollisionGrid::InBounds(int x, int y) {
	return (x >= 0 && x < width && y >= 0 && y < height);
}

shared_ptr<Object> CollisionGrid::GetCollidingObject(Vector2f pos) {
	vector<shared_ptr<Object>> objects = GetObjects(pos, 1);
	for (auto object : objects) {
		if (object->CollidesWith(pos))
			return object;
	}
	return nullptr;
}

vector<shared_ptr<Object>> CollisionGrid::GetObjects(int xPos, int yPos, int r, string type) {
	vector<shared_ptr<Object>> foundObjects;
	for (int x = xPos - r; x <= xPos + r; x++) {
		for (int y = yPos - r; y <= yPos + r; y++) {
			if (InBounds(x, y)) {
				if (type == "")
					foundObjects.insert(foundObjects.end(), objects[x][y].begin(), objects[x][y].end());
				else {
					// Only include objects that match the given type
					for (auto object : objects[x][y])
						if (object->GetType() == type)
							foundObjects.push_back(object);
				}
			}
		}
	}
	return foundObjects;
}

vector<shared_ptr<Object>> CollisionGrid::GetObjects(Vector2f pos, int r, string type) {
	int x = pos.x / cellSize;
	int y = pos.y / cellSize;
	return GetObjects(x, y, r, type);
}

int CollisionGrid::GetWidth() {
	return width;
}

int CollisionGrid::GetHeight() {
	return height;
}

