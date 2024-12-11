#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void sendChoice(int sock) {
    int choice;
    cout << "Выберите класс персонажа:\n1. Маг\n2. Воин\n3. Лучник\nВаш выбор: ";
    cin >> choice;

    // Отправка выбора класса на сервер
    send(sock, &choice, sizeof(choice), 0);
}

void gameLoop(int sock) {
    int action;
    while (true) {
        // Получение информации о текущем состоянии
        char buffer[256];
        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cout << "Ошибка получения данных от сервера." << endl;
            break;
        }
        buffer[bytesReceived] = '\0'; // Завершение строки
        cout << buffer; // Вывод информации о состоянии

        // Запрос действия от игрока
        cout << "Выберите действие:\n1. Атаковать\n2. Исцелиться\n3. Использовать способность\nВаш выбор: ";
        cin >> action;

        // Отправка действия на сервер
        send(sock, &action, sizeof(action), 0);

    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7432);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Ошибка подключения к серверу." << endl;
        return -1;
    }

    cout << "Подключено к серверу." << endl;

    // Логика взаимодействия с сервером
    sendChoice(sock); // Отправка выбора класса

    // Запуск игрового цикла
    gameLoop(sock);

    close(sock);
    return 0;
}
