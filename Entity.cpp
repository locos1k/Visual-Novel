#include "Entity.hpp"

#include <SFML/Graphics.hpp>

void Entity::setup(const sf::Texture& texture, int hp, int atkMin, int atkMax, int def, sf::Vector2f position) {
    sprite.setTexture(texture);
    maxHP = hp;
    currentHP = hp;
    attackMin = atkMin;
    attackMax = atkMax;
    defense = def;
        
    // Настройка спрайта
    sprite.setPosition(position);
    if (position.x > 400) { // Если это демон (справа)
        sprite.setScale(-1.f, 1.f); // Отражаем по горизонтали
        sprite.setOrigin(sprite.getLocalBounds().width, 0);
    }
        
    // Настройка полоски здоровья
    healthBar.setSize(sf::Vector2f(100, 10));
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setOutlineThickness(1);
    healthBar.setOutlineColor(sf::Color::Black);
    updateHealthBar();
}

void Entity::updateHealthBar() {
    float healthPercentage = static_cast<float>(currentHP) / maxHP;
    healthBar.setSize(sf::Vector2f(100 * healthPercentage, 10));
    healthBar.setPosition(sprite.getPosition().x - 50, sprite.getPosition().y - 20);
}

int Entity::attack() {
    return rand() % (attackMax - attackMin + 1) + attackMin;
}

void Entity::takeDamage(int damage) {
    int actualDamage = std::max(0, damage - defense);
    currentHP = std::max(0, currentHP - actualDamage);
    updateHealthBar();
}