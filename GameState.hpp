#pragma once

#include "Scene.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

using namespace std;

struct GameState {
    int currentScene = 0;
    bool inMenu = true;
    int gameResult = 0;
    sf::Font font;
    
    vector<Scene> scenes;
    map<string, sf::Texture> textures;
    map<string, unique_ptr<sf::Music>> musicMap;
    sf::Music* currentMusic = nullptr;
    
    void playMusic(const string& music) {
    if (music.empty()) { 
        if (currentMusic) currentMusic->stop();
        currentMusic = nullptr;
        return;
    }

    auto it = musicMap.find(music);
    if (it != musicMap.end()) {
        if (currentMusic != it->second.get()) { 
            if (currentMusic) currentMusic->stop();
            currentMusic = it->second.get();
            currentMusic->setLoop(true);
            currentMusic->play();
        }
    } else {
        cerr << "Музыка не найдена: " << music << endl;
    }
}
};