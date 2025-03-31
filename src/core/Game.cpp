#include <SFML/Graphics.hpp>
#include <vector>

#include "Game.hpp"

namespace Menu {
Menu::Menu(float width, float height) {
    if (!font.loadFromFile("arial.ttf")) {
        throw std::runtime_error("Ошибка загрузки шрифта");
    }

    title.setFont(font);
    title.setString("Визуальная новелла");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::White);
    title.setPosition(width / 2 - 200, 100);

    std::vector<std::string> items = {"Начать игру", "Выход"};
    for (size_t i = 0; i < items.size(); i++) {
        sf::Text text;
        text.setFont(font);
        text.setString(items[i]);
        text.setCharacterSize(30);
        text.setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
        text.setPosition(width / 2 - 100, 250 + i * 50);
        options.push_back(text);
    }
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(title);
    for (auto& option : options) {
        window.draw(option);
    }
}

void Menu::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Up) {
        if (selectedOption > 0) selectedOption--;
    } else if (key == sf::Keyboard::Down) {
        if (selectedOption < options.size() - 1) selectedOption++;
    }

    for (size_t i = 0; i < options.size(); i++) {
        options[i].setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
    }
}

int Menu::getSelectedOption() const {
    return selectedOption;
}

} // namespace Menu
