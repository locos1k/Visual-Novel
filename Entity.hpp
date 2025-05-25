#pragma once 

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <fstream>

class Entity {
public:
    sf::Sprite sprite;
    sf::RectangleShape healthBar;
    int maxHP = 0;
    int currentHP = 0;
    int attackMin = 0;
    int attackMax = 0;
    int defense = 0;

    void setup(const sf::Texture& texture, int hp, int atkMin, int atkMax, int def, sf::Vector2f position);
    void updateHealthBar();
    int attack();
    void takeDamage(int damage);
};