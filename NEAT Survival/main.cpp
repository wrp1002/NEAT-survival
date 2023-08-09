#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <iostream>

#include "Globals.h"
#include "UserInput.h"
#include "Agent.h"
#include "GameManager.h"
#include "InfoDisplay.h"
#include "Camera.h"
#include "Vector2f.h"
#include "Font.h"
#include "GameRules.h"
#include "Object.h"

using namespace std;


int main() {
	srand(time(0));
	bool done = false;
	bool redraw = true;
	bool forceRedraw = false;
	double lastRedrawTime = 0;
	float maxRedrawTime = 2.0;

	al_init();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_mouse();
	al_install_keyboard();


	ALLEGRO_DISPLAY* display = al_create_display(Globals::screenWidth, Globals::screenHeight);

	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
	InfoDisplay::event_queue = event_queue;

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_keyboard_event_source());


	al_start_timer(timer);

	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;

				case ALLEGRO_KEY_P:
					GameManager::TogglePlayer();
					if (!GameManager::player.expired()) {
						Camera::FollowObject(GameManager::player.lock());
						InfoDisplay::SelectObject(GameManager::player);
					}
					break;

				case ALLEGRO_KEY_I:
					InfoDisplay::Toggle();
					break;

				case ALLEGRO_KEY_SPACE:
					GameManager::TogglePaused();
					break;

				case ALLEGRO_KEY_R:
					//if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(InfoDisplay::selectedObject.lock()))
						//agent->Reproduce();
					//GameManager::Reset();
					break;



				case ALLEGRO_KEY_N:
					if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(InfoDisplay::selectedObject.lock()))
						agent->MutateAddNode();
					break;

				case ALLEGRO_KEY_M:
					if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(InfoDisplay::selectedObject.lock()))
						agent->MutateRemoveNode();
					break;

				case ALLEGRO_KEY_C:
					if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(InfoDisplay::selectedObject.lock()))
						agent->MutateAddConnection();
					break;


				case ALLEGRO_KEY_S:
					for (int i = 0; i < 100; i++)
						GameManager::SpawnRandomAgent();
					break;

				case ALLEGRO_KEY_PGDN:
					GameManager::DecreaseSpeed();
					break;

				case ALLEGRO_KEY_PGUP:
					GameManager::IncreaseSpeed();
					break;

				case ALLEGRO_KEY_HOME:
					GameManager::ResetSpeed();
					break;

				case ALLEGRO_KEY_ENTER:
					GameRules::GameRulePrompt();
					al_flush_event_queue(event_queue);
					lastRedrawTime = al_get_time();
					break;
			}

			UserInput::SetPressed(ev.keyboard.keycode, true);

		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			UserInput::SetPressed(ev.keyboard.keycode, false);
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
			Vector2f mousePos(ev.mouse.x, ev.mouse.y);
			if (ev.mouse.display == display) {
				UserInput::SetMousePos(mousePos);
				int wheelDiff = ev.mouse.z - UserInput::mouseWheel;
				Camera::UpdateZoom(wheelDiff);
				UserInput::mouseWheel = ev.mouse.z;
			}
			else if (ev.mouse.display == InfoDisplay::display) {
				InfoDisplay::mousePos = mousePos;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.display == display) {
				if (ev.mouse.button == 1) {
					Vector2f worldPos = Camera::ScreenPos2WorldPos(Vector2f(ev.mouse.x, ev.mouse.y));
					cout << worldPos.x << " " << worldPos.y << endl;
					shared_ptr<Object> clickedObject = GameManager::collisionGrid.GetCollidingObject(worldPos);
					Camera::FollowObject(clickedObject);
					InfoDisplay::SelectObject(clickedObject);
					if (clickedObject)
						clickedObject->Print();
				}
				if (ev.mouse.button == 2)
					UserInput::StartDragging(Vector2f(ev.mouse.x, ev.mouse.y));
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (ev.mouse.display == display) {
				if (ev.mouse.button == 2) {
					Camera::pos = Camera::CalculatePos();
					UserInput::StopDragging();
				}
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;

			if (al_get_time() - lastRedrawTime > maxRedrawTime) {
				cout << "Last redraw was more than " << maxRedrawTime << " seconds ago" << endl;
				cout << "Clearing event queue and lowering speed to " << (GameManager::GetSpeed() > 1 ? GameManager::GetSpeed() - 1 : 1) << endl;

				if (GameManager::GetSpeed() == 1) {
					GameManager::TogglePaused();
					cout << "MAJOR LAG DETECTED. paused sim" << endl;
				}

				GameManager::DecreaseSpeed();
				al_flush_event_queue(event_queue);
			}
			else {
				GameManager::Update();

				if (Camera::followObject.expired() && GameRules::IsRuleEnabled("FollowRandomAgent")) {
					shared_ptr<Object> followObj = GameManager::GetRandomAgent();
					Camera::FollowObject(followObj);
					InfoDisplay::SelectObject(followObj);
				}
			}
		}


		if ((redraw && al_is_event_queue_empty(event_queue)) || forceRedraw) {
			redraw = false;
			forceRedraw = false;
			lastRedrawTime = al_get_time();

			InfoDisplay::Draw();

			al_set_target_backbuffer(display);
			Camera::UpdateTransform();
			al_clear_to_color(al_map_rgb(10, 0, 0));

			//al_draw_circle(300, 300, 30, al_map_rgb(255, 0, 255), 2);

			GameManager::Draw();

			//al_draw_text(Font::GetFont("Minecraft.ttf", 10), al_map_rgb(255, 255, 255), 10, 10, NULL, "LOLOLOL");

			al_flip_display();
		}

	}

	GameManager::Shutdown();
	al_destroy_display(display);
	al_destroy_display(InfoDisplay::display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_uninstall_mouse();
	al_uninstall_keyboard();
}

