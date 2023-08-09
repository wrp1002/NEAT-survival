#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include <vector>
#include <iostream>

class Object;

using namespace std;

class QuadTree {
private:
	int x, y;
	int w, h;
	static const int maxObjects = 10;
	static const int minSize = 50;
	vector<Object*> objects;

	QuadTree* parent;
	vector<QuadTree> children;

public:
	QuadTree(int w, int h);
	QuadTree(int x, int y, int w, int h);
	~QuadTree();

	void HandleCollisions();
	void Draw();
	void Clear();

	void AddObject(Object* object);
	bool AddObjectToChildren(Object* object);
	void Split();
	bool ObjectInBounds(Object* object);

};

