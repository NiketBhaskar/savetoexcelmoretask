#ifndef IMOCAST_H
#define IMOCAST_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>
#include <QVariant>
#include <QTimer>
#include <QHostAddress>

class drivingDataInput;//added now

class imocast:public QObject
{
    Q_OBJECT

public:
    explicit imocast(QObject* parent = nullptr);

    //Pointer to file manager object
    drivingDataInput *draData;//added now

    QHostAddress imoAddress = QHostAddress("192.168.1.106");
    quint16 imoPort = 7117;
    int signalRefreshTimer = 10;//Time for polling the Multicast port

public slots:
    void receiveMulticastData(QVariant time,QVariant lanepos,QVariant steeringangle,QVariant collisions,QVariant speed,QVariant throttle,QVariant braking,
                          QVariant distancetravel,QVariant nearestcar,QVariant leftBlindSpot,QVariant rightBlindSpot,QVariant indicatorStatus,QVariant SD_LP,
                          QVariant SD_S,QVariant unixTime);
    void castData(QString message, QHostAddress address, int port);

private:
    QUdpSocket udpSocket;

};

#endif // IMOCAST_H
