#include "Scene.hpp"
#include <vector>

std::vector<Scene> loadScenes() {
    return {
        // 0
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png"},
            "nachalo.ogg",
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
            {28, 3}
        },
        //3
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "nachalo.ogg",
            L"Лео:Привет, Луна! Слушай, я нашел старую карту в библиотеке. Говорят, она ведет к Лунному Саду!",
            {L"Поинтересоваться", L"Не неси чепуху, фуфел"},
            {4, 28}
        },
        //4
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png",  "assets/chars/leo.png"},
            "nachalo.ogg",
            L"Лео:Я слышал, что медальон, который ты недавно нашла, \n может помочь нам попасть туда. Можешь показать мне его?",
            {L"Да", L"Нет"},
            {5, 28}
        },
        //5
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "nachalo.ogg",
            L"Да, это тот самый медальон. Слушай, а давай вместе пойдем туда?",
            {L"Ну го", L"Мб это опасно?"},
            {7, 6}
        },
        //6
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "nachalo.ogg",
            L"Разве это хоть раз тебя останавливало?",
            {L"Верно, кто не рискует, то не сдаёт рк по физие", L"Не хочу искать приключений на задницу"},
            {7, 28}
        },
        //7
        {
            "assets/bg/luna_room.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "nachalo.ogg",
            L"С нами пойдут мои друзья, не против?",
            {L"Конечно нет", L"А разве есть выбор?(нет)"},
            {8, 8}
        },
        //8
        {
            "assets/bg/forest_path.png",
            {},
            "nachalo.ogg",
            L"Спустя час",
            {L"Далее"},
            {9}
        },
        //9
        {
            "assets/bg/forest_path.png",
            {"assets/chars/luna.png", "assets/chars/leo.png","assets/chars/stella.png", "assets/chars/tima.png"},
            "nachalo.ogg",
            L"Вот мы и в сборе, перед нами 3 пути. С кем из компаньенов пойдешь?",
            {L"Лео", L"Не хватило времени", L"Не хватило времени"},
            {10, 10, 10}
        },
        //10
        {
            "assets/bg/forest_path.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "nachalo.ogg",
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
            {30, 21}
        },
        //16
        {
            "assets/bg/castle1.png",
            {"assets/chars/luna.png", "assets/chars/leo.png"},
            "mimic_here.ogg",
            L"Перед вами сундук и дверь",
            {L"Открыть сундук", L"Пройти дальше"},
            {17}
        }, 
        //17
        {
            "assets/bg/castle1.png",
            {},
            "mimic_here.ogg",
            L"",
            {},
            {-1} // Специальная метка для мини-игры
        }, 
        //18
        {
            "assets/bg/luna_dead.png",
            {""},
            "demon_win.ogg",
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
            "boss_fight.ogg",
            L"Выйд из замка, вы оказались в портале. приключения не ждут!",
            {L"Войти в портал"},
            {-1}
        },
        //26
        {
            "assets/bg/demon_lose.png",
            {},
            "boss_fight.ogg",
            L"Луна побеждает демона и триумфально отрезает ему голову",
            {L"Далее"},
            {27}
        },
        //27
        {
            "assets/bg/leo_dead.png",
            {},
            "boss_fight1.ogg",
            L"После победы, Луна похоронила Лео. Это приключение окончилось не так радостно, как ребята ожидали.",
            {L"Конец"},
            {-2}
        },
        //28
        {
            "assets/bg/leo_endd.png",
            {},
            "boss_fight1.ogg",
            L"Лео уходит из дома Луны",
            {L"Далее"},
            {29}
        },
        //29
        {
            "assets/bg/leo_dead.png",
            {},
            "boss_fight1.ogg",
            L"Вскоре, Луна узнает, что ее товарищ погиб и приходит с ним попрощаться.",
            {L"Конец"},
            {-2}
        },
        //30
        {
            "assets/bg/castle1.png",
            {},
            "mimic_here.ogg",
            L"Сундук оказался мимиком, приготовьтесь жать пробел.",
            {L"Далее"},
            {17}
        },

    };
}