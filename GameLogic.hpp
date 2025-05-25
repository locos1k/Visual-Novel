#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include "Entity.hpp"

void scaleSprite(sf::Sprite& sprite, const sf::Vector2u& windowSize);
void drawScene(sf::RenderWindow& window, GameState& game);
void handleGameInput(sf::RenderWindow& window, GameState& game);
void showMenu(sf::RenderWindow& window, GameState& game);
int runTugOfWarGame(sf::RenderWindow& window, GameState& game);
int runBossBattle(sf::RenderWindow& window, GameState& game);
