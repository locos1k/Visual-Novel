#pragma once

#include <string>
#include <vector>

using namespace std;

struct Scene {
    string background;
    vector<string> characters;
    string music;
    wstring dialog;
    vector<wstring> choices;
    vector<int> nextScenes;
};

vector<Scene> loadScenes();