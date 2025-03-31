#include <SFML/Graphics.hpp>
#include <vector>

namespace Menu {
class Menu {
    private:
        sf::Font font;
        sf::Text title;
        std::vector<sf::Text> options;
        int selectedOption = 0;
    
    public:
        Menu(float width, float height);
    
        void draw(sf::RenderWindow& window) {
            window.draw(title);
            for (auto& option : options) {
                window.draw(option);
            }
        }
    
        void handleInput(sf::Keyboard::Key key) {
            if (key == sf::Keyboard::Up) {
                if (selectedOption > 0) selectedOption--;
            } else if (key == sf::Keyboard::Down) {
                if (selectedOption < options.size() - 1) selectedOption++;
            }
    
            for (size_t i = 0; i < options.size(); i++) {
                options[i].setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
            }
        }
    
        int getSelectedOption() const {
            return selectedOption;
        }
};
}