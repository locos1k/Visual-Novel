#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <map>
#include <fstream>

sf::RenderWindow window(sf::VideoMode(800, 600), "Визуальная новелла");

struct Egg {
    sf::Sprite sprite;
    float x, y;
    float speed;
};

struct Wolf {
    sf::Sprite sprite;
    float x, y;
    float speed;
};

void playReactionTest(sf::RenderWindow& window) {
    // Инициализация
    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        return;
    }

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("resources/pong.wav")) {
        return;
    }
    sf::Sound sound;
    sound.setBuffer(buffer);

    // Звук победы
    sf::SoundBuffer victoryBuffer;
    if (!victoryBuffer.loadFromFile("resources/vict.ogg")) {
        return;
    }
    sf::Sound victorySound;
    victorySound.setBuffer(victoryBuffer);

    sf::RectangleShape target(sf::Vector2f(50, 50));
    target.setFillColor(sf::Color::Red);
    target.setPosition(-100, -100);

    sf::Text instructions;
    instructions.setFont(font);
    instructions.setString(L"Нажмите пробел, когда увидите красный квадрат");
    instructions.setCharacterSize(24);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(100, 100);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString(L"Игра окончена! Нажмите R для повтора");
    gameOverText.setPosition(100, 300);

    int score = 0;
    int attempts = 3;
    bool gameActive = false;
    bool targetVisible = false;
    bool canPress = true; // Флаг для кд
    bool victorySoundPlayed = false; // Флаг для звука победы
    sf::Clock cooldownClock; // Таймер для кд
    float cooldownTime = 0.2f; // 0.2 секунды кд

    sf::Clock gameClock;
    sf::Clock reactionClock;

    // Сброс игры
    auto resetGame = [&]() {
        score = 0;
        attempts = 3;
        gameActive = true;
        targetVisible = false;
        canPress = true;
        victorySoundPlayed = false;
        target.setPosition(-100, -100);
        gameClock.restart();
    };

    // Основной игровой цикл
    resetGame();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    resetGame();
                }
            }
        }

        // Обработка ввода
        if (gameActive) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                if (canPress) { // Проверка кд
                    if (targetVisible) {
                        sound.play();
                        float reactionTime = reactionClock.getElapsedTime().asSeconds();
                        score += static_cast<int>(1000 / reactionTime);
                        targetVisible = false;
                        gameClock.restart();
                    } else {
                        attempts--;
                        if (attempts <= 0) {
                            gameActive = false;
                        }
                    }
                    canPress = false; // Установка флага кд
                    cooldownClock.restart(); // Запуск таймера кд
                }
            }
        }

        // Проверка кд
        if (!canPress && cooldownClock.getElapsedTime().asSeconds() > cooldownTime) {
            canPress = true; // Сброс кд
        }

        // Обновление
        if (gameActive) {
            if (!targetVisible && gameClock.getElapsedTime().asSeconds() > 2.0f) {
                int x = rand() % (800 - 50);
                int y = rand() % (600 - 50);
                target.setPosition(x, y);
                targetVisible = true;
                reactionClock.restart();
            }
        }

        // Отрисовка
        window.clear();
        if (gameActive) {
            window.draw(target);
            window.draw(instructions);
            scoreText.setString(L"Очки: " + std::to_wstring(score) + L" | Попыток: " + std::to_wstring(attempts));
            window.draw(scoreText);
        } else {
            sf::Text resultText;
            resultText.setFont(font);
            resultText.setCharacterSize(36);
            resultText.setFillColor(sf::Color::Green);
            
            if (score > 12000) {
                if (!victorySoundPlayed) {
                    victorySound.play();
                    victorySoundPlayed = true;
                }
                return;
            } else {
                resultText.setString(L"Поражение! Ваш результат: " + std::to_wstring(score));
            }
            resultText.setPosition(100, 250);
            window.draw(resultText);

            gameOverText.setString(L"Нажмите R для повтора");
            window.draw(gameOverText);
        }
        window.display();

        // Ограничение частоты кадров
        sf::sleep(sf::milliseconds(16));
    }
}

void playEggCollector(sf::RenderWindow& window) {
    // Инициализация
    sf::Clock clock;
    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) return;

    // Загрузка ресурсов
    sf::Texture wolfTexture, eggTexture, backgroundTexture;
    if (!wolfTexture.loadFromFile("resources/wolf.png") || 
        !eggTexture.loadFromFile("resources/egg.png") || 
        !backgroundTexture.loadFromFile("resources/background.png")) {
        return;
    }

    // Инициализация волка
    Wolf wolf;
    wolf.sprite.setTexture(wolfTexture);
    wolf.x = 400;
    wolf.y = 435;
    wolf.sprite.setPosition(wolf.x, wolf.y);
    wolf.speed = 5;

    // Инициализация фона
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    // Инициализация яиц
    std::vector<Egg> eggs;
    sf::SoundBuffer collectBuffer;
    if (!collectBuffer.loadFromFile("resources/pong.wav")) return;
    sf::Sound collectSound;
    collectSound.setBuffer(collectBuffer);

    // Счет и жизни
    int score = 0;
    int lives = 3;

    // Текст для отображения счета и жизней
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    sf::Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(30);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(10, 50);

    // Текст для экрана победы/поражения
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(100, 250);

    // Кнопка "Повторить попытку"
    sf::Text retryText;
    retryText.setFont(font);
    retryText.setCharacterSize(30);
    retryText.setFillColor(sf::Color::Yellow);
    retryText.setString(L"Нажмите R для повтора");
    retryText.setPosition(200, 350);

    bool gameOver = false;
    sf::Clock victoryClock; // Таймер для отсчета 5 секунд после победы
    bool victory = false;

    // Игровой цикл
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                // Перезапуск игры
                score = 0;
                lives = 3;
                eggs.clear();
                wolf.x = 400;
                wolf.sprite.setPosition(wolf.x, wolf.y);
                gameOver = false;
                victory = false;
            }
        }

        if (!gameOver) {
            // Управление волком
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && wolf.x > 0) {
                wolf.x -= wolf.speed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && wolf.x < 750) {
                wolf.x += wolf.speed;
            }
            wolf.sprite.setPosition(wolf.x, wolf.y);

            // Генерация яиц
            if (clock.getElapsedTime().asSeconds() > 2.0f) {
                Egg egg;
                egg.sprite.setTexture(eggTexture);
                egg.x = rand() % 750;
                egg.y = 0;
                egg.sprite.setPosition(egg.x, egg.y);
                egg.speed = 2 + rand() % 3;
                eggs.push_back(egg);
                clock.restart();
            }

            // Обновление яиц
            for (auto it = eggs.begin(); it != eggs.end();) {
                it->y += it->speed;
                it->sprite.setPosition(it->x, it->y);

                // Проверка столкновения с волком
                if (wolf.sprite.getGlobalBounds().intersects(it->sprite.getGlobalBounds())) {
                    collectSound.play();
                    score += 10;
                    it = eggs.erase(it);
                    continue;
                }

                // Проверка выхода за пределы экрана
                if (it->y > 600) {
                    lives--;
                    it = eggs.erase(it);
                    if (lives <= 0) {
                        gameOver = true;
                        gameOverText.setString(L"Игра окончена! Вы проиграли.");
                    }
                    continue;
                }

                ++it;
            }

            // Проверка победы
            if (score >= 100 && !victory) {
                victory = true;
                gameOver = true;
                gameOverText.setString(L"Поздравляем! Вы победили!");
                victoryClock.restart(); // Запускаем таймер
                return;
            }

            // Проверка, прошло ли 5 секунд после победы
            if (victory && victoryClock.getElapsedTime().asSeconds() > 5.0f) {
                window.close(); // Закрываем окно мини-игры
                return; // Возвращаемся к новелле
            }
        }

        // Отрисовка
        window.clear();
        window.draw(backgroundSprite);
        window.draw(wolf.sprite);
        for (const auto& egg : eggs) {
            window.draw(egg.sprite);
        }

        scoreText.setString("score: " + std::to_string(score));
        livesText.setString("lives: " + std::to_string(lives));
        window.draw(scoreText);
        window.draw(livesText);

        if (gameOver) {
            window.draw(gameOverText);
            if (!victory) {
                window.draw(retryText);
            }
        }

        window.display();
    }
}

struct Scene {
    std::string background;
    std::string character;
    std::string music;     
    std::wstring dialog;
    std::vector<std::wstring> choices;
    std::vector<int> nextScenes;
};

void showMenu(sf::RenderWindow& window, std::vector<Scene>& scenes, int& currentScene, sf::Font& font, std::map<std::string, sf::Texture>& textures) {
    sf::Text menuTitle;
    menuTitle.setFont(font);
    menuTitle.setCharacterSize(40);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setString(L"Визуальная новелла");
    menuTitle.setPosition(200, 100);

    std::vector<std::wstring> menuOptions = { L"Новая игра", L"Загрузить", L"Выйти" };
    std::vector<sf::Text> menuTexts;

    for (size_t i = 0; i < menuOptions.size(); i++) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::Yellow);
        text.setString(menuOptions[i]);
        text.setPosition(250, 200 + i * 80);
        menuTexts.push_back(text);
    }

    sf::Event event;
    bool menuActive = true;

    while (menuActive) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num3) {
                    int choiceIndex = event.key.code - sf::Keyboard::Num1;
                    if (choiceIndex < menuOptions.size()) {
                        if (choiceIndex == 0) {
                            currentScene = 0;
                            menuActive = false;
                        } else if (choiceIndex == 1) {
                            std::ifstream saveFile("save.txt");
                            if (saveFile.is_open()) {
                                saveFile >> currentScene;
                                saveFile.close();
                                menuActive = false;
                            } else {
                                sf::Text noSave;
                                noSave.setFont(font);
                                noSave.setCharacterSize(24);
                                noSave.setFillColor(sf::Color::Red);
                                noSave.setString(L"Сохранение не найдено");
                                noSave.setPosition(200, 400);
                                window.draw(noSave);
                                window.display();
                                sf::sleep(sf::seconds(1));
                            }
                        } else if (choiceIndex == 2) {
                            window.close();
                        }
                    }
                }
            }
        }

        window.clear();
        window.draw(menuTitle);
        for (const auto& text : menuTexts)
            window.draw(text);
        window.display();
    }
}

int main() {
    setlocale(LC_ALL, "Russian");    

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return -1;

    std::vector<Scene> scenes = {
        // Сцена 0
        {            
            "image.png",
            "",
            "",     
            L"Паук просыпается. В нос ударяет запах протухших аппельсинов.",
            {L"Где я?", L"Вчера последняя была лишней."},
            {1, 1}
            
        },
         //Сцена 1
        {
            "image.png",
            "",
            "music0.ogg",
            L"Все таки предстоит принять непростое решение",
            {L"Продолжать лежать", L"Открыть глаза"},
            {1, 2}
        },
        // Сцена 2
        {
            "trash.png",
            "",
            "music2.ogg",
            L"Глаза Сергея открылись и увидили дичайший вид.",
            {L"Попытаться встать и протрезветь"},
            {3}
        },
        // Сцена 3
        {
            "podvorot.png",
            "char0.png",
            "music3.ogg",
            L"Наконец-то Сергей протрезвел.",
            {L"Далее"},
            {4}
        },
        // Сцена 4
        {
            "street.png",
            "char0.png",
            "", 
            L"Серега на веселе выходит из этой подворотни. Куда идти дальше?",
            {L"Направо", L"Налево"},
            {5, 6}
        },
        // Сцена 5
        {
            "himki.png",
            "",
            "",
            L"Решать надо быстро",
            {L"Идти дальше", L"Бежать со всех ног"},
            {6, 7}
        },
        // Сцена 6: Встреча с загадочной девушкой
        {
            "himki.png", // Парк ночью
            "char1.png", // Девушка
            "music6.ogg", // Музыка для сцены
            L"Вас настиг Андрей Свинский",
            {L"сражаться", L"сопротивляться"},
            {13, 14}
        }
        
    };

    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Music> musicMap; // Карта для хранения музыки

    // Загружаем текстуры и музыку
    for (const auto& scene : scenes) {
        if (!scene.background.empty()) {
            sf::Texture texture;
            if (!texture.loadFromFile(scene.background)) return -1;
            textures[scene.background] = texture;
        }
        if (!scene.character.empty()) {
            sf::Texture texture;
            if (!texture.loadFromFile(scene.character)) return -1;
            textures[scene.character] = texture;
        }        
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
    text.setPosition(50, 500);

    sf::Sprite backgroundSprite;
    sf::Sprite characterSprite;

    int currentScene = 0;
    sf::Music currentMusic;

    bool nachh = false;
    bool himki = false;

    showMenu(window, scenes, currentScene, font, textures);

    bool eggPlay = false;
    bool reactPlay = false;

    while (window.isOpen()) {
        sf::Event event;
        window.clear();
        while (window.pollEvent(event)) {
            if (currentScene == 0 && nachh == false){
                if (!currentMusic.openFromFile("nachalo.ogg")) {
                    return -1;
                }
                nachh = true;
                currentMusic.setVolume(100);
                currentMusic.setLoop(true);
                currentMusic.play();
            }
            if (currentScene == 3 && reactPlay == false){
                playReactionTest(window);
                reactPlay = true;
                currentMusic.stop();
                if (!currentMusic.openFromFile("trezvost.ogg")) {
                    return -1;
                }
                nachh = true;
                currentMusic.setVolume(100);
                currentMusic.setLoop(true);
                currentMusic.play();
            }
            if (currentScene == 5 && himki == false){
                currentMusic.stop();
                if (!currentMusic.openFromFile("himki.ogg")) {
                    return -1;
                }
                nachh = true;
                currentMusic.setVolume(100);
                currentMusic.setLoop(true);
                currentMusic.play();
            }            
            if(currentScene == 9 && eggPlay == false){
                playEggCollector(window);
                eggPlay = true;
            }
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) {
                    std::ofstream saveFile("save.txt");
                    if (saveFile.is_open()) {
                        saveFile << currentScene;
                        saveFile.close();
                    }
                }
                if (event.key.code == sf::Keyboard::L) {
                    std::ifstream saveFile("save.txt");
                    if (saveFile.is_open()) {
                        saveFile >> currentScene;
                        saveFile.close();
                    }
                }
                if (!scenes[currentScene].choices.empty()) {
                    if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9) {
                        int choiceIndex = event.key.code - sf::Keyboard::Num1;
                        if (choiceIndex < scenes[currentScene].nextScenes.size()) {
                            currentScene = scenes[currentScene].nextScenes[choiceIndex];
                        }
                    }
                }
            }
        }  
        
        
        const auto& scene = scenes[currentScene];        

        window.clear(); // Очистка окна перед каждой итерацией

        if (textures.find(scene.background) != textures.end()) {
            backgroundSprite.setTexture(textures[scene.background]);
            // Сбрасываем масштаб спрайта перед загрузкой нового фона
            backgroundSprite.setScale(1.0f, 1.0f);
            // Масштабируем фон, чтобы он соответствовал размеру окна
            backgroundSprite.setScale(
                static_cast<float>(window.getSize().x) / textures[scene.background].getSize().x,
                static_cast<float>(window.getSize().y) / textures[scene.background].getSize().y
            );
            window.draw(backgroundSprite);
        }

        if (!scene.character.empty() && textures.find(scene.character) != textures.end()) {
            characterSprite.setTexture(textures[scene.character]);
            characterSprite.setPosition(600, 100);
            window.draw(characterSprite);
        }

        text.setString(scene.dialog);
        text.setFillColor(sf::Color::Green);
        window.draw(text);

        if (!scene.choices.empty()) {
            for (size_t i = 0; i < scene.choices.size(); i++) {
                sf::Text choiceText;
                choiceText.setFont(font);
                choiceText.setCharacterSize(24);
                choiceText.setFillColor(sf::Color::Yellow);
                choiceText.setString(L"[" + std::to_wstring(i + 1) + L"] " + scene.choices[i]);
                choiceText.setPosition(50, 400 + i * 40);
                window.draw(choiceText);
            }
        }

        window.display();
    }

    return 0;
}