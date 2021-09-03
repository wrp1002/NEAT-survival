#pragma once
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>

#include "Vector2f.h"

using namespace std;

class Connection;

class Node {
private:
	Vector2f pos;
	double output;
	int type;
	string name;
	int ID;
	vector<shared_ptr<Connection>> allConnections;
	vector<shared_ptr<Connection>> fromConnections;
	vector<shared_ptr<Connection>> toConnections;

	void _RemoveConnection(shared_ptr<Connection> connection, vector<shared_ptr<Connection>>* connections);

public:
	enum NODE_TYPE { INPUT, HIDDEN, OUTPUT };

	Node(float x, float y, int type, int ID, string name = "");
	~Node();

	void Print();

	void Calculate();
	double ActivationFunction(double input);
	void AddConnection(shared_ptr<Connection>connection);
	void RemoveConnection(shared_ptr<Connection> connection);
	void AddConnectionToNode(Node* node);
	void ClearConnections();

	Vector2f GetPos();
	vector<shared_ptr<Connection>> GetFromConnections();
	vector<shared_ptr<Connection>> GetToConnections();
	vector<shared_ptr<Connection>> GetAllConnections();

	int GetID();

	double GetOutput() const { return output; }
	void SetOutput(double val) { output = val; }
	string GetName() { return this->name; }
	int GetType() const { return type; }
	void SetType(int val) { type = val; }
};

