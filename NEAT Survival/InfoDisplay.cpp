#include "InfoDisplay.h"

ALLEGRO_DISPLAY* InfoDisplay::display = nullptr;
ALLEGRO_EVENT_QUEUE* InfoDisplay::event_queue = nullptr;
weak_ptr<Object> InfoDisplay::selectedObject;
Vector2f InfoDisplay::screenSize(400, 400);
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

	al_draw_text(font, al_map_rgb(255, 255, 255), 10,					10, NULL,					format("Food: {}", GameManager::allFood.size()).c_str());
	al_draw_text(font, al_map_rgb(255, 255, 255), screenSize.x / 2,		10, ALLEGRO_ALIGN_CENTER,	format("Objects: {}", GameManager::allObjects.size()).c_str());
	al_draw_text(font, al_map_rgb(255, 255, 255), screenSize.x - 10,	10, ALLEGRO_ALIGN_RIGHT,	format("Agents: {}", GameManager::agents.size()).c_str());


	if (!selectedObject.expired()) {
		al_draw_text(font, al_map_rgb(255, 255, 255), 10, 25, NULL, format("X: {}  Y: {}", int(selectedObject.lock()->GetPos().x), int(selectedObject.lock()->GetPos().y)).c_str());
		al_draw_text(font, al_map_rgb(255, 255, 255), screenSize.x / 2, 25, ALLEGRO_ALIGN_CENTER, format("Energy: {:.2f}", selectedObject.lock()->GetEnergy()).c_str());

		if (selectedObject.lock()->GetType() == "agent") {
			Agent* selectedAgent = dynamic_cast<Agent*>(selectedObject.lock().get());
			al_draw_text(font, al_map_rgb(255, 255, 255), screenSize.x - 10, 25, ALLEGRO_ALIGN_RIGHT, format("Generation: {}", selectedAgent->GetGeneration()).c_str());


			al_draw_text(font, al_map_rgb(255, 255, 255), 10, 40, NULL, format("Energy: {}%", int(selectedAgent->GetEnergyPercent() * 100)).c_str());
			al_draw_text(font, al_map_rgb(255, 255, 255), screenSize.x - 10, 40, ALLEGRO_ALIGN_RIGHT, format("Health: {}%", int(selectedAgent->GetHealthPercent() * 100)).c_str());

			al_draw_text(font, al_map_rgb(255, 255, 255), 10, 55, NULL, format("Age: {:.2f}", selectedAgent->GetAge()).c_str());

			DrawNN(selectedAgent->GetNN());
		}
	}

	al_flip_display();
}

void InfoDisplay::DrawNN(shared_ptr<NEAT> nn) {
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
			al_draw_text(Font::GetFont("Minecraft.ttf", 14), al_map_rgb(255, 255, 255), screenSize.x / 2, 25, ALLEGRO_ALIGN_CENTER, node->GetName().c_str());
		}

		al_draw_filled_circle(realPos.x, realPos.y, nodeSize, al_map_rgb(100 - (node->GetOutput() * 100), 100 + (node->GetOutput() * 100), 0));

		if (hovering) {
			al_draw_text(Font::GetFont("Minecraft.ttf", 10), al_map_rgb(255, 255, 255), realPos.x, realPos.y - 4, ALLEGRO_ALIGN_CENTER, format("{:.2f}", node->GetOutput()).c_str());
		}
	}
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