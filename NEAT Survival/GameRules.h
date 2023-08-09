#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace GameRules {
	extern unordered_map<string, bool> rules;

	bool IsInt(string input);

	void PrintPrompt(vector<string> keys);
	void GameRulePrompt();

	bool IsRuleEnabled(string key);
};

