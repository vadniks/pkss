
#include "Server.hpp"
#include <QtCore/QCoreApplication>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    Server server;
    return QCoreApplication::exec();
}
