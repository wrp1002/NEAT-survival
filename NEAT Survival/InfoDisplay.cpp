#include "InfoDisplay.h"

#include <allegro5/allegro_primitives.h>
#include <fmt/core.h>

#include "Object.h"
#include "Font.h"
#include "GameManager.h"
#include "Vector2f.h"
#include "Agent.h"
#include "Egg.h"

#include "NEAT.h"
#include "Node.h"
#include "Connection.h"

ALLEGRO_DISPLAY* InfoDisplay::display = nullptr;
ALLEGRO_EVENT_QUEUE* InfoDisplay::event_queue = nullptr;
weak_ptr<Object> InfoDisplay::selectedObject;
Vector2f InfoDisplay::screenSize(400, 600);
Vector2f InfoDisplay::mousePos(0, 0);

void InfoDisplay::Show() {
	if (display != nullptr)
		return;

	display = al_create_display(screenSize.x, screenSize.y);
	al_set_window_title(display, "Info");
	al_register_event_source(event_queue, al_get_display_event_source(display));
}

void InfoDisplay::Hide() {
	if (display == nullptr)
		return;

	al_unregister_event_source(event_queue, al_get_display_event_source(display));
	al_destroy_display(display);
	display = nullptr;
}

void InfoDisplay::Draw() {
	if (display == nullptr)
		return;

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);
	al_use_transform(&t);

	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0, 0, 0));

	ALLEGRO_FONT* font = Font::GetFont("Minecraft.ttf", 14);
	vector<string> infoText;


	infoText.insert(infoText.end(), {
		fmt::format("Food: {}", GameManager::allFood.size()),
		fmt::format("Objects: {}", GameManager::allObjects.size()),
		fmt::format("Agents: {}", GameManager::agents.size()),

		fmt::format("Sim Time: {}", GameManager::GetSimTicksStr()),
		fmt::format("Speed: {}x", GameManager::GetSpeed()),
		fmt::format("R Time: {}", GameManager::GetSimTimeStr()),

		fmt::format("Total En: {:.2f}", GameManager::GetTotalEnergy()),
	});


	if (!selectedObject.expired()) {
		shared_ptr<Object> object = selectedObject.lock();

		infoText.insert(infoText.end(), {
			fmt::format("Energy: {:.2f}", object->GetEnergy()),
			fmt::format("X: {}  Y: {}", int(object->GetPos().x), int(object->GetPos().y)),
		});

		if (shared_ptr<Agent> selectedAgent = dynamic_pointer_cast<Agent>(object)) {
			infoText.insert(infoText.end(), {
				fmt::format("Generation: {}", selectedAgent->GetGeneration()),
				fmt::format("Health: {:.2f}/{}", selectedAgent->GetHealth(), int(selectedAgent->GetMaxHealth())),
				fmt::format("Age: {}/{}", int(selectedAgent->GetAge()), int(selectedAgent->GetMaxAge())),
				fmt::format("Kills: {}", selectedAgent->GetKills()),
				fmt::format("Energy Usage: {:.2f}", selectedAgent->GetEnergyUsage()),
			});

			infoText.insert(infoText.end(), {
				"",
				DrawNN(selectedAgent->GetNN()),
			});
		}

		if (shared_ptr<Egg> selectedEgg = dynamic_pointer_cast<Egg>(object)) {
			infoText.insert(infoText.end(), {
				fmt::format("Generation: {}", selectedEgg->GetGeneration()),
				fmt::format("Health: {:.2f}", selectedEgg->GetHealth()),
				fmt::format("Hatch Timer: {}", selectedEgg->GetHatchTimer()),
			});
		}
	}

	for (unsigned i = 0; i < infoText.size(); i++) {
		int flags = 0;
		int x = 10;
		int y = (i) / 3 * 17 + 10;

		int index = (i + 1) % 3;

		if (index == 0) {
			flags = ALLEGRO_ALIGN_RIGHT;
			x = screenSize.x - 10;
		}
		if (index == 1) {

		}
		if (index == 2) {
			flags = ALLEGRO_ALIGN_CENTRE;
			x = screenSize.x / 2;
		}

		al_draw_text(font, al_map_rgb(255, 255, 255), x, y, flags, infoText[i].c_str());
	}

	al_flip_display();
}

vector<shared_ptr<Node>> GetConnectedNodes(shared_ptr<Node> node) {
	vector<shared_ptr<Node>> nodes = { node };

	for (auto connection : node->GetToConnections()) {
		shared_ptr<Node> connectedNode = connection->GetTo();
		vector<shared_ptr<Node>> connectedNodes = GetConnectedNodes(connectedNode);
		nodes.insert(nodes.end(), connectedNodes.begin(), connectedNodes.end());
	}

	return nodes;
}

void InfoDisplay::DrawNodes(vector<shared_ptr<Node>> nodes, shared_ptr<Node> hoveredNode, float alphaCoef, bool drawWeights) {
	for (auto node : nodes) {
		Vector2f realPos = CalculateNodePos(node);

		float distance = realPos.GetDistance(mousePos);
		int nodeSize = 10;
		bool hovering = node == hoveredNode;

		//cout << "X:" << node->GetX() * screenSize.x << " Y:" << node->GetY() * screenSize.y << endl;

		// Draw connections coming out of node
		for (auto connection : node->GetToConnections()) {
			Vector2f fromPos = CalculateNodePos(connection->GetFrom());
			Vector2f toPos = CalculateNodePos(connection->GetTo());
			float width = abs(connection->GetWeight()) * 2 + 1;
			ALLEGRO_COLOR color = (
				connection->GetEnabled() ? (
					connection->GetWeight() > 0 ?
					al_map_rgba(50 * alphaCoef, 100 * alphaCoef, 255 * alphaCoef, 50 * alphaCoef)
					: al_map_rgba(255 * alphaCoef, 50 * alphaCoef, 50 * alphaCoef, 50 * alphaCoef)
				)
				: al_map_rgba(50 * alphaCoef, 50 * alphaCoef, 50 * alphaCoef, 50 * alphaCoef)
			);

			al_draw_line(fromPos.x, fromPos.y, toPos.x, toPos.y, color, width);

			if (drawWeights)
				al_draw_text(Font::GetFont("Minecraft.ttf", 10), al_map_rgb(255, 255, 255), (fromPos.x + toPos.x) / 2, (fromPos.y + toPos.y) / 2, ALLEGRO_ALIGN_CENTER, fmt::format("{:.2f}", connection->GetWeight()).c_str());
		}

		if (hovering)
			al_draw_filled_circle(realPos.x, realPos.y, nodeSize + 1, al_map_rgba(255, 255, 255, 255));

		al_draw_filled_circle(
			realPos.x,
			realPos.y,
			nodeSize,
			al_map_rgba(
				(100 - (node->GetOutput() * 100)) * alphaCoef,
				(100 + (node->GetOutput() * 100)) * alphaCoef,
				0,
				255 * alphaCoef
			)
		);

		if (hovering) {
			al_draw_text(
				Font::GetFont("Minecraft.ttf", 10),
				al_map_rgb(255, 255, 255), realPos.x, realPos.y - 4,
				ALLEGRO_ALIGN_CENTER,
				fmt::format("{:.2f}",
				node->GetOutput()).c_str()
			);
		}

	}
}

string InfoDisplay::DrawNN(shared_ptr<NEAT> nn) {
	string selectedNodeName = "";
	int nodeSize = 10;
	vector<shared_ptr<Node>> nodes;
	shared_ptr<Node> hoveredNode = nullptr;

	for (auto node : nn->GetNodes()) {
		Vector2f realPos = CalculateNodePos(node);

		float distance = realPos.GetDistance(mousePos);

		if (distance < nodeSize) {
			hoveredNode = node;
			nodes = GetConnectedNodes(node);
			selectedNodeName = fmt::format("{} ({})", node->GetName(), node->GetActivationFunctionStr());
			break;
		}
	}

	float firstDrawAlpha = nodes.size() ? 0.1 : 1.0;

	DrawNodes(nn->GetNodes(), hoveredNode, firstDrawAlpha, false);
	DrawNodes(nodes, hoveredNode, 1, true);

	shared_ptr<Agent> selectedAgent = dynamic_pointer_cast<Agent>(selectedObject.lock());

	for (unsigned i = 0; i < nn->GetInputNodes().size(); i++) {
		if (selectedAgent->InputNodeDisabled(i)) {
			shared_ptr<Node> node = nn->GetInputNodes()[i];
			Vector2f pos = CalculateNodePos(node);
			al_draw_filled_circle(pos.x, pos.y, 10, al_map_rgb(255, 0, 255));
		}
	}

	return selectedNodeName;
}

void InfoDisplay::SelectObject(weak_ptr<Object> obj) {
	selectedObject = obj;
}

void InfoDisplay::Toggle() {
	if (display == nullptr)
		Show();
	else
		Hide();
}

Vector2f InfoDisplay::CalculateNodePos(shared_ptr<Node> node) {
	unsigned topPadding = 120;
	unsigned sidePadding = 20;
	unsigned bottomPadding = 20;

	Vector2f nodePos = node->GetPos();

	Vector2f realPos(
		nodePos.x * (double(screenSize.x) - sidePadding * 2.0) + sidePadding,
		nodePos.y * (double(screenSize.y) - topPadding - bottomPadding) + topPadding
	);

	return realPos;
}

bool InfoDisplay::IsVisible() {
	return display != nullptr;
}

