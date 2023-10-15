
#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QCoreApplication>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <iostream>

class Client final : public QObject {
    Q_OBJECT
private:
    QTcpSocket mSocket;

public:
    Client() : mSocket(nullptr) {
        connect(&mSocket, &QTcpSocket::connected, this, &Client::socketConnected);
        connect(&mSocket, &QTcpSocket::readyRead, this, &Client::socketReadyToRead);
        connect(&mSocket, &QTcpSocket::disconnected, this, &Client::socketDisconnected);
        mSocket.connectToHost("127.0.0.1", 8080);
    }

private slots:
    void socketConnected() {
        int command;

        while (true) {
            std::cout << "Enter command: ";
            std::cin >> command;

            QJsonObject output;
            output["command"] = command;

            switch (command) {
                case 19: {
                    std::cout << "Enter one int parameter: ";
                    int a;
                    std::cin >> a;
                    output["parameters"] = QJsonArray({a});
                    } break;
                case 22: {
                    std::cout << "Enter one float parameter: ";
                    float a;
                    std::cin >> a;
                    output["parameters"] = QJsonArray({a});
                    } break;
                case 25: {
                    std::cout << "Enter one float parameter: ";
                    float a;
                    std::cin >> a;
                    output["parameters"] = QJsonArray({a});
                    } break;
                case 28:
                    break;
                case 3: {
                    std::cout << "Enter parameters count: ";
                    int count;
                    std::cin >> count;

                    std::cout << "Enter parameters separated by enter:\n";
                    QJsonArray array;

                    for (int i = 0; i < count; i++) {
                        float a;
                        std::cin >> a;
                        if (a < 0) break;
                        array.append(a);
                    }

                    output["parameters"] = array;
                    } break;
                case 0:
                    break;
                default:
                    goto end;
            }

            if (mSocket.write(QJsonDocument(output).toJson()) == 0) {
                std::cout << "Disconnecting from host due to write error..." << std::endl;
                break;
            }

            const auto array = mSocket.readAll();
            if (array.size() == 0) {
                std::cout << "Disconnecting from host due to read error..." << std::endl;
                break;
            }

            const auto json = QJsonDocument::fromJson(array).object();
            switch (json["command"].toInt()) {
                case 19: [[fallthrough]];
                case 22: [[fallthrough]];
                case 25:
                    std::cout << static_cast<float>(json["result"].toDouble()) << std::endl;
                    break;
                case 28: [[fallthrough]];
                case 3:
                    std::cout << "Result: ";
                    for (auto i : json["result"].toArray())
                        std::cout << static_cast<float>(i.toDouble());
                    std::cout << std::endl;
                    break;
                default:
                    throw QException();
            }
        }
        end:

        mSocket.close();
        QCoreApplication::quit();
    };

    void socketReadyToRead() {

    };

    void socketDisconnected() {
        mSocket.close();
        QCoreApplication::quit();
    }
};
