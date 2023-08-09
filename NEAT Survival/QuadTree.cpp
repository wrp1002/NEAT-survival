#include "QuadTree.h"

#include "Font.h"
#include "Object.h"

QuadTree::QuadTree(int w, int h) {
	this->x = 0;
	this->y = 0;
	this->w = w;
	this->h = h;
	parent = nullptr;
}

QuadTree::QuadTree(int x, int y, int w, int h) {
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	parent = nullptr;
}

QuadTree::~QuadTree() {
}

void QuadTree::AddObject(Object* object) {
	if (children.size() > 0) {
		if (AddObjectToChildren(object))
			return;
		else
			objects.push_back(object);
	}
	else {
		objects.push_back(object);
		if (objects.size() >= maxObjects)
			Split();
	}
}

bool QuadTree::AddObjectToChildren(Object* object) {
	for (unsigned int i = 0; i < children.size(); i++) {
		if (children[i].ObjectInBounds(object)) {
			children[i].AddObject(object);
			return true;
		}
	}
	return false;
}

void QuadTree::Split() {
	children.push_back(QuadTree(x, y, w / 2, h / 2));
	children.push_back(QuadTree(x + w / 2, y, w / 2, h / 2));
	children.push_back(QuadTree(x, y + h / 2, w / 2, h / 2));
	children.push_back(QuadTree(x + w / 2, y + h / 2, w / 2, h / 2));

	vector<Object*> splitObjects = objects;
	objects.clear();

	for (Object* object : splitObjects) {
		AddObject(object);
	}
}

bool QuadTree::ObjectInBounds(Object* object) {
	Vector2f objectPos = object->GetPos();
	float radius = object->GetRadius();
	return (objectPos.x - radius >= x && objectPos.x + radius <= x + w && objectPos.y - radius >= y && objectPos.y + radius <= y + h);
}

void QuadTree::HandleCollisions() {
	for (unsigned int i = 0; i < children.size(); i++)
		children[i].HandleCollisions();

	if (objects.size() < 2)
		return;

	/*
	for (unsigned i = 0; i < objects.size() - 1; i++) {
		for (unsigned j = i + 1; j < objects.size(); j++) {
			if (objects[i]->CollidesWith(objects[j])) {
				objects[i]->HandleCollision(objects[j]);
			}
		}
	}
	*/
}

void QuadTree::Draw() {
	if (children.size() > 0) {
		for (QuadTree child : children)
			child.Draw();
	}
	al_draw_rectangle(x, y, x + w, y + h, al_map_rgb(100, 100, 100), 5);
	al_draw_textf(Font::GetFont("Minecraft.ttf", 14), al_map_rgb(255, 255, 255), x + w / 2, y + h / 2, ALLEGRO_ALIGN_CENTER, "%i", objects.size());
}

void QuadTree::Clear() {
	children.clear();
	objects.clear();
}
