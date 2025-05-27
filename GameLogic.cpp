#include "GameLogic.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <iostream>

using namespace std;

void scaleSprite(sf::Sprite& sprite, const sf::Vector2u& windowSize) {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    float scaleX = windowSize.x / bounds.width;
    float scaleY = windowSize.y / bounds.height;
    sprite.setScale(scaleX, scaleY);
}

void drawScene(sf::RenderWindow& window, GameState& game) {
    const Scene& scene = game.scenes[game.currentScene];
    const sf::Vector2u windowSize = window.getSize();

    // Отрисовка фона
    if (game.textures.count(scene.background)) {
        sf::Sprite bg(game.textures[scene.background]);
        scaleSprite(bg, windowSize);
        window.draw(bg);
    }

    // Отрисовка персонажей с автоматическим позиционированием
    if (!scene.characters.empty()) {
        const float BASE_Y = windowSize.y * 0.9f;    // Базовая линия для персонажей
        const float MARGIN_X = windowSize.x * 0.1f;    // Боковые отступы
        const float AREA_WIDTH = windowSize.x - 2 * MARGIN_X;
        const float STEP = AREA_WIDTH / (scene.characters.size() + 1);

        for (size_t i = 0; i < scene.characters.size(); ++i) {
            if (game.textures.count(scene.characters[i])) {
                sf::Sprite charSprite(game.textures[scene.characters[i]]);

                float posX;
                
                // Центрирование по горизонтали
                if(i == 0){
                    posX = MARGIN_X + STEP * (i * 1.2 + 1) - charSprite.getGlobalBounds().width / 1.2;
                }
                else{
                    posX = MARGIN_X + STEP * (i * 1.2 + 2) - charSprite.getGlobalBounds().width / 1.2;
                }
                
                
                // Вертикальное смещение для "оживления" сцены
                float offsetY = (i % 2 == 0) ? 0 : charSprite.getGlobalBounds().height * 0.2f;
                float posY = BASE_Y - charSprite.getGlobalBounds().height + offsetY;
                
                // Позиционирование и отрисовка
                charSprite.setPosition(posX, posY);
                window.draw(charSprite);
            }
        }
    }

    // Отрисовка диалогового текста
    sf::Text dialogText;
    dialogText.setFont(game.font);
    dialogText.setCharacterSize(32);
    dialogText.setFillColor(sf::Color::White);
    dialogText.setOutlineColor(sf::Color::Black);
    dialogText.setOutlineThickness(2);
    dialogText.setPosition(50, windowSize.y - 250);
    dialogText.setString(scene.dialog);
    window.draw(dialogText);

    // Отрисовка вариантов выбора
    const float CHOICES_START_Y = windowSize.y - 150;
    for (size_t i = 0; i < scene.choices.size(); ++i) {
        sf::Text choiceText;
        choiceText.setFont(game.font);
        choiceText.setCharacterSize(28);
        choiceText.setFillColor(sf::Color::Yellow);
        choiceText.setOutlineColor(sf::Color::Black);
        choiceText.setOutlineThickness(1);
        choiceText.setPosition(100, CHOICES_START_Y + i * 50);
        choiceText.setString(to_wstring(i + 1) + L". " + scene.choices[i]);
        window.draw(choiceText);
    }    
}

void handleGameInput(sf::RenderWindow& window, GameState& game) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        
        if (event.type == sf::Event::KeyPressed) {
            const Scene& scene = game.scenes[game.currentScene];
            
            if (event.key.code >= sf::Keyboard::Num1 && 
                event.key.code <= sf::Keyboard::Num9) {
                int choice = event.key.code - sf::Keyboard::Num1;
                if (choice < scene.nextScenes.size()) {
                    game.currentScene = scene.nextScenes[choice];
                    game.playMusic(game.scenes[game.currentScene].music);
                }
            }
            
            if (event.key.code == sf::Keyboard::S) {
                ofstream save("save.dat");
                if (save) save << game.currentScene;
            }
            
            if (event.key.code == sf::Keyboard::L) {
                ifstream save("save.dat");
                if (save) {
                    save >> game.currentScene;
                    game.playMusic(game.scenes[game.currentScene].music);
                }
            }
        }
    }
}


void showMenu(sf::RenderWindow& window, GameState& game) {
    game.playMusic("nachalo.ogg");
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/bg/image.png")) {
        std::cerr << "Failed to load background image!" << std::endl;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
    backgroundSprite.setScale(
        windowSize.x / backgroundTexture.getSize().x,
        windowSize.y / backgroundTexture.getSize().y
    );
    
    vector<wstring> options = {L"1. Новая игра", L"2. Выход"};
    vector<sf::Text> menuItems;
    
    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text item(options[i], game.font, 50);
        item.setPosition(550, 550 + i * 75);
        menuItems.push_back(item);
    }
    
    while (game.inMenu && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    game.currentScene = 0;
                    game.inMenu = false;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    window.close();
                }
            }
        }
        window.clear();
        window.draw(backgroundSprite);
        for (auto& item : menuItems) window.draw(item);
        window.display();
    }
}

int runTugOfWarGame(sf::RenderWindow& window, GameState& game) {
    sf::Texture playerTex, mimicClosedTex, mimicOpenTex;
    if (!playerTex.loadFromFile("assets/chars/luna.png") ||
        !mimicClosedTex.loadFromFile("assets/chars/mimic.png") ||
        !mimicOpenTex.loadFromFile("assets/chars/mimic.png")) 
    {
        return -1;
    }

    sf::Sprite player(playerTex);
    player.setPosition(200, 500); 
    player.setScale(0.5f, 0.5f);

    sf::Sprite mimic(mimicClosedTex);
    mimic.setPosition(1000, 100); 
    mimic.setScale(1.2f, 1.2f);

    sf::RectangleShape progressBar(sf::Vector2f(1000, 20));
    progressBar.setFillColor(sf::Color::Red);
    sf::RectangleShape backgroundBar(sf::Vector2f(1000, 20));
    backgroundBar.setFillColor(sf::Color(128, 128, 128));
    backgroundBar.setPosition(140, 300);
    
    sf::Text infoText;
    infoText.setFont(game.font);
    infoText.setCharacterSize(32);
    infoText.setFillColor(sf::Color::White);

    float playerForce = 0.f;
    float botForce = 0.f;
    float ropePosition = 640.f;
    bool spacePressed = false;
    bool gameOver = false;
    int result = 0;
    sf::Clock clock;
    const float MAX_POSITION = 1180.f;
    const float MIN_POSITION = 100.f;

    while (window.isOpen() && result == 0) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::KeyPressed && !gameOver) {
                if (event.key.code == sf::Keyboard::Space && !spacePressed) {
                    playerForce += 3.f;
                    spacePressed = true;
                }
            }
            
            if (event.type == sf::Event::KeyReleased && 
                event.key.code == sf::Keyboard::Space) {
                spacePressed = false;
            }
        }

        if (!gameOver) {
            if (clock.getElapsedTime().asSeconds() > 0.5f) {
                botForce += 3.8f;
                clock.restart();
            }

            ropePosition += (botForce - playerForce) * 0.7f;
            ropePosition = std::clamp(ropePosition, MIN_POSITION, MAX_POSITION);
            if (ropePosition <= MIN_POSITION) { 
                gameOver = true;
                result = 555;
                mimic.setTexture(mimicOpenTex);
                infoText.setString(L"Вы победили мимика!\nНажмите пробел для продолжения");
            } 
            else if (ropePosition >= MAX_POSITION) { 
                gameOver = true;
                result = 666;
                mimic.setTexture(mimicOpenTex);
                infoText.setString(L"Вас поглотил мимик!\nНажмите пробел для продолжения");
            }

            playerForce = std::max(0.f, playerForce - 0.3f);
            botForce = std::max(0.f, botForce - 0.2f);
        }

        window.clear(sf::Color(30, 30, 50));
        
        float progressWidth = ((ropePosition - MIN_POSITION) / (MAX_POSITION - MIN_POSITION)) * 1000;
        progressBar.setSize(sf::Vector2f(progressWidth, 20));
        progressBar.setPosition(140, 300);
        infoText.setPosition(140, 200);
        if (!gameOver) {
            infoText.setString(L"Жми ПРОБЕЛ, чтобы тянуть канат!");
        }

        window.draw(backgroundBar);
        window.draw(progressBar);
        window.draw(player);
        window.draw(mimic);
        window.draw(infoText);
        window.display();
    }
    
    while (window.isOpen() && result != 0) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed && 
                event.key.code == sf::Keyboard::Space) {
                return result;
            }
        }
    }
    return result;
}

int runBossBattle(sf::RenderWindow& window, GameState& game) {
    sf::Texture playerTex, bossTex;
    if (!playerTex.loadFromFile("assets/chars/luna.png") || 
        !bossTex.loadFromFile("assets/chars/stellar_demon.png")) {
        return -1;
    }
    Entity player, boss;
    player.setup(playerTex, 100, 20, 15, 20, sf::Vector2f(200, 400));
    boss.setup(bossTex, 200, 12, 8, 10, sf::Vector2f(1000, 400));
    sf::Text infoText;
    infoText.setFont(game.font);
    infoText.setCharacterSize(32);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(400, 50);
    
    int potions = 4;
    bool playerTurn = true;
    bool gameOver = false;
    int result = 0;
    
    while (window.isOpen() && !gameOver) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (playerTurn && event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Num1: 
                        boss.takeDamage(player.attack());
                        playerTurn = false;
                        break;
                    case sf::Keyboard::Num2: 
                        player.defense += 5;
                        if(player.defense > 30){
                            player.defense = 30;
                        }
                        playerTurn = false;
                        break;
                    case sf::Keyboard::Num3:
                        if(potions > 0) {
                            player.currentHP += 30;
                            potions--;
                        }
                        playerTurn = false;
                        break;
                }
            }
        }

        if(!playerTurn && !gameOver) {
            int demonAction = rand() % 3;
            switch(demonAction) {
                case 0: 
                    player.takeDamage(boss.attack());
                    break;
                case 1: 
                    player.takeDamage(40);
                    break;
                case 2: 
                    player.defense = max(0, player.defense - 3);
                    break;
            }
            playerTurn = true;
        }

        if(player.currentHP <= 0) {
            gameOver = true;
            result = 666;
        }
        else if(boss.currentHP <= 0) {
            gameOver = true;
            result = 555;
        }

        window.clear();
        
        player.updateHealthBar();
        boss.updateHealthBar();

        infoText.setString(L"1. Атака\n2. Защита\n3. Зелье (" + to_wstring(potions) + L")");

        window.draw(player.sprite);
        window.draw(boss.sprite);
        window.draw(player.healthBar);
        window.draw(boss.healthBar);
        window.draw(infoText);
        window.display();
    }
    
    return result;
}
