#include <QCoreApplication>
#include <QDebug>
#include <QCommandLineParser>
#include "agnssagentserver.h"

int main(int argc, char *argv[])
{
        QCoreApplication a(argc, argv);
	qDebug("AGNSS agent server (v%d.%d).", MAJOR_VERSION, MINOR_VERSION);

	a.setApplicationVersion(QString().asprintf("%1.%2").arg(MAJOR_VERSION).arg(MINOR_VERSION));

	QCommandLineParser parser;
	parser.setApplicationDescription("This is an AGNSS agent server.");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption opt_user(QStringList()<<"u"<<"user", "AGPS Username", "user", "");
	parser.addOption(opt_user);

	QCommandLineOption opt_passwd(QStringList()<<"s"<<"password", "AGPS Password", "password", "");
	parser.addOption(opt_passwd);

	QCommandLineOption opt_port(QStringList()<<"p"<<"port", "Server port", "port", "3131");
	parser.addOption(opt_port);


	parser.process(a);

	QString userstr = parser.value(opt_user).left(40);
	QString passwdstr = parser.value(opt_passwd).left(40);
	unsigned short port = parser.value(opt_port).toUShort();

	if (userstr == "") {
		qDebug()<<"Missing username.";
		return -1;
	}

	if (passwdstr == "") {
		qDebug()<<"Missing password.";
		return -1;
	}

        AGnssAgentServer server;
	server.setOptions(userstr, passwdstr, port);
        server.start();

        return a.exec();
}
