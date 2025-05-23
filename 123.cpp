#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace sf;
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;
const float MAX_FORCE = 100.f;
const float ROPE_SPEED = 0.7f;
const float BOT_REACTION_TIME = 0.2f;
const float SWALLOW_SPEED = 1.f;
const float ESCAPE_SPEED = 3.f;
const float BOT_FORCE_INCREMENT = 2.0f;

class TugOfWarGame {
private:
    RenderWindow window;
    RectangleShape rope;
    Text infoText;
    Font font;
    
    Sprite player;
    Sprite mimic;
    Texture playerTexture;
    Texture mimicClosedTexture;
    Texture mimicOpenTexture;
    
    float playerForce;
    float botForce;
    float ropePosition;
    bool gameOver;
    bool spaceKeyHeld;
    bool swallowing;
    bool escaping;
    float animationProgress;
    Clock animClock;

    void resetGame() {
        playerForce = botForce = 0.f;
        ropePosition = WIDTH / 2.f;
        gameOver = false;
        swallowing = false;
        escaping = false;
        animationProgress = 0.f;
        mimic.setTexture(mimicClosedTexture);
        player.setColor(Color::White);
        player.setPosition(100, HEIGHT/2 - 50);
    }

    void loadTextures() {
        if (!playerTexture.loadFromFile("assets/chars/luna.png") ||
            !mimicClosedTexture.loadFromFile("mimic.png") ||
            !mimicOpenTexture.loadFromFile("mimic.png") ||
            !font.loadFromFile("assets/fonts/main.ttf")) 
        {
            cerr << "Error loading resources!" << endl;
            exit(1);
        }
    }

    void updateMimicAnimation() {
        if (animClock.getElapsedTime().asSeconds() > 0.3f) {
            if (mimic.getTexture() == &mimicClosedTexture) {
                mimic.setTexture(mimicOpenTexture);
            } else {
                mimic.setTexture(mimicClosedTexture);
            }
            animClock.restart();
        }
    }

public:
        TugOfWarGame() : 
        window(sf::VideoMode(WIDTH, HEIGHT), "Mimic Tug of War"), // Добавлен заголовок
        ropePosition(WIDTH/2),
        gameOver(false),
        spaceKeyHeld(false),
        swallowing(false),
        escaping(false),
        animationProgress(0.f) 
    {
        loadTextures();
        
        player.setTexture(playerTexture);
        player.setPosition(100, HEIGHT/2 - 50);
        player.setScale(0.5f, 0.5f);

        mimic.setTexture(mimicClosedTexture);
        mimic.setPosition(WIDTH - 250, HEIGHT/2 - 80);
        mimic.setScale(1.2f, 1.2f);

        infoText.setFont(font);
        infoText.setCharacterSize(32);
        infoText.setPosition(150, HEIGHT/2 - 50);
        infoText.setFillColor(Color::White);
        infoText.setString("Press SPACE to resist!\nDon't get swallowed!");

        rope.setSize(Vector2f(400.f, 10.f));
        rope.setFillColor(Color(200, 150, 50));
        resetGame();
    }

    void run() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Space && !gameOver && !spaceKeyHeld) {
                        playerForce += 2.5f;
                        spaceKeyHeld = true;
                    }
                    
                    if (gameOver && event.key.code == Keyboard::Space) {
                        resetGame();
                    }
                }

                if (event.type == Event::KeyReleased && event.key.code == Keyboard::Space) {
                    spaceKeyHeld = false;
                }
            }

            if (!gameOver) {
                static Clock botActionClock;
                if (botActionClock.getElapsedTime().asSeconds() > BOT_REACTION_TIME) {
                    botForce += BOT_FORCE_INCREMENT;
                    updateMimicAnimation();
                    botActionClock.restart();
                }

                // ИЗМЕНЕНО: Исправлен расчет силы
                float forceDifference = botForce - playerForce; // Теперь бот тянет к себе
                ropePosition += forceDifference * ROPE_SPEED;

                // Условия остаются прежними, но логика изменилась
                if (ropePosition > WIDTH - 250) { // Мимик перетянул к себе
                    swallowing = true;
                    animationProgress += SWALLOW_SPEED;
                    player.setColor(Color(255, 255, 255, 255 - animationProgress));
                    player.move(1.5f, 0); // Движение к мимику
                }
                else if (ropePosition <= 100) { // Игрок набрал максимальную силу
                    escaping = true;
                    animationProgress += ESCAPE_SPEED;
                    player.move(-3.f, 0); // Движение от мимика
                    player.setColor(Color(255, 255, 255, 255 - animationProgress));
                    gameOver = true;
                }

                if (swallowing && animationProgress >= 255) {
                    infoText.setString("SWALLOWED!\nPress SPACE to retry");
                    gameOver = true;
                    mimic.setTexture(mimicOpenTexture);
                }
                else if (escaping && animationProgress >= 255) {
                    infoText.setString("ESCAPED!\nPress SPACE to play again");
                    gameOver = true;
                }

                playerForce = max(0.f, playerForce - 0.3f);
                botForce = max(0.f, botForce - 0.4f);
            }

            window.clear(Color(30, 30, 50));

            // Анимация мимика
            if (swallowing && !gameOver) {
                mimic.setTexture(mimicOpenTexture);
            }
            else if (!gameOver) {
                updateMimicAnimation();
            }

            window.draw(mimic);

            // Анимация игрока
            if ((!gameOver || animationProgress < 255) && !escaping) {
                window.draw(player);
            }

            // Анимация победы
            if (escaping) {
                window.draw(player);
                
                // Создаем эффект побега
                RectangleShape escapeEffect;
                FloatRect playerBounds = player.getGlobalBounds();
                escapeEffect.setSize(Vector2f(playerBounds.width, playerBounds.height));
                escapeEffect.setPosition(playerBounds.left, playerBounds.top);
                
                escapeEffect.setFillColor(Color(0, 255, 0, 50));
                window.draw(escapeEffect);
            }

            rope.setPosition(ropePosition - 200, HEIGHT/2 + 50);
            window.draw(rope);

            window.draw(infoText);

            if (gameOver) {
                RectangleShape overlay(Vector2f(WIDTH, HEIGHT));
                overlay.setFillColor(Color(0, 0, 0, 150));
                window.draw(overlay);
                
                Text resultText;
                resultText.setFont(font);
                resultText.setCharacterSize(48);
                resultText.setPosition(WIDTH/2 - 150, HEIGHT/2 - 50);
                resultText.setString(escaping ? "You Escaped!" : "You Were Eaten!");
                resultText.setFillColor(escaping ? Color::Green : Color::Red);
                
                window.draw(resultText);
                window.draw(infoText);
            }

            window.display();
        }
    }
};

int main() {
    srand(time(NULL));
    TugOfWarGame game;
    game.run();
    return 0;
}