#include "NEAT.h"

void NEAT::_RemoveNode(vector<shared_ptr<Node>>* nodes, shared_ptr<Node> nodeToRemove) {
	for (unsigned int i = 0; i < nodes->size(); i++) {
		if (nodes->at(i) == nodeToRemove) {
			nodes->erase(nodes->begin() + i);
			return;
		}
	}
}

NEAT::NEAT(vector<string> inputLabels, vector<string> outputLabels) {
	for (string label : inputLabels) {
		shared_ptr<Node> inputNode = make_shared<Node>(Node(0, float(inputNodes.size()) / inputLabels.size(), Node::INPUT, allNodes.size(), label));
		AddNode(inputNode);
	}

	for (string label : outputLabels) {
		shared_ptr<Node> outputNode = make_shared<Node>(Node(1, float(outputNodes.size()) / outputLabels.size(), Node::OUTPUT, allNodes.size(), label));
		AddNode(outputNode);
	}
}

NEAT::NEAT() {
}


NEAT::~NEAT() {
}

shared_ptr<NEAT> NEAT::Copy() {
	shared_ptr<NEAT> nn = make_unique<NEAT>(NEAT());
	vector<shared_ptr<Node>> newNodes;

	for (unsigned int i = 0; i < allNodes.size(); i++) {
		shared_ptr<Node>node = allNodes[i];
		shared_ptr<Node>newNode = make_shared<Node>(Node(*node));
		newNode->ClearConnections();

		nn->AddNode(newNode);
		newNodes.push_back(newNode);
	}

	for (unsigned int i = 0; i < allNodes.size(); i++) {
		shared_ptr<Node> node = allNodes[i];
		shared_ptr<Node> newNode = newNodes[i];

		for (shared_ptr<Connection> connection : node->GetToConnections()) {
			int fromID = connection->GetFrom()->GetID();
			int toID = connection->GetTo()->GetID();
			shared_ptr<Node> fromNode;
			shared_ptr<Node> toNode;
			
			for (shared_ptr<Node> testNode : newNodes) {
				int ID = testNode->GetID();
				if (ID == fromID)
					fromNode = testNode;
				if (ID == toID)
					toNode = testNode;
				if (fromNode && toNode)
					break;
			}

			if (fromNode && toNode) {
				shared_ptr<Connection> newConnection = make_shared<Connection>(Connection(fromNode, toNode));
				nn->AddConnection(newConnection);
				newConnection->SetWeight(connection->GetWeight());
			}
			else
				cout << "Couldnt find node!" << endl;
		}

	}
	

	return nn;
}

vector<shared_ptr<Node>> NEAT::GetNodes() {
	return allNodes;
}

vector<shared_ptr<Connection>> NEAT::GetConnections() {
	return connections;
}

void NEAT::PrintNN() {
	cout << "nodes:" << allNodes.size() << " input:" << inputNodes.size() << " output:" << outputNodes.size() << endl;
	/*
	for (auto node : allNodes) {
		cout << "node " << node->GetID() << " type:" << node->GetType() << endl;
		cout << "connections:" << node->allConnections.size() << " toConnections:" << node->toConnections.size() << " fromConnections:" << node->fromConnections.size() << endl;
		for (auto connection : node->allConnections) {
			cout << connection->GetFrom()->GetID() << " --> " << connection->GetTo()->GetID() << endl;
		}
		cout << endl;
	}
	*/
	cout << endl;
	cout << "connections:" << connections.size() << endl;
	for (auto connection : connections) {
		cout << connection->GetFrom()->GetID() << " --> " << connection->GetTo()->GetID() << endl;
	}
}

vector<double> NEAT::Calculate(vector<double> inputs) {
	vector<double> outputs;

	if (inputs.size() != inputNodes.size())
		throw exception("Input sizes don't match!");

	for (unsigned i = 0; i < inputs.size(); i++)
		inputNodes[i]->SetOutput(inputs[i]);

	for (shared_ptr<Node> node : allNodes) {
		if (node->GetType() == Node::INPUT)
			continue;

		node->Calculate();
	}

	for (shared_ptr<Node> outputNode : outputNodes) {
		outputs.push_back(outputNode->GetOutput());
	}

	return outputs;
}

vector<double> NEAT::GetOutputs() {
	vector<double> outputs;
	for (shared_ptr<Node> outputNode : outputNodes)
		outputs.push_back(outputNode->GetOutput());
	return outputs;
}

void NEAT::AddNode(shared_ptr<Node> node) {
	if (node->GetType() == Node::INPUT)
		inputNodes.push_back(node);
	else if (node->GetType() == Node::OUTPUT)
		outputNodes.push_back(node);
	else if (node->GetType() == Node::HIDDEN)
		hiddenNodes.push_back(node);

	if (allNodes.size() == 0) {
		allNodes.push_back(node);
		return;
	}
	for (unsigned i = 0; i < allNodes.size(); i++) {
		if (node->GetPos().x < allNodes[i]->GetPos().x) {
			allNodes.insert(allNodes.begin() + i, node);
			return;
		}
	}
	allNodes.push_back(node);
}

void NEAT::RemoveNode(shared_ptr<Node> node) {
	vector<shared_ptr<Connection>> nodeConnections = node->GetAllConnections();
	for (unsigned i = 0; i < nodeConnections.size(); i++) {
		shared_ptr<Connection> connection = nodeConnections[i];
		shared_ptr<Node> otherNode = nullptr;
		if (connection->GetFrom() == node)
			otherNode = connection->GetTo();
		else if (connection->GetTo() == node)
			otherNode = connection->GetFrom();

		RemoveConnection(connection);

		if (otherNode && otherNode->GetType() == Node::HIDDEN && otherNode != node && (otherNode->GetFromConnections().size() == 0 || otherNode->GetToConnections().size() == 0))
			RemoveNode(otherNode);
	}

	_RemoveNode(&allNodes, node);
	_RemoveNode(&inputNodes, node);
	_RemoveNode(&hiddenNodes, node);
	_RemoveNode(&outputNodes, node);
}

shared_ptr<Connection> NEAT::AddConnection(shared_ptr<Connection> connection) {
	connections.push_back(connection);
	connection->GetTo()->AddConnection(connection);
	connection->GetFrom()->AddConnection(connection);
	return connection;
}

shared_ptr<Connection> NEAT::AddConnection(shared_ptr<Node> from, shared_ptr<Node> to) {
	shared_ptr<Connection> connection = make_shared<Connection>(Connection(from, to));
	connections.push_back(connection);
	from->AddConnection(connection);
	to->AddConnection(connection);
	return connection;
}

void NEAT::RemoveConnection(shared_ptr<Connection> connection) {
	shared_ptr<Node> fromNode = connection->GetFrom();
	shared_ptr<Node> toNode = connection->GetTo();

	if (fromNode)
		connection->GetFrom()->RemoveConnection(connection);
	if (toNode)
		connection->GetTo()->RemoveConnection(connection);
	
	for (unsigned i = 0; i < connections.size(); i++) {
		if (connections[i] == connection) {
			connections.erase(connections.begin() + i);
			return;
		}
	}
}

bool NEAT::ConnectionExists(shared_ptr<Connection> connection) {
	for (auto compare : connections) {
		if (compare->GetFrom() == connection->GetFrom() && compare->GetTo() == connection->GetTo())
			return true;
	}
	return false;
}

bool NEAT::ConnectionExists(shared_ptr<Node> from, shared_ptr<Node> to) {
	for (auto compare : connections) {
		if (compare->GetFrom() == from && compare->GetTo() == to)
			return true;
		if (compare->GetFrom() == to && compare->GetTo() == from)
			return true;
	}
	return false;
}

void NEAT::Mutate() {
	for (auto connection : connections) {
		if (Globals::Random() < PROBABILITY_MUTATE_TOGGLE * MUTATE_COEF) {
			connection->ToggleEnabled();
		}
		if (Globals::Random() < PROBABILITY_MUTATE_WEIGHT_SHIFT * MUTATE_COEF) {
			double weight = connection->GetWeight();
			weight *= Globals::Random() + 0.5;
			connection->SetWeight(weight);
		}
		if (Globals::Random() < PROBABILITY_MUTATE_WEIGHT_RANDOM * MUTATE_COEF) {
			connection->RandomWeight();
		}
	}

	
	if (Globals::Random() < PROBABILITY_MUTATE_ADD_CONNECTION * MUTATE_COEF) {
		MutateAddConnection();
	}
	
	if (Globals::Random() < PROBABILITY_MUTATE_ADD_NODE * MUTATE_COEF) {
		MutateAddNode();
	}
	
	
	if (Globals::Random() < PROBABILITY_MUTATE_REMOVE_NODE * MUTATE_COEF) {
		MutateRemoveNode();
	}
}


void NEAT::MutateAddConnection() {
	if (allNodes.size() == 0) {
		cout << "NN has no nodes?" << endl;
		return;
	}
	for (unsigned i = 0; i < MUTATE_ADD_CONNECTION_TRIES; i++) {
		shared_ptr<Node> node1 = allNodes[rand() % allNodes.size()];
		shared_ptr<Node> node2 = allNodes[rand() % allNodes.size()];
		shared_ptr<Connection> connection = nullptr;

		if (node1 == node2)
			continue;

		if (ConnectionExists(node1, node2))
			continue;

		if (node1->GetPos().x == node2->GetPos().x)
			continue;

		if (node1->GetPos().x < node2->GetPos().x)
			connection = make_shared<Connection>(Connection(node1, node2));
		else
			connection = make_shared<Connection>(Connection(node2, node1));

		AddConnection(connection);
		return;
	}
}

void NEAT::MutateAddNode() {
	if (connections.size() == 0)
		return;

	// pick a random connection
	shared_ptr<Connection> connection = connections[rand() % connections.size()];
	double weight = connection->GetWeight();
	shared_ptr<Node> fromNode = connection->GetFrom();
	shared_ptr<Node> toNode = connection->GetTo();
	Vector2f fromPos = fromNode->GetPos();
	Vector2f toPos = toNode->GetPos();
	
	// create new node at center of connection
	shared_ptr<Node> newNode = make_shared<Node>(Node((fromPos.x + toPos.x) / 2, (fromPos.y + toPos.y) / 2 + (Globals::Random() * 0.2) - 0.1, Node::HIDDEN, allNodes.size(), "Hidden Node"));
	AddNode(newNode);

	RemoveConnection(connection);

	shared_ptr<Connection>connection1 = AddConnection(fromNode, newNode);
	connection1->SetWeight(1.0);

	shared_ptr<Connection> connection2 = AddConnection(newNode, toNode);
	connection2->SetWeight(weight);
}

void NEAT::MutateRemoveNode() {
	if (hiddenNodes.size() == 0)
		return;

	shared_ptr<Node> node = hiddenNodes[rand() % hiddenNodes.size()];
	RemoveNode(node);
}

