#include "InfoDisplay.h"

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
		format("Food: {}", GameManager::allFood.size()),
		format("Objects: {}", GameManager::allObjects.size()),
		format("Agents: {}", GameManager::agents.size()),

		format("Sim Time: {}", GameManager::GetSimTicksStr()),
		format("Speed: {}x", GameManager::GetSpeed()),
		format("R Time: {}", GameManager::GetSimTimeStr()),
		
		format("Total En: {:.2f}", GameManager::GetTotalEnergy()),
	});


	if (!selectedObject.expired()) {
		shared_ptr<Object> object = selectedObject.lock();

		infoText.insert(infoText.end(), {
			format("Energy: {:.2f}", object->GetEnergy()),
			format("X: {}  Y: {}", int(object->GetPos().x), int(object->GetPos().y)),
		});

		if (shared_ptr<Agent> selectedAgent = dynamic_pointer_cast<Agent>(object)) {
			infoText.insert(infoText.end(), {
				format("Generation: {}", selectedAgent->GetGeneration()),
				format("Health: {:.2f}%", selectedAgent->GetHealthPercent() * 100),
				format("Age: {:.2f}", selectedAgent->GetAge())
			});

			infoText.insert(infoText.end(), {
				"",
				DrawNN(selectedAgent->GetNN()),
			});
		}
	}

	for (unsigned i = 0; i < infoText.size(); i++) {
		int flags = NULL;
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

string InfoDisplay::DrawNN(shared_ptr<NEAT> nn) {
	string selectedNodeName = "";

	for (auto node : nn->GetNodes()) {
		Vector2f realPos = CalculateNodePos(node);

		float distance = realPos.GetDistance(mousePos);
		int nodeSize = 10;
		bool hovering = (distance < nodeSize);
		
		//cout << "X:" << node->GetX() * screenSize.x << " Y:" << node->GetY() * screenSize.y << endl;

		// Draw connections coming out of node
		for (auto connection : node->GetToConnections()) {
			Vector2f fromPos = CalculateNodePos(connection->GetFrom());
			Vector2f toPos = CalculateNodePos(connection->GetTo());
			ALLEGRO_COLOR color = (connection->GetEnabled() ? ( connection->GetWeight() > 0 ? al_map_rgba(50, 100, 255, 50) : al_map_rgba(255, 50, 50, 50) ) : al_map_rgba(50, 50, 50, 50) );
			float width = abs(connection->GetWeight()) * 2 + 1;
			al_draw_line(fromPos.x, fromPos.y, toPos.x, toPos.y, color, width);
			if (hovering)
				al_draw_text(Font::GetFont("Minecraft.ttf", 10), al_map_rgb(255, 255, 255), (fromPos.x + toPos.x) / 2, (fromPos.y + toPos.y) / 2, ALLEGRO_ALIGN_CENTER, format("{:.2f}", connection->GetWeight()).c_str());
		}

		if (hovering) {
			al_draw_filled_circle(realPos.x, realPos.y, nodeSize + 1, al_map_rgb(255, 255, 255));
			selectedNodeName = format("{} ({})", node->GetName(), node->GetActivationFunctionStr());
		}

		al_draw_filled_circle(realPos.x, realPos.y, nodeSize, al_map_rgb(100 - (node->GetOutput() * 100), 100 + (node->GetOutput() * 100), 0));

		if (hovering) {
			al_draw_text(Font::GetFont("Minecraft.ttf", 10), al_map_rgb(255, 255, 255), realPos.x, realPos.y - 4, ALLEGRO_ALIGN_CENTER, format("{:.2f}", node->GetOutput()).c_str());
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