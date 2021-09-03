#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <unordered_map>
#include <string>

#include "Globals.h"

using namespace std;

namespace Font {
	extern unordered_map<string, ALLEGRO_FONT*> fonts;

	ALLEGRO_FONT* GetFont(string fontName, int fontSize);
};

