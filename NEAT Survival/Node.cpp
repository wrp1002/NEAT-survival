#include "Node.h"
#include "Connection.h"

void Node::_RemoveConnection(shared_ptr<Connection> connection, vector<shared_ptr<Connection>>* connections) {
	for (unsigned i = 0; i < connections->size(); i++) {
		if (connections->at(i) == connection) {
			connections->erase(connections->begin() + i);
			break;
		}
	}
}

Node::Node(float x, float y, int type, int ID, string name) : pos(x, y) {
	this->type = type;
	this->name = name;
	this->output = 0;
	this->ID = ID;
}


Node::~Node() {
}

void Node::Print() {
	printf("Node(X:%f, Y:%f, Connections:%i)", this->pos.x, this->pos.y, this->allConnections.size());
}

void Node::Calculate() {
	double sum = 0;
	for (auto connection : fromConnections) {
		if (connection->GetEnabled()) {
			sum += connection->GetFrom()->GetOutput() * connection->GetWeight();
		}
	}
	this->output = ActivationFunction(sum);
}

double Node::ActivationFunction(double input) {
	//return tanh(input);	//tanh
	return 1 / (1 + exp(-input));
}

void Node::AddConnection(shared_ptr<Connection> connection) {
	allConnections.push_back(connection);

	if (connection->GetFrom().get() == this)
		toConnections.push_back(connection);
	if (connection->GetTo().get() == this)
		fromConnections.push_back(connection);
}

void Node::RemoveConnection(shared_ptr<Connection> connection) {
	_RemoveConnection(connection, &allConnections);
	_RemoveConnection(connection, &fromConnections);
	_RemoveConnection(connection, &toConnections);
}

void Node::ClearConnections() {
	allConnections.clear();
	toConnections.clear();
	fromConnections.clear();
}

Vector2f Node::GetPos() {
	return pos;
}

vector<shared_ptr<Connection>> Node::GetFromConnections() {
	return fromConnections;
}

vector<shared_ptr<Connection>> Node::GetToConnections() {
	return toConnections;
}

vector<shared_ptr<Connection>> Node::GetAllConnections() {
	return allConnections;
}

int Node::GetID() {
	return ID;
}
