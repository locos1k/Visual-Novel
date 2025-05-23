#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <iostream>

using namespace std;

struct Scene {
    string background;
    vector<string> characters;
    string music;
    wstring dialog;
    vector<wstring> choices;
    vector<int> nextScenes;
};

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
    if (music.empty()) { // Если музыка не указана для сцены
        if (currentMusic) currentMusic->stop();
        currentMusic = nullptr;
        return;
    }

    auto it = musicMap.find(music);
    if (it != musicMap.end()) {
        if (currentMusic != it->second.get()) { // Только если это новая музыка
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

class Entity {
public:
    sf::Sprite sprite;
    sf::RectangleShape healthBar;
    int maxHP = 0;
    int currentHP = 0;
    int attackMin = 0;
    int attackMax = 0;
    int defense = 0;

    void setup(const sf::Texture& texture, int hp, int atkMin, int atkMax, int def, sf::Vector2f position) {
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

    void updateHealthBar() {
        float healthPercentage = static_cast<float>(currentHP) / maxHP;
        healthBar.setSize(sf::Vector2f(100 * healthPercentage, 10));
        healthBar.setPosition(sprite.getPosition().x - 50, sprite.getPosition().y - 20);
    }

    int attack() {
        return rand() % (attackMax - attackMin + 1) + attackMin;
    }

    void takeDamage(int damage) {
        int actualDamage = std::max(0, damage - defense);
        currentHP = std::max(0, currentHP - actualDamage);
        updateHealthBar();
    }
};

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
    sf::Text title(L"Лунный Сад", game.font, 50);
    title.setPosition(400, 100);
    
    vector<wstring> options = {L"1. Новая игра", L"2. Загрузить", L"3. Выход"};
    vector<sf::Text> menuItems;
    
    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text item(options[i], game.font, 35);
        item.setPosition(450, 250 + i * 100);
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
                    ifstream save("save.dat");
                    if (save) {
                        save >> game.currentScene;
                        game.inMenu = false;
                    }
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    window.close();
                }
            }
        }
        
        window.clear(sf::Color(30, 30, 120));
        window.draw(title);
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

    // Инициализация объектов
    sf::Sprite player(playerTex);
    player.setPosition(200, 500); // Луна слева
    player.setScale(0.5f, 0.5f);

    sf::Sprite mimic(mimicClosedTex);
    mimic.setPosition(1000, 100); // Мимик справа
    mimic.setScale(1.2f, 1.2f);

    // Полоса прогресса
    sf::RectangleShape progressBar(sf::Vector2f(1000, 20));
    progressBar.setFillColor(sf::Color::Red);
    sf::RectangleShape backgroundBar(sf::Vector2f(1000, 20));
    backgroundBar.setFillColor(sf::Color(128, 128, 128));
    backgroundBar.setPosition(140, 300);
    
    sf::Text infoText;
    infoText.setFont(game.font);
    infoText.setCharacterSize(32);
    infoText.setFillColor(sf::Color::White);

    // Переменные игры
    float playerForce = 0.f;
    float botForce = 0.f;
    float ropePosition = 640.f; // Центр окна (1280x900)
    bool spacePressed = false;
    bool gameOver = false;
    int result = 0;
    sf::Clock clock;
    const float MAX_POSITION = 1180.f;
    const float MIN_POSITION = 100.f;

    // Главный цикл
    while (window.isOpen() && result == 0) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::KeyPressed && !gameOver) {
                if (event.key.code == sf::Keyboard::Space && !spacePressed) {
                    playerForce += 3.f; // Увеличиваем силу Луны
                    spacePressed = true;
                }
            }
            
            if (event.type == sf::Event::KeyReleased && 
                event.key.code == sf::Keyboard::Space) {
                spacePressed = false;
            }
        }

        // Обновление игры
        if (!gameOver) {
            // Автоматическое увеличение силы мимика
            if (clock.getElapsedTime().asSeconds() > 0.5f) {
                botForce += 3.8f;
                clock.restart();
            }

            // Перемещение каната
            ropePosition += (botForce - playerForce) * 0.7f;
            ropePosition = std::clamp(ropePosition, MIN_POSITION, MAX_POSITION);

            // Проверка условий завершения
            if (ropePosition <= MIN_POSITION) { // Победа
                gameOver = true;
                result = 555;
                mimic.setTexture(mimicOpenTex);
                infoText.setString(L"Вы победили мимика!\nНажмите пробел для продолжения");
            } 
            else if (ropePosition >= MAX_POSITION) { // Поражение
                gameOver = true;
                result = 666;
                mimic.setTexture(mimicOpenTex);
                infoText.setString(L"Вас поглотил мимик!\nНажмите пробел для продолжения");
            }

            // Постепенное снижение сил
            playerForce = std::max(0.f, playerForce - 0.3f);
            botForce = std::max(0.f, botForce - 0.2f);
        }

        // Отрисовка
        window.clear(sf::Color(30, 30, 50));
        
        // Обновление прогресс-бара
        float progressWidth = ((ropePosition - MIN_POSITION) / (MAX_POSITION - MIN_POSITION)) * 1000;
        progressBar.setSize(sf::Vector2f(progressWidth, 20));
        progressBar.setPosition(140, 300);

        // Текст с инструкцией
        infoText.setPosition(140, 200);
        if (!gameOver) {
            infoText.setString(L"Жми ПРОБЕЛ, чтобы тянуть канат!");
        }

        // Отрисовка элементов
        window.draw(backgroundBar);
        window.draw(progressBar);
        window.draw(player);
        window.draw(mimic);
        window.draw(infoText);
        window.display();
    }
    
    // Ожидание нажатия пробела для продолжения
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
    // Загрузка текстур
    sf::Texture playerTex, bossTex;
    if (!playerTex.loadFromFile("assets/chars/luna.png") || 
        !bossTex.loadFromFile("assets/chars/stellar_demon.png")) {
        return -1;
    }

    // Инициализация объектов
    Entity player, boss;
    player.setup(playerTex, 100, 20, 15, 20, sf::Vector2f(200, 400));
    boss.setup(bossTex, 200, 12, 8, 10, sf::Vector2f(1000, 400));

    // Текст интерфейса
    sf::Text infoText;
    infoText.setFont(game.font);
    infoText.setCharacterSize(32);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(400, 50);

    // Игровые переменные
    int mana = 30;
    int potions = 5;
    bool playerTurn = true;
    bool gameOver = false;
    int result = 0;
    
    // Главный цикл битвы
    while (window.isOpen() && !gameOver) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (playerTurn && event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Num1: // Атака
                        boss.takeDamage(player.attack());
                        playerTurn = false;
                        break;
                    case sf::Keyboard::Num2: // Защита
                        player.defense += 5;
                        if(player.defense > 30){
                            player.defense = 30;
                        }
                        playerTurn = false;
                        break;
                    case sf::Keyboard::Num3: // Зелье
                        if(potions > 0) {
                            player.currentHP += 30;
                            potions--;
                        }
                        playerTurn = false;
                        break;
                }
            }
        }

        // Ход босса
        if(!playerTurn && !gameOver) {
            int demonAction = rand() % 3;
            switch(demonAction) {
                case 0: // Обычная атака
                    player.takeDamage(boss.attack());
                    break;
                case 1: // Сильная атака
                    player.takeDamage(40);
                    break;
                case 2: // Проклятие
                    player.defense = max(0, player.defense - 3);
                    break;
            }
            playerTurn = true;
        }

        // Проверка условий победы
        if(player.currentHP <= 0) {
            gameOver = true;
            result = 666;
        }
        else if(boss.currentHP <= 0) {
            gameOver = true;
            result = 555;
        }

        // Отрисовка
        window.clear();
        
        // Полоски здоровья
        player.updateHealthBar();
        boss.updateHealthBar();

        // Текст с информацией
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

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 900), "Лунный Сад");
    GameState game;
    setlocale(LC_ALL, "Russian");
    
    // Загрузка шрифта
    if (!game.font.loadFromFile("assets/fonts/main.ttf")) {
        cerr << "Ошибка загрузки шрифта!" << endl;
        return -1;
    }
    
    // Инициализация сцен
    game.scenes = {
        // 0
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png"},
            "music1_calm.ogg",
            L"Луна просыпается в своей комнате. Искорка мурлычет у окна. Внезапно кот подталкивает лапой старый медальон.",
            {L"Взять медальон", L"Проигнорировать"},
            {1, 1}
        },
        //1
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png"},
            "stuk.ogg",
            L"Вдруг в дверь кто-то застучал. Луна смотрит в окно и видит, что это Лео",
            {L"Открыть дверь", L"Подождать пока Лео уйдет"},
            {3, 2}
        },
        //2
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", },
            "stuk1.ogg",
            L"Стук не прекращается",
            {L"не открывать", L"открыть"},
            {125, 3}
        },
        //3
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "music1_calm.ogg",
            L"Лео:Привет, Луна! Слушай, я нашел старую карту в библиотеке. Говорят, она ведет к Лунному Саду!",
            {L"Поинтересоваться", L"Не неси чепуху, фуфел"},
            {4, 125}
        },
        //4
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png",  "assets/chars/leo.png"},
            "music1_calm.ogg",
            L"Лео:Я слышал, что медальон, который ты недавно нашла, \n может помочь нам попасть туда. Можешь показать мне его?",
            {L"Да", L"Нет"},
            {5, 125}
        },
        //5
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "music1_calm.ogg",
            L"Да, это тот самый медальон. Слушай, а давай вместе пойдем туда?",
            {L"Ну го", L"Мб это опасно?"},
            {7, 6}
        },
        //6
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "music1_calm.ogg",
            L"Разве это хоть раз тебя останавливало?",
            {L"Верно, кто не рискует, то не сдаёт рк по физие", L"Не хочу искать приключений на задницу"},
            {7, 125}
        },
        //7
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "music1_calm.ogg",
            L"С нами пойдут мои друзь, не против?",
            {L"Конечно нет", L"А разве есть выбор?(нет)"},
            {8, 8}
        },
        //8
        {
            "assets/bg/forest_path.png",
            {},
            "music1_calm.ogg",
            L"Спустя час",
            {L"Далее"},
            {9}
        },
        //9
        {
            "assets/bg/forest_path.png",
            {"assets/chars/luna.png", "assets/chars/leo.png","assets/chars/stella.png", "assets/chars/tima.png"},
            "music1_calm.ogg",
            L"Вот мы и в сборе, перед нами 3 пути. С кем из компаньенов пойдешь?",
            {L"Лео", L"Стелла", L"Тимо"},
            {10, 125, 125}
        },
        //10
        {
            "assets/bg/forest_path.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "music1_calm.ogg",
            L"Я так рад, что ты выбрала меня. Пойдем налево",
            {L"Идти"},
            {11}
        },
        //11
        {
            "assets/bg/castle.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "castle_enter.ogg",
            L"Вы пошли с Лео и оказались у входа в замок",
            {L"Лео, мне страшно", L"Зайти внутрь"},
            {12, 13}
        },
        //12
        {
            "assets/bg/castle.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "castle_enter.ogg",
            L"Да брось, мы ведь такой путь проделали.",
            {L"Ладно, ты прав. Пойдем", L"Нет, нет, я туда не пойду"},
            {13, 125}
        },
        //13
        {
            "assets/bg/castle1.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "merzost.ogg",
            L"Вы вошли в замок, но не заметили ловушку. Двери захлопнулись",
            {L"АААА, что же делать", L"Собраться с мыслями и пойти дальше"},
            {14, 15}
        },
        //14
        {
            "assets/bg/castle1.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "castle_enter.ogg",
            L"Лео успакаивает тебя",
            {L"Далее"},
            {15}
        },   
        //15
        {
            "assets/bg/castle1.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "castle_enter.ogg",
            L"Перед вами сундук и дверь",
            {L"Открыть сундук", L"Пройти дальше"},
            {16, 25}
        },
        //16
        {
            "assets/bg/castle1.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "castle_enter.ogg",
            L"Перед вами сундук и дверь",
            {L"Открыть сундук", L"Пройти дальше"},
            {17}
        }, 
        //17
        {
            "assets/bg/castle1.png",
            {},
            "",
            L"",
            {},
            {-1} // Специальная метка для мини-игры
        }, 
        //18
        {
            "assets/bg/luna_dead.png",
            {""},
            "end.ogg",
            L"Вы погибли",
            {L"Конец игры"},
            {-2}
        },   
        //19
        {
            "assets/bg/castle1.png",
            {""},
            "vict.ogg",
            L"Пронесло, но нечего рассиживаться, идем дальше",
            {L"Пройти дальше"},
            {20}
        },  
                 
        //20
        {
            "assets/bg/map_merge.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "castle_enter.ogg",
            L"После такого сражения, вы как можно быстрее выбегаете из замка и оказываетесь у портала",
            {L"В замок, я не вернусь"},
            {21}
        },  
        //21
        {
            "assets/bg/final.png",
            {"assets/chars/luna.png", "assets/chars/leo.png", "assets/chars/stellar.png"},
            "boss_fight.ogg",
            L"Вы оказались в логове сектантов. Вдруг из ритуального круга кто-то вылез.",
            {L"Кто ты?", L"Внезапно напасть"},
            {22, 24}
        },
        //22
        {
            "assets/bg/final.png",
            {"assets/chars/luna.png", "assets/chars/leo.png", "assets/chars/stellar.png"},
            "boss_fight.ogg",
            L"Я Стеллар, верховый демон.",
            {L"Ха-ха-ха, демон без крыльев", L"Такая нечисть как ты должна умереть"},
            {23, 24}
        }, 
        //23
        {
            "assets/bg/final.png",
            {"assets/chars/luna.png", "assets/chars/leo.png", "assets/chars/stellar_demon.png"},
            "boss_fight.ogg",
            L"Ты сама напросилась, малявка. Приготовься к смерти.",
            {L"Вступить в бой"},
            {24}
        },  
        //24
        {
            "assets/bg/leo_lose.png",
            {"assets/chars/luna.png"},
            "boss_fight.ogg",
            L"Не успев опомниться, вы видите как Лео нападает на демона. \n Но... Он терпит поражени и умирает",
            {L"Ты умрешь за это, демон!"},
            {25}
        },
        //25
        {
            "assets/bg/final.png",
            {},
            "boss_fight1.ogg",
            L"Выйд из замка, вы оказались в портале. приключения не ждут!",
            {L"Войти в портал"},
            {-1}
        },
        //26
        {
            "assets/bg/demon_lose.png",
            {},
            "boss_fight1.ogg",
            L"Луна побеждает демона и триумфально отрезает ему голову",
            {L"Далее"},
            {27}
        },
        //27
        {
            "assets/bg/leo_dead.png",
            {},
            "boss_fight1.ogg",
            L"Луна побеждает демона и триумфально отрезает ему голову",
            {L"Далее"},
            {27}
        },
        
    };
    
    // Загрузка текстур
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
    
    // Загрузка музыки
    vector<string> musicFiles = {"music1_calm.ogg", "music2_light.ogg", "stuk.ogg", "boss_fight.ogg", "end.ogg",
         "stuk1.ogg", "nachalo.ogg", "castle_enter.ogg", "merzost.ogg", "boss_fight1.ogg"};
for (auto& file : musicFiles) {
    auto music = make_unique<sf::Music>();
    if (!music->openFromFile("assets/music/" + file)) {
        cerr << "Ошибка загрузки музыки: " << file << endl;
        continue;
    }
    game.musicMap.emplace(file, move(music));
}

// +++ Добавьте проверку здесь +++
for (size_t i = 0; i < game.scenes.size(); ++i) {
    const auto& scene = game.scenes[i];
    if (!scene.music.empty() && !game.musicMap.count(scene.music)) {
        cerr << "Предупреждение: Музыка для сцены " << i 
             << " (" << scene.music << ") не загружена!" << endl;
    }
}
    
    // Главный цикл
    showMenu(window, game);
    game.playMusic(game.scenes[game.currentScene].music);
    // В главном игровом цикле (замените текущую обработку сцены 17):
while (window.isOpen()) {
    handleGameInput(window, game);
    
    // Специальная обработка для сцены 17
    if (game.currentScene == 17) {
        int result = runTugOfWarGame(window, game);
        game.currentScene = (result == 555) ? 19 : 18; // Переход на 18 или 125
        game.playMusic(game.scenes[game.currentScene].music);
    }
    else if (game.currentScene == 25) {
        int result = runBossBattle(window, game);
        game.currentScene = (result == 555) ? 26 : 18;
        game.playMusic(game.scenes[game.currentScene].music);
    }

    
    
    // Обычная отрисовка
    window.clear();
    drawScene(window, game);
    window.display();
}
    
    return 0;
}