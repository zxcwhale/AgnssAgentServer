#include "agnssagentserver.h"
#include <QDebug>
#include <QDateTime>
#include <QTimerEvent>
#include "casicAgnssAidIni.c"

#define CHECK_CONSTELL(c) ((c) == GPS || (c) == BDS)
#define CONSTELL_NAME(c) ((c) == GPS ? "GPS" : "BDS")

AGnssAgentServer::AGnssAgentServer(QObject *parent) : QObject(parent)
{
        server = new QTcpServer;
        connect(server, SIGNAL(newConnection()), this, SLOT(onAgnssConnection()));

	for (int i = 0; i < MAX_CONSTELL_COUNT; i++) {
                ephSockets[i] = new QTcpSocket;
		ephData[i] = QByteArray();
		tmpData[i] = QByteArray();
		ephTimeStamp[i] = 0;
	}

	connect(ephSockets[GPS], SIGNAL(connected()), this, SLOT(gpsEphConnected()));
	connect(ephSockets[GPS], SIGNAL(readyRead()), this, SLOT(gpsEphReadyRead()));
	connect(ephSockets[BDS], SIGNAL(connected()), this, SLOT(bdsEphConnected()));
	connect(ephSockets[BDS], SIGNAL(readyRead()), this, SLOT(bdsEphReadyRead()));

        ephTimerId = startTimer(EPH_UPDATE_INTERVAL * 1000);
}

void AGnssAgentServer::start()
{
	//qDebug("AGnss server start(v%d.%d), listen port: %d", MAJOR_VERSION, MINOR_VERSION, PORT);
	qDebug("AGnss server start(v%d.%d), listen port: %d", MAJOR_VERSION, MINOR_VERSION, port_no);
	qDebug()<<"EPH data source:"<<EPH_BANNER;
        qDebug()<<"-------------------------------------------------";
        qDebug()<<"Accept message formats:";
        qDebug()<<"cmd=?; '?' must be one of 'full', 'eph', 'aid'.";
        qDebug()<<"lat=?; '?' must be a float number.";
        qDebug()<<"lon=?; '?' must be a float number.";
	qDebug()<<"gnss=?; '?' can be 'gps', 'bds' or 'gps+bds'.";
	qDebug()<<"For example: 'cmd=full;lat=39.12;lon=114.21;gnss=gps+bds;'";
        qDebug()<<"-------------------------------------------------";
	server->listen(QHostAddress::Any, port_no);
	getEphDataFromServer(GPS);
	getEphDataFromServer(BDS);
}

void AGnssAgentServer::setOptions(QString u, QString w, unsigned short p)
{
	qDebug()<<"Setup Options:"<<"User="<<u<<", Password="<<w<<", Port="<<p;
	username = u;
	password = w;
	port_no = p;

}

void AGnssAgentServer::timerEvent(QTimerEvent *event)
{
        if (event->timerId() == ephTimerId) {
		getEphDataFromServer(GPS);
		getEphDataFromServer(BDS);
	}
}

void AGnssAgentServer::onAgnssConnection()
{
        QTcpSocket *client = server->nextPendingConnection();
        qDebug()<<"New connection established: "<<client->peerAddress();
        connect(client, SIGNAL(readyRead()), this, SLOT(onClientMessage()));
        connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
}

void AGnssAgentServer::onClientMessage()
{
        QTcpSocket *client = (QTcpSocket*)sender();
        QByteArray message = client->readAll();
        qDebug()<<"Client:"<<client->peerAddress();
        qDebug()<<"Message:"<<message;

        QByteArray reply = packReplyMessage(message);
        client->write(reply);
        qDebug()<<"Reply"<<reply.count()<<"bytes to client.";
        client->disconnectFromHost();
        qDebug()<<"Close connection!\n";
}

void AGnssAgentServer::ephConnected(int constell)
{
	if (!CHECK_CONSTELL(constell))
		return;

	tmpData[constell].clear();
	// tempEphData.clear();
        qDebug()<<"AGNSS server connected.";
	qDebug()<<CONSTELL_NAME(constell)<<"On Connected";

        qDebug()<<"******************************************";
	//qDebug()<<"User:"<<USERNAME<<"Password:"<<PASSWORD;
	qDebug()<<"User:"<<username<<"Password:"<<password;
	//if (strcmp(USERNAME, "freetrial") == 0)
	if (username == "freetrial")
                qDebug()<<"WARNING: THIS IS A freetrial ACCOUNT!!!";
        qDebug()<<"******************************************";

	char request[256];
	//sprintf(request, constell == GPS ? GPS_EPH_REQUEST : BDS_EPH_REQUEST, USERNAME, PASSWORD);
	snprintf(request, sizeof(request), constell == GPS ? GPS_EPH_REQUEST : BDS_EPH_REQUEST, username.toLocal8Bit().data(), password.toLocal8Bit().data());

	ephSockets[constell]->write(request);
        qDebug()<<"Request message:"<<request;
}

void AGnssAgentServer::ephReadyRead(int constell)
{
	if (!CHECK_CONSTELL(constell))
		return;

	QByteArray d = ephSockets[constell]->readAll();
	qDebug()<<CONSTELL_NAME(constell)<<"On Ready Read";
	qDebug()<<"Received"<<d.count()<<"bytes eph data from server.";
        qDebug()<<"Data:"<<d.left(128)<<"...";
	tmpData[constell].append(d);
	if (checkEphDataLength(tmpData[constell])) {
		ephData[constell].clear();
		bool key_recved = false;
		foreach(uchar ch, tmpData[constell]) {
			if (ch == EPH_KEY_BYTE)
				key_recved  = true;

			if (key_recved)
				ephData[constell].append(ch);
                }
		ephTimeStamp[constell] = QDateTime::currentMSecsSinceEpoch() / 1000;
                qDebug()<<"EPH data updated!\n";

		tmpData[constell].clear();
	}
}

void AGnssAgentServer::gpsEphConnected()
{
	ephConnected(GPS);

}

void AGnssAgentServer::gpsEphReadyRead()
{
	ephReadyRead(GPS);

}

void AGnssAgentServer::bdsEphConnected()
{
	ephConnected(BDS);

}

void AGnssAgentServer::bdsEphReadyRead()
{
	ephReadyRead(BDS);
}

QByteArray AGnssAgentServer::packReplyMessage(QByteArray clientMessage)
{
	struct client_message msg;
	memset(&msg, 0, sizeof(msg));
	msg.gnss |= BIT_MASK(GPS);	// Set GPS as default
	msg.mask[GNSS] = true;

        unpackClientMessage(clientMessage, &msg);

        QByteArray payload = packPayload(msg);
        qDebug("Payload length = %d bytes", payload.length());

        QByteArray reply;
        reply.append(QString("%1 AGNSS agent server.\n").arg(EPH_BANNER));
        reply.append(QString("%1: %2.\n").arg(EPH_LENGTH_FLAG).arg(payload.length()));
        reply.append("\n");
        reply.append(payload);
        return reply;
}

QByteArray AGnssAgentServer::packPayload(struct client_message msg)
{
	if (!msg.mask[CMD])
                return QByteArray("Missing or invalid \"cmd=?;\".");

	if (!msg.mask[LON])
                return QByteArray("Missing or invalid \"lon=?;\".");

	if (!msg.mask[LAT])
                return QByteArray("Missing or invalid \"lat=?;\".");

	if (!msg.mask[GNSS])
		return QByteArray("Invalid \"gnss=?;\".");

        QByteArray payload;
        if (msg.cmd & AID) {
                QByteArray aidIniMessage = packCurrentAidIniMessage(msg.lat, msg.lon);
                qDebug()<<"Add AID INI message";
                payload.append(aidIniMessage);
        }

        if (msg.cmd & EPH) {
                qDebug()<<"Add EPH messages.";
		if (msg.gnss & BIT_MASK(GPS)) {
			if (ephExpired(GPS))
				qDebug()<<"WARNING: GPS Eph data expired!!!";
			payload.append(ephData[GPS]);
		}
		if (msg.gnss & BIT_MASK(BDS)) {
			if (ephExpired(BDS))
				qDebug()<<"WARNING: BDS Eph data expired!!!";
			payload.append(ephData[BDS]);
		}
	}
        return payload;
}

void AGnssAgentServer::unpackClientMessage(QByteArray message, struct client_message *pMsg)
{
        // message: "lat=34.123412;lon=114.219213;"
        QByteArrayList tokenList = message.toLower().split(';');
        foreach(QByteArray token, tokenList) {
                QByteArrayList keyAndValue = token.split('=');
                if (keyAndValue.count() == 2) {
                        QByteArray key = keyAndValue.at(0);
                        QByteArray value = keyAndValue.at(1);
                        bool ok;
                        if (key == "cmd") {
                                if (value == "full") {
                                        pMsg->cmd = FULL;
                                        pMsg->mask[CMD] = true;
                                }
                                else if (value == "eph") {
                                        pMsg->cmd = EPH;
                                        pMsg->mask[CMD] = true;
                                }
                                else if (value == "aid") {
                                        pMsg->cmd = AID;
                                        pMsg->mask[CMD] = true;
                                }
                                else {
                                        qDebug()<<"WARNING: invalid cmd:"<<value;
                                }
                        }
                        else if (key == "lat") {
                                pMsg->lat = value.toDouble(&ok);
                                if (!ok) {
                                        qDebug()<<"WARNING: invalid lat:"<<value;
                                }
                                pMsg->mask[LAT] = ok;
                        }
                        else if (key == "lon") {
                                pMsg->lon = value.toDouble(&ok);
                                if (!ok) {
                                        qDebug()<<"WARNING: invalid lon:"<<value;
                                }
                                pMsg->mask[LON] = ok;
                        }
			else if (key == "gnss") {
				pMsg->gnss = 0;
				pMsg->mask[GNSS] = false;

				if (value.contains("gp"))
					pMsg->gnss |= BIT_MASK(GPS);
				if (value.contains("bd"))
					pMsg->gnss |= BIT_MASK(BDS);

				if (pMsg->gnss)
					pMsg->mask[GNSS] = true;
			}
                }
        }
}

QByteArray AGnssAgentServer::packCurrentAidIniMessage(double lat, double lon)
{
        DATETIME_STR datetime;
        QDateTime dt = QDateTime::currentDateTimeUtc();
        datetime.year = dt.date().year();
        datetime.month = dt.date().month();
        datetime.day = dt.date().day();
        datetime.hour = dt.time().hour();
        datetime.minute = dt.time().minute();
        datetime.second = dt.time().second();
        datetime.ms = (float)dt.time().msec();
        datetime.valid = 1;

        POS_LLA_STR lla;
        lla.alt = 0.0;
        lla.lat = lat;
        lla.lon = lon;
        lla.valid = 1;

        qDebug("DATETIME: valid=%d, date=%d-%02d-%02d, time=%02d:%02d:%02d, ms=%f",
               datetime.valid,
               datetime.year, datetime.month, datetime.day,
               datetime.hour, datetime.minute, datetime.second, datetime.ms);
        qDebug("LLA:      valid=%d, lat=%f, lon=%f, alt=%f", lla.valid, lla.lat, lla.lon, lla.alt);

        qDebug("Pack %s AID INI Message.", EPH_BANNER);

        char aidIniMsg[AID_INI_SIZE];
        memset(aidIniMsg, 0, AID_INI_SIZE);
        casicAgnssAidIni(datetime, lla, aidIniMsg);
        return QByteArray(aidIniMsg, AID_INI_SIZE);
}

void AGnssAgentServer::getEphDataFromServer(int constell)
{
	if (!CHECK_CONSTELL(constell))
		return;

	qDebug()<<"Start update EPH data"<<CONSTELL_NAME(constell);
        qDebug()<<"Try to connect to server:"<<EPH_SERVER<<":"<<EPH_PORT;
	if (ephSockets[constell]->state() != QTcpSocket::UnconnectedState) {
		ephSockets[constell]->abort();
                qDebug()<<"Eph socket: Abort!";
        }
	ephSockets[constell]->connectToHost(EPH_SERVER, EPH_PORT);
}

bool AGnssAgentServer::checkEphDataLength(QByteArray data)
{
        qDebug()<<"Check EPH data";
        int n = getEphDataLength(data);
        qDebug()<<"Get EPH data length:"<<n;
	if (n == 0)
                return false;

        int counter = countEphDataLength(data);
        qDebug()<<"Count EPH data length:"<<counter;
	if (counter == 0)
                return false;

        if (counter == n) {
                qDebug()<<"EPH data length check passed.";
                return true;
        }

        else {
		if (n > counter)
                        qDebug()<<"Wait for more EPH data.";
		else
                        qDebug()<<"EPH data length unmatched:"<<n<<counter;
        }
        return false;
}

int AGnssAgentServer::getEphDataLength(QByteArray data)
{
        QByteArrayList lineList = data.split('\n');
        if (lineList.count() > 2) {
                QByteArray lengthLine = lineList.at(1);
                if (lengthLine.startsWith(EPH_LENGTH_FLAG)) {
                        int n = 0;
			foreach(char c, lengthLine)
				if (c >= '0' && c <= '9')
                                        n = n * 10 + c - '0';

                        return n;
                }
        }
        return 0;
}

int AGnssAgentServer::countEphDataLength(QByteArray data)
{
        int counter = 0;
        bool startCount = false;
        foreach(uchar b, data) {
		if (b == EPH_KEY_BYTE)
                        startCount = true;

		if (startCount)
                        counter += 1;
        }
        return counter;
}

bool AGnssAgentServer::ephExpired(int constell)
{
	if (!CHECK_CONSTELL(constell))
		return false;

        qint64 timestamp = QDateTime::currentMSecsSinceEpoch() / 1000;
	return timestamp - ephTimeStamp[constell] > EPH_EXPIRED_SECONDS;
}
