#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "udpsocket.h"
#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>


class ledcontrol : public QObject

{
    Q_OBJECT


public:

    explicit ledcontrol(QObject *parent = nullptr);
    ~ledcontrol();                              //Multicast Frequency
    QString ProtoMessage = "0";
    UDPSocket *udpLEDcomms;
    QTimer *timer = new QTimer(this);                                   //Time for polling the Multicast port


public slots:

    void changePattern(QString desiredPattern);
    void getProData(QByteArray LEDpattern);
    void dataTriggerCommand();


private:

    QSerialPort *serialPort;

};

#endif // LEDCONTROL_H




