
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
    Client() : mSocket(this) {
        connect(&mSocket, &QTcpSocket::connected, this, &Client::socketConnected);
        mSocket.connectToHost("127.0.0.1", 8080);
    }

private slots:
    void socketConnected() {
        int command;

        while (mSocket.isOpen()) {
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
                        array.append(a);
                        if (a < 0) break;
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

            mSocket.waitForReadyRead();
            socketReadyToRead();
        }
        end:

        mSocket.close();
        QCoreApplication::quit();
    };

    void socketReadyToRead() {
        const auto array = mSocket.readAll();

        if (array.size() == 0) {
            std::cout << "Disconnected" << std::endl;
            mSocket.close();
            QCoreApplication::quit();
            return;
        }

        const auto json = QJsonDocument::fromJson(array).object();
        switch (json["command"].toInt()) {
            case 19: [[fallthrough]];
            case 22:
                std::cout << "Result: " << static_cast<float>(json["result"].toDouble()) << std::endl;
                break;
            case 25: [[fallthrough]];
            case 28: [[fallthrough]];
            case 3:
                std::cout << "Result: ";
                for (auto i : json["result"].toArray())
                    std::cout << static_cast<float>(i.toDouble()) << ' ';
                std::cout << std::endl;
                break;
            default:
                throw QException();
        }
    }
};
