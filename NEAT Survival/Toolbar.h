#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <memory>
#include <string>

class Agent;

using namespace std;

namespace Toolbar {
    extern ALLEGRO_MENU *menu;

    namespace BUTTON_IDS {
        enum IDS {
            EXIT,
            TOGGLE_INFO_DISPLAY,
            SEARCH_RANDOM,
            SEARCH_KILLS,
            SEARCH_DAMAGED_INPUTS,
            SEARCH_AGE,
            SEARCH_ENERGY,
            SEARCH_HEALTH,

            SPEED_INCREASE,
            SPEED_DECREASE,
            SPEED_RESET,

            PLAY_START,
        };
    }

    void Init(ALLEGRO_DISPLAY *display);
    void HandleEvent(ALLEGRO_EVENT ev);
    void SetMenuCaption(int id, string text);

    template <class searchType>
    void AgentSearch(bool highest, searchType(Agent::*funcPtr)(void));
}