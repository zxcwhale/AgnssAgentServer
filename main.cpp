#include <QCoreApplication>
#include <QDebug>
#include "agnssagentserver.h"

int main(int argc, char *argv[])
{
        QCoreApplication a(argc, argv);
        qDebug()<<"Good day, AGNSS agent server.";

        AGnssAgentServer server;
        server.start();

        return a.exec();
}
