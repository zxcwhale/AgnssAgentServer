#ifndef AGNSSAGENTSERVER_H
#define AGNSSAGENTSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#define EPH_BANNER "CASIC"
#define EPH_SERVER "www.gnss-aide.com"
#define EPH_PORT 2621
#define GPS_EPH_REQUEST "user=%s;pwd=%s;cmd=eph;lat=0.0;lon=0.0;gnss=gps;"
#define BDS_EPH_REQUEST "user=%s;pwd=%s;cmd=eph;lat=0.0;lon=0.0;gnss=bds;"
#define USERNAME "freetrial"
#define PASSWORD "123456"
#define EPH_KEY_BYTE 0xBA
#define EPH_LENGTH_FLAG "DataLength"
#define AID_INI_SIZE 66

#define PORT 3131
#define EPH_UPDATE_INTERVAL 1800 // seconds
#define EPH_EXPIRED_SECONDS 7200 // seconds

#define MAJOR_VERSION	2
#define MINOR_VERSION	1


#define BIT_MASK(c) (1 << (c))
enum {
        GPS,
        BDS,
        MAX_CONSTELL_COUNT,
};

typedef enum {
        CMD = 0,
        LAT = 1,
        LON = 2,
	GNSS = 3,
        MAX_MASK,
} CLIENT_MESSAGE_MASK;

typedef enum {
        EPH = 0x1,
        AID = 0x2,
        FULL = EPH | AID,
} CLIENT_MESSAGE_CMD;

struct client_message {
        int cmd;
	int gnss;
        double lat;
        double lon;
        bool mask[MAX_MASK];
};

class AGnssAgentServer : public QObject
{
        Q_OBJECT
public:
        explicit AGnssAgentServer(QObject *parent = 0);
        void start();

protected:
        virtual void timerEvent(QTimerEvent *event);

signals:

private slots:
        void onAgnssConnection();
        void onClientMessage();

	void gpsEphConnected();
	void gpsEphReadyRead();

	void bdsEphConnected();
	void bdsEphReadyRead();
private:
        QByteArray packReplyMessage(QByteArray message);
	QByteArray packPayload(struct client_message msg);
	void unpackClientMessage(QByteArray message, struct client_message *pMsg);
        QByteArray packCurrentAidIniMessage(double lat, double lon);
	void getEphDataFromServer(int constell);
        bool checkEphDataLength(QByteArray data);
        int getEphDataLength(QByteArray data);
        int countEphDataLength(QByteArray data);
	bool ephExpired(int constell);

	void ephConnected(int constell);
	void ephReadyRead(int constell);


private:
        QTcpServer *server;
        int ephTimerId;

	qint64 ephTimeStamp[MAX_CONSTELL_COUNT];
        QTcpSocket *ephSockets[MAX_CONSTELL_COUNT];
	QByteArray ephData[MAX_CONSTELL_COUNT];
	QByteArray tmpData[MAX_CONSTELL_COUNT];
};

#endif // AGNSSAGENTSERVER_H
