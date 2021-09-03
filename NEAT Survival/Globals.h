#pragma once
#define _USE_MATH_DEFINES
#include <random>
#include <math.h>
#include <string>

using namespace std;

namespace Globals {
	const int screenWidth = 1000;
	const int screenHeight = 800;
	const int mapSize = 15000;
	const int mapWidth = mapSize;
	const int mapHeight = mapSize;
	const string resourcesDir = "Resources/";
	const bool drawCollisionGrid = false;

	static double Random() {
		return double(rand()) / double(RAND_MAX);
	}

	static float RandomDir() {
		return float((rand() % 360 - 180) * (M_PI / 180));
	}

	static float GetDistance(float x1, float y1, float x2, float y2) {
		return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	}

	static int RandomInt(int min, int max) {
		return rand() % (max - min) + min;
	}

	static float RandomNormal() {
		float amount = 16;
		double sum = 0;
		for (int i = 0; i < amount; i++)
			sum += Random();
		sum /= amount;
		return float(sum);
	}

	static double Round(double num) {
		return double(int(num * 100) / 100);
	}
};
