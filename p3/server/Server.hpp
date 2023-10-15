
#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QSet>
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QCoreApplication>
#include "Tasks.hpp"

#ifdef assert
#   undef assert
#endif

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

    ~Server() override {
        for (auto i : mSockets) {
            i->close();
            delete i;
        }

        mServer.close();
        qInfo() << "Stopping server...";
        QCoreApplication::quit();
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

    static inline QString peerAddressAndPort(const QTcpSocket* socket) {
        return QString().append(socket->peerAddress().toString()).append(':').append(socket->peerPort());
    }

private slots:
    void newConnection() {
        while (mServer.hasPendingConnections()) {
            auto socket = mServer.nextPendingConnection();
            connect(socket, &QTcpSocket::readyRead, this, [this, socket](){ socketReadyToRead(socket); });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket](){ socketDisconnected(socket); });
            mSockets.insert(socket);
            qInfo() << "connected client " << peerAddressAndPort(socket);
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
            case 0:
                qInfo() << "received stopping signal from client " << peerAddressAndPort(socket);
                socket->disconnectFromHost();
                mServer.close();
                break;
            default:
                break;
        }

        if (socket->write(QJsonDocument(outputJson).toJson()) == 0) {
            qInfo() << "disconnecting client " << peerAddressAndPort(socket) << " in cause of write error...";
            mSockets.remove(socket);
            socket->close();
            delete socket;
        }
    }

    void socketDisconnected(QTcpSocket* socket) {
        qInfo() << "disconnected client " << peerAddressAndPort(socket);
        socket->close();
        mSockets.remove(socket);
        delete socket;
    }
};
