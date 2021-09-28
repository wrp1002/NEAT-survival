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
	vector<shared_ptr<Connection>> allConnections;
	vector<shared_ptr<Connection>> fromConnections;
	vector<shared_ptr<Connection>> toConnections;
	int ID;
	Vector2f pos;
	int type;
	double output;
	string name;
	string activationFuncStr;
	double (Node::*ActivationFunction)(double);

	void _RemoveConnection(shared_ptr<Connection> connection, vector<shared_ptr<Connection>>* connections);
	double Tanh(double input);
	double Sigmoid(double input);

public:
	enum NODE_TYPE { INPUT, HIDDEN, OUTPUT };

	Node(float x, float y, int type, int ID, string name = "", string activationFuncStr ="tanh");
	~Node();

	void Print();

	void Calculate();
	void AddConnection(shared_ptr<Connection>connection);
	void RemoveConnection(shared_ptr<Connection> connection);
	void AddConnectionToNode(Node* node);
	void ClearConnections();

	Vector2f GetPos();
	vector<shared_ptr<Connection>> GetFromConnections();
	vector<shared_ptr<Connection>> GetToConnections();
	vector<shared_ptr<Connection>> GetAllConnections();

	void SetOutput(double val) { output = val; }
	void SetType(int val) { type = val; }

	int GetID();
	int GetType() const { return type; }
	double GetOutput() const { return output; }
	string GetName() { return this->name; }
	string GetActivationFunctionStr() { return activationFuncStr; }
};

