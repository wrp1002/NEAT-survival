#include "Toolbar.h"

#include <allegro5/allegro_native_dialog.h>
#include <iostream>

#include "Camera.h"
#include "InfoDisplay.h"
#include "GameManager.h"
#include "Agent.h"

using namespace std;

ALLEGRO_MENU *Toolbar::menu;

void Toolbar::Init(ALLEGRO_DISPLAY *display) {
    ALLEGRO_MENU_INFO menu_info[] = {
		ALLEGRO_START_OF_MENU("&File", 100),
			{ "&Open", 101, 0, NULL },
			ALLEGRO_START_OF_MENU("Open &Recent...", 110),
				{ "Recent 1", 112, 0, NULL },
				{ "Recent 2", 113, 0, NULL },
				ALLEGRO_END_OF_MENU,

			{ "Show Info Display", BUTTON_IDS::TOGGLE_INFO_DISPLAY, 0, NULL },
			{ "E&xit", BUTTON_IDS::EXIT, 0, NULL },
		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Search", 200),
			{"Random", BUTTON_IDS::SEARCH_RANDOM, 0, NULL },
			ALLEGRO_START_OF_MENU("Highest", 210),
				{ "Age", BUTTON_IDS::SEARCH_HIGHEST_AGE, 0, NULL },
				{ "Damaged Inputs", BUTTON_IDS::SEARCH_HIGHEST_DAMAGED_INPUTS, 0, NULL },
				{ "Energy", BUTTON_IDS::SEARCH_HIGHEST_ENERGY, 0, NULL },
				{ "Health", BUTTON_IDS::SEARCH_HIGHEST_HEALTH, 0, NULL },
				{ "Kills", BUTTON_IDS::SEARCH_HIGHEST_KILLS, 0, NULL },
			ALLEGRO_END_OF_MENU,
            ALLEGRO_START_OF_MENU("Lowest", 220),
				{ "Age", BUTTON_IDS::SEARCH_LOWEST_AGE, 0, NULL },
				{ "Damaged Inputs", BUTTON_IDS::SEARCH_LOWEST_DAMAGED_INPUTS, 0, NULL },
				{ "Energy", BUTTON_IDS::SEARCH_LOWEST_ENERGY, 0, NULL },
				{ "Health", BUTTON_IDS::SEARCH_LOWEST_HEALTH, 0, NULL },
				{ "Kills", BUTTON_IDS::SEARCH_LOWEST_KILLS, 0, NULL },
			ALLEGRO_END_OF_MENU,
		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Rules", 300),
			{"Follow Random Agent", 301, 0, NULL },
		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("Sim Speed", 400),
			{"Increase", BUTTON_IDS::SPEED_INCREASE, 0, NULL },
			{"Current: 1x", 302, 0, NULL },
			{"Decrease", BUTTON_IDS::SPEED_DECREASE, 0, NULL },
			{"Reset", BUTTON_IDS::SPEED_RESET, 0, NULL },
			{"Enable Auto Increase", 305, 0, NULL },
		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("Play", 500),
			{"Spawn Player", BUTTON_IDS::PLAY_START, 0, NULL },
			{"Current: 1x", 502, 0, NULL },
			{"Decrease", 503, 0, NULL },
			{"Reset", 504, 0, NULL },
		ALLEGRO_END_OF_MENU,

		ALLEGRO_END_OF_MENU
	};

    menu = al_build_menu(menu_info);
    al_set_display_menu(display, menu);
}


void Toolbar::HandleEvent(ALLEGRO_EVENT ev) {
    switch (ev.user.data1) {
        case BUTTON_IDS::TOGGLE_INFO_DISPLAY: {
            InfoDisplay::Toggle();
            if (InfoDisplay::IsVisible())
                al_set_menu_item_caption(menu, ev.user.data1, "Hide Info Display");
            else
                al_set_menu_item_caption(menu, ev.user.data1, "Show Info Display");

            break;
        }
        case BUTTON_IDS::EXIT: {
            //done = true;
            break;
        }
        case BUTTON_IDS::SEARCH_RANDOM: {
            shared_ptr<Agent> selectedAgent = GameManager::GetRandomAgent();
            InfoDisplay::SelectObject(selectedAgent);
            Camera::FollowObject(selectedAgent);
            break;
        }
        case BUTTON_IDS::SEARCH_HIGHEST_KILLS: {
            AgentSearch<int>(true, &Agent::GetKills);
            break;
        }
        case BUTTON_IDS::SEARCH_HIGHEST_DAMAGED_INPUTS: {
            AgentSearch<int>(true, &Agent::GetDamagedInputsCount);
            break;
        }
        case BUTTON_IDS::SEARCH_HIGHEST_AGE: {
            AgentSearch<float>(true, &Agent::GetAge);
            break;
        }
        case BUTTON_IDS::SEARCH_HIGHEST_ENERGY: {
            AgentSearch<double>(true, &Agent::GetEnergy);
            break;
        }
        case BUTTON_IDS::SEARCH_HIGHEST_HEALTH: {
            AgentSearch<double>(true, &Agent::GetHealth);
            break;
        }
        case BUTTON_IDS::SEARCH_LOWEST_KILLS: {
            AgentSearch<int>(false, &Agent::GetKills);
            break;
        }
        case BUTTON_IDS::SEARCH_LOWEST_DAMAGED_INPUTS: {
            AgentSearch<int>(false, &Agent::GetDamagedInputsCount);
            break;
        }
        case BUTTON_IDS::SEARCH_LOWEST_AGE: {
            AgentSearch<float>(false, &Agent::GetAge);
            break;
        }
        case BUTTON_IDS::SEARCH_LOWEST_ENERGY: {
            AgentSearch<double>(false, &Agent::GetEnergy);
            break;
        }
        case BUTTON_IDS::SEARCH_LOWEST_HEALTH: {
            AgentSearch<double>(false, &Agent::GetHealth);
            break;
        }
    }
}

void Toolbar::SetMenuCaption(int id, string text) {
    cout << "Setting" << id << " to " << text << endl;
    al_set_menu_item_caption(menu, id, text.c_str());
}

template <class searchType>
void Toolbar::AgentSearch(bool highest, searchType(Agent::*funcPtr)(void)) {
    shared_ptr<Agent> selectedAgent = nullptr;
    int record = highest ? 0 : 1000000000;

    for (auto agent : GameManager::agents) {
        int count = ((Agent *)(agent.get())->*funcPtr)();
        if (highest && count > record || !highest && count < record) {
            selectedAgent = agent;
            record = count;
        }
    }

    InfoDisplay::SelectObject(selectedAgent);
    Camera::FollowObject(selectedAgent);
}
