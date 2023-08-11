#include "Font.h"

#include "Globals.h"

unordered_map<string, ALLEGRO_FONT*> Font::fonts;

ALLEGRO_FONT* Font::GetFont(string fontName, int fontSize) {
	string key = fontName + to_string(fontSize);
	if (fonts[key] == nullptr) {
		string fontPath = Globals::resourcesDir + fontName;
		fonts[key] = al_load_font(fontPath.c_str(), fontSize, 0);
	}
	return fonts[key];
}
