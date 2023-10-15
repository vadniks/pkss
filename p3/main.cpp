
#include "Server.hpp"
#include <QtCore/QCoreApplication>
#include "Tasks.hpp"

int main(int argc, char** argv) {
    qInfo() << Tasks::t19(5);
    qInfo() << Tasks::t22(10);
    qInfo() << Tasks::t25(1.0f);


//    QCoreApplication app(argc, argv);
//    Server server;
//    return QCoreApplication::exec();
    return 0;
}
