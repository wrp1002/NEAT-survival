#include "GameRules.h"

unordered_map<string, bool> GameRules::rules(
    {
        { "WasteDamage", false },
        { "EyeMovement", false },
        { "MouthControl", false },
        { "FollowRandomAgent", false },
        { "ForceMinPopulation", false },
        { "EnforceDiet", false },
    }
);


bool GameRules::IsInt(string input) {
    try {
        stoi(input);
    }
    catch (exception e) {
        return false;
    }
    return true;
}

void GameRules::PrintPrompt(vector<string> keys) {
    for (unsigned i = 0; i < keys.size(); i++) {
        string key = keys[i];
        cout << i + 1 << ". " << keys[i] << ": " << (rules[key] ? "Enabled" : "Disabled") << endl;
    }
    cout << keys.size() + 1 << ". return" << endl;
}

void GameRules::GameRulePrompt() {
    //SetFocus(GetConsoleWindow());

    int input = -1;
    string inputStr = "";

    vector<string> keys;

    for (auto val : rules) {
        keys.push_back(val.first);
    }


    while (input != keys.size()) {
        PrintPrompt(keys);

        cin >> inputStr;
        if (!IsInt(inputStr))
            continue;
        input = stoi(inputStr);
        input--;

        if (input < 0 || input >= keys.size())
            continue;

        string key = keys[input];
        rules[key] = !rules[key];
    }
}

bool GameRules::IsRuleEnabled(string key) {
    if (rules.find(key) == rules.end()) {
        cout << "Key not found in GameRules: " << key << endl;
        return false;
    }
    return rules[key];
}
