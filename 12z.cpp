#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>

using namespace std;

// Интерфейсы для персонажей
class Character {
public:
    virtual void info() = 0;
    virtual void attack(Character& target) = 0;
    virtual void heal() = 0;
    virtual int getHealth() const = 0;
    virtual bool isAlive() const = 0;
    virtual void takeDamage(int damage) = 0;
    virtual ~Character() = default;

    virtual void useAbility(Character& target) = 0; // Использование способности
    virtual bool canUseAbility() const = 0; // Проверка возможности использования способности
    virtual void resetAbilityCooldown() = 0; // Сброс отката способности
    virtual int getAbilityCooldown() const = 0; // Получение оставшегося отката
};

class Mage : public Character {
private:
    int health = 26;
    bool burning = false;
    int burnDamage = 0;
    int abilityCooldown = 0; // Откат способности

public:
    void info() override {
        cout << "Маг Здоровье: " << health << (burning ? " (горение активно)" : "") << endl;
    }

    void attack(Character& target) override {
        int damage = 7;
        cout << "Маг атакует на " << damage << " урона!" << endl;
        target.takeDamage(damage);
    }

    void heal() override {
        int healAmount = 3;
        health += healAmount;
        cout << "Маг исцеляет себя на " << healAmount << " здоровья. Здоровье: " << health << endl;
    }

    int getHealth() const override {
        return health;
    }

    bool isAlive() const override {
        return health > 0;
    }

    void takeDamage(int damage) override {
        health -= damage;
        cout << "Маг получил " << damage << " урона! Здоровье: " << health << endl;
    }

    void useAbility(Character& target) override {
        if (canUseAbility()) {
            int damage = 6;
            cout << "Маг использует Огненный шар, нанося " << damage << " урона!" << endl;
            target.takeDamage(damage);
            burning = true;
            burnDamage = 2; // Урон от горения в следующие два хода
            abilityCooldown = 5; // Откат 5 ходов
        } else {
            cout << "Способность еще не готова к использованию! Осталось ходов: " << getAbilityCooldown() << endl;
        }
    }

    void applyBurn() {
        if (burning) {
            takeDamage(1);
            burnDamage--;
            if (burnDamage <= 0) {
                burning = false;
            }
        }
    }

    bool canUseAbility() const override {
        return abilityCooldown <= 0;
    }

    void resetAbilityCooldown() override {
        if (abilityCooldown > 0) {
            abilityCooldown--;
        }
    }

    int getAbilityCooldown() const override {
        return abilityCooldown;
    }
};

class Warrior : public Character {
private:
    int health = 38;
    bool shieldActive = false;
    int shieldTurns = 0;
    int abilityCooldown = 0; // Откат способности

public:
    void info() override {
        cout << "Воин Здоровье: " << health << (shieldActive ? " (щит активен)" : "") << endl;
    }

    void attack(Character& target) override {
        int damage = 5;
        cout << "Воин атакует на " << damage << " урона!" << endl;
        target.takeDamage(damage);
    }

    void heal() override {
        int healAmount = 2;
        health += healAmount;
        cout << "Воин исцеляет себя на " << healAmount << " здоровья. Здоровье: " << health << endl;
    }

    int getHealth() const override {
        return health;
    }

    bool isAlive() const override {
        return health > 0;
    }

    void takeDamage(int damage) override {
        if (shieldActive) {
            damage /= 2; // Урон делится пополам, если щит активен
            shieldTurns--;
            if (shieldTurns <= 0) {
                shieldActive = false; // Деактивация щита
                cout << "Щит деактивирован!" << endl;
            }
        }
        health -= damage;
        cout << "Воин получил " << damage << " урона! Здоровье: " << health << endl;
    }

    void useAbility(Character& target) override {
        if (canUseAbility()) {
            cout << "Воин активирует щит!" << endl;
            shieldActive = true;
            shieldTurns = 2; // Щит активен в течение двух ходов
            abilityCooldown = 4; // Откат 4 хода
        } else {
            cout << "Способность еще не готова к использованию! Осталось ходов: " << getAbilityCooldown() << endl;
        }
    }

    bool canUseAbility() const override {
        return abilityCooldown <= 0;
    }

    void resetAbilityCooldown() override {
        if (abilityCooldown > 0) {
            abilityCooldown--;
        }
    }

    int getAbilityCooldown() const override {
        return abilityCooldown;
    }
};

class Archer : public Character {
private:
    int health = 29;
    bool invisible = false;
    int abilityCooldown = 0; // Откат способности
    int invisibilityTurns = 0; // Счетчик оставшихся ходов невидимости

public:
    void info() override {
        cout << "Лучник Здоровье: " << health << (invisible ? " (невидимость активна)" : "") << endl;
    }

    void attack(Character& target) override {
        if (invisible) {
            cout << "Лучник невидим и не может быть атакован!" << endl;
            return; // Невидимый лучник не может быть атакован
        }
        int damage = 8;
        cout << "Лучник атакует на " << damage << " урона!" << endl;
        target.takeDamage(damage);
    }

    void heal() override {
        int healAmount = 5;
        health += healAmount;
        cout << "Лучник исцеляет себя на " << healAmount << " здоровья. Здоровье: " << health << endl;
    }

    int getHealth() const override {
        return health;
    }

    bool isAlive() const override {
        return health > 0;
    }

    void takeDamage(int damage) override {
        if (!invisible) { // Если не невидим, получаем урон
            health -= damage;
            cout << "Лучник получил " << damage << " урона! Здоровье: " << health << endl;
        } else {
            cout << "Лучник невидим и не получает урон!" << endl;
        }
    }

    void useAbility(Character& target) override {
        if (canUseAbility()) {
            cout << "Лучник использует плащ невидимости!" << endl;
            invisible = true;
            invisibilityTurns = 2; // Невидимость активна в течение двух ходов
            abilityCooldown = 3; // Откат 3 хода
        } else {
            cout << "Способность еще не готова к использованию! Осталось ходов: " << getAbilityCooldown() << endl;
        }
    }

    bool canUseAbility() const override {
        return abilityCooldown <= 0;
    }

    void resetAbilityCooldown() override {
        if (abilityCooldown > 0) {
            abilityCooldown--;
        }
    }

    int getAbilityCooldown() const override {
        return abilityCooldown;
    }

    void updateInvisibility() {
        if (invisibilityTurns > 0) {
            invisibilityTurns--;
            if (invisibilityTurns == 0) {
                invisible = false; // Сбросить невидимость
                cout << "Невидимость закончилась!" << endl;
            }
        }
    }
};

// Фабрика для создания персонажей
class CharacterFactory {
public:
    virtual unique_ptr<Character> createMage() = 0;
    virtual unique_ptr<Character> createWarrior() = 0;
    virtual unique_ptr<Character> createArcher() = 0;
    virtual ~CharacterFactory() = default;
};

// Фабрика для героев
class ConcreteFactory : public CharacterFactory {
public:
    unique_ptr<Character> createMage() override {
        return make_unique<Mage>();
    }

    unique_ptr<Character> createWarrior() override {
        return make_unique<Warrior>();
    }

    unique_ptr<Character> createArcher() override {
        return make_unique<Archer>();
    }
};

// Игровой процесс
class Game {
private:
    unique_ptr<Character> player1;
    unique_ptr<Character> player2;
    int clientSocket1;
    int clientSocket2;
    mutex mtx;

public:
    Game(unique_ptr<Character> p1, unique_ptr<Character> p2, int cs1, int cs2)
        : player1(move(p1)), player2(move(p2)), clientSocket1(cs1), clientSocket2(cs2) {}

    void playerTurn(int playerNum, Character& currentPlayer, Character& opponent) {
    int action;
    while (currentPlayer.isAlive() && opponent.isAlive()) {
        // Отправка информации о текущем состоянии игроков
        string info = "Ход игрока " + to_string(playerNum) + ".\n";
        info += "Ваше здоровье: " + to_string(currentPlayer.getHealth()) + "\n";
        info += "Здоровье противника: " + to_string(opponent.getHealth()) + "\n";

        // Отправка информации о состоянии игры на оба клиентских сокета
        send(clientSocket1, info.c_str(), info.size(), 0);
        send(clientSocket2, info.c_str(), info.size(), 0);

        // Запрос действия от игрока
        recv(playerNum == 1 ? clientSocket1 : clientSocket2, &action, sizeof(action), 0);

        switch (action) {
            case 1: // Атаковать
                currentPlayer.attack(opponent);
                break;
            case 2: // Исцелиться
                currentPlayer.heal();
                break;
            case 3: // Использовать способность
                currentPlayer.useAbility(opponent);
                break;
            default:
                cout << "Неверное действие. Попробуйте снова." << endl;
                continue; // Пропустить итерацию цикла
        }

        // Обновление состояния невидимости
        if (auto archer = dynamic_cast<Archer*>(&currentPlayer)) {
            archer->updateInvisibility();
        }

        // Проверка, жив ли противник
        if (!opponent.isAlive()) {
            string victoryMsg = "Игрок " + to_string(playerNum) + " победил!\n";
            send(clientSocket1, victoryMsg.c_str(), victoryMsg.size(), 0);
            send(clientSocket2, victoryMsg.c_str(), victoryMsg.size(), 0);
            cout << "Игрок " << playerNum << " победил!\n"; // Вывод на сервере
            break;
        }

        // Передача хода следующему игроку
        if (playerNum == 1) {
            playerTurn(2, *player2, *player1); // Передаем ход второму игроку
        } else {
            playerTurn(1, *player1, *player2); // Передаем ход первому игроку
        }
    }
}




void startGame() {
        while (player1->isAlive() && player2->isAlive()) {
            playerTurn(1, *player1, *player2);
            if (!player2->isAlive()) {
                break;
            }
            playerTurn(2, *player2, *player1);
        }
    }

void handleClient(int clientSocket, CharacterFactory& factory, int serverSocket) {
        int choice;
        recv(clientSocket, &choice, sizeof(choice), 0);

    // Проверяем, какой игрок подключен (player1 или player2)
    if (!player1) {
        switch (choice) {
            case 1:
                player1 = factory.createMage();
                break;
            case 2:
                player1 = factory.createWarrior();
                break;
            case 3:
                player1 = factory.createArcher();
                break;
            default:
                cout << "Неверный выбор класса!" << endl;
                close(clientSocket);
                return;
        }
        cout << "Игрок 1 выбрал класс: ";
            player1->info();
            clientSocket1 = clientSocket;
    } else if (!player2) {
        switch (choice) {
            case 1:
                player2 = factory.createMage();
                break;
            case 2:
                player2 = factory.createWarrior();
                break;
            case 3:
                player2 = factory.createArcher();
                break;
            default:
                cout << "Неверный выбор класса!" << endl;
                close(clientSocket);
                return;
        }
        cout << "Игрок 2 выбрал класс: ";
            player2->info();
            clientSocket2 = clientSocket;
    }

    if (player1 && player2) {
            // Запуск игрового процесса
            Game game(move(player1), move(player2), clientSocket1, clientSocket2); // Создаем объект игры
            game.startGame(); // Запуск игры
    }
}


};

int main() {
    srand(static_cast<unsigned int>(time(0))); // Инициализация генератора случайных чисел

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(7432);

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 2); // Ожидание двух подключений

    cout << "Сервер запущен. Ожидание игроков..." << endl;

    ConcreteFactory factory;

    unique_ptr<Character> player1 = nullptr;
    unique_ptr<Character> player2 = nullptr;

    // В функции main
    shared_ptr<Game> game;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cerr << "Ошибка при принятии соединения" << endl;
            continue;
        }

        thread t([&](int clientSocket, CharacterFactory& factory, int serverSocket, shared_ptr<Game>& game) {
            if (!game) {
                game = make_shared<Game>(nullptr, nullptr, clientSocket, clientSocket);
            }
            game->handleClient(clientSocket, factory, serverSocket);
        }, clientSocket, ref(factory), serverSocket, ref(game));
        t.detach();
    }

    close(serverSocket);
    return 0;
}
