
#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QSet>
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include "Tasks.hpp"

#undef assert
static void assert(bool a) { if (!a) throw QException(); }

class Server final : public QObject {
    Q_OBJECT
private:
    QTcpServer mServer;
    QSet<QTcpSocket*> mSockets;

public:
    Server() : mServer(this) {
        connect(&mServer, &QTcpServer::newConnection, this, &Server::newConnection);
        assert(mServer.listen(QHostAddress::LocalHost, 8080));
        qInfo() << "Server started on " << mServer.serverAddress() << ':' << mServer.serverPort();
    }

private:
    static QJsonArray listToJsonArray(const QList<float>& list) {
        QJsonArray array;
        for (auto i : list)
            array.append(i);
        return array;
    }

    static QList<float> jsonArrayToList(const QJsonArray& array) {
        QList<float> list;
        for (auto i : array)
            list.append(static_cast<float>(i.toDouble()));
        return list;
    }

private slots:
    void newConnection() {
        while (mServer.hasPendingConnections()) {
            auto socket = mServer.nextPendingConnection();
            connect(socket, &QTcpSocket::readyRead, this, [this, socket](){ socketReadyToRead(socket); });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket](){ socketDisconnected(socket); });
            mSockets.insert(socket);
            qInfo() << "connected client " << socket->peerAddress() << ':' << socket->peerPort();
        }
    };

    void socketReadyToRead(QTcpSocket* socket) {
        const auto inputJson = QJsonDocument::fromJson(socket->readAll()).object();
        QJsonObject outputJson;

        switch (inputJson["command"].toInt()) {
            case 19:
                outputJson["result"] = Tasks::t19(inputJson["parameters"].toArray()[0].toInt());
                break;
            case 22:
                outputJson["result"] = Tasks::t22(static_cast<float>(inputJson["parameters"].toArray()[0].toDouble()));
                break;
            case 25:
                outputJson["result"] = listToJsonArray(Tasks::t25(static_cast<float>(inputJson["parameters"].toArray()[0].toDouble())));
                break;
            case 28:
                outputJson["result"] = listToJsonArray(Tasks::t28());
                break;
            case 3:
                outputJson["result"] = listToJsonArray(Tasks::t3(jsonArrayToList(inputJson["parameters"].toArray())));
                break;
        }

        if (socket->write(QJsonDocument(outputJson).toJson()) == 0) {
            qInfo() << "disconnecting client " << socket->peerAddress() << ' ' << socket->peerPort() << " in cause of write error...";
            mSockets.remove(socket);
            socket->close();
            delete socket;
        }
    }

    void socketDisconnected(QTcpSocket* socket) {
        qInfo() << "disconnected client " << socket->peerAddress() << ' ' << socket->peerPort();
        socket->close();
        mSockets.remove(socket);
        delete socket;
    }
};
