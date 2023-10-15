
#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QSet>
#include <QtCore/QDebug>

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

    }

    void socketDisconnected(QTcpSocket* socket) {

    }
};
