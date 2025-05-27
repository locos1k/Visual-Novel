#include "Scene.hpp"
#include "GameState.hpp"
#include "Entity.hpp"
#include "GameLogic.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 900), "Лунный Сад");
    GameState game;
    setlocale(LC_ALL, "Russian");
    
    if (!game.font.loadFromFile("assets/fonts/main.ttf")) {
        cerr << "Ошибка загрузки шрифта!" << endl;
        return -1;
    }
    
    game.scenes = loadScenes(); 
    
    for (auto& scene : game.scenes) {
        if (!scene.background.empty() && !game.textures.count(scene.background)) {
            sf::Texture tex;
            if (!tex.loadFromFile(scene.background)) {
                cerr << "Ошибка загрузки текстуры: " << scene.background << endl;
            }
            else game.textures[scene.background] = tex;
        }
        for (auto& character : scene.characters) {
            if (!character.empty() && !game.textures.count(character)) {
                sf::Texture tex;
                if (!tex.loadFromFile(character)) {
                    cerr << "Ошибка загрузки персонажа: " << character << endl;
                }
                else game.textures[character] = tex;
            }
        }
    }
    
    vector<string> musicFiles = {"stuk.ogg", "boss_fight.ogg", "end.ogg", "vict.ogg",
         "stuk1.ogg", "nachalo.ogg", "castle_enter.ogg", "merzost.ogg", "boss_fight1.ogg", "mimic_here.ogg", "demon_win.ogg"};
for (auto& file : musicFiles) {
    auto music = make_unique<sf::Music>();
    if (!music->openFromFile("assets/music/" + file)) {
        cerr << "Ошибка загрузки музыки: " << file << endl;
        continue;
    }
    game.musicMap.emplace(file, move(music));
}

for (size_t i = 0; i < game.scenes.size(); ++i) {
    const auto& scene = game.scenes[i];
    if (!scene.music.empty() && !game.musicMap.count(scene.music)) {
        cerr << "Предупреждение: Музыка для сцены " << i 
             << " (" << scene.music << ") не загружена!" << endl;
    }
}
    
    showMenu(window, game);
    game.playMusic(game.scenes[game.currentScene].music);
    while (window.isOpen()) {
    handleGameInput(window, game);
    
    if (game.currentScene == 17) {
        int result = runTugOfWarGame(window, game);
        game.currentScene = (result == 555) ? 19 : 18; 
        game.playMusic(game.scenes[game.currentScene].music);
    }
    else if (game.currentScene == 25) {
        int result = runBossBattle(window, game);
        game.currentScene = (result == 555) ? 26 : 18;
        game.playMusic(game.scenes[game.currentScene].music);
    }

    window.clear();
    drawScene(window, game);
    window.display();
}
    
    return 0;
}