#ifndef ANDROIDMULTICAST_H
#define ANDROIDMULTICAST_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>
#include <QVariant>
#include <QTimer>
#include <QHostAddress>

class drivingDataInput;//added now

class androidmulticast:public QObject
{
    Q_OBJECT

public:
    explicit androidmulticast(QObject* parent = nullptr);
    //Pointer to file manager object
    drivingDataInput *draData;//added now

    QHostAddress multicastdroidAddress = QHostAddress("224.0.1.0");
    quint16 multicastdroidport = 5555;
    int signalRefreshTimer = 50;//Time for polling the Multicast port

public slots:
    void getMulticastData(QVariant time,QVariant lanepos,QVariant steeringangle,QVariant collisions,QVariant speed,QVariant throttle,QVariant braking,
                          QVariant distancetravel,QVariant nearestcar,QVariant leftBlindSpot,QVariant rightBlindSpot,QVariant indicatorStatus,QVariant SD_LP,
                          QVariant SD_S,QVariant unixTime);
    void sendMulticastData(QString message, QHostAddress address, int port);

private:
    QUdpSocket udpSocket;

};

#endif // ANDROIDMULTICAST_H




