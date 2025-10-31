#ifndef SWCONTROL_H
#define SWCONTROL_H
#include "udpsocket.h"
#include <QObject>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QTimer>

class Server;                   // ✅ Forward declaration
class UDPMessagingOutput;       //Required for performing the UDP calls to Open Module in STISIM
class drivingDataInput;         //Required to check the braking and throttle to disengage features

class SWControl : public QObject
{
    Q_OBJECT
public:
    explicit SWControl(QObject *parent = nullptr);
    UDPSocket *SWControlComms;                                            //New socket for ADAS
    UDPMessagingOutput *updOutputData;                                  //Pointer to output messaging

    UDPMessagingOutput *SWControlData;                                  //Pointer to output messaging
    drivingDataInput *draData;                                          //Pointer to driving data object
    Server *tcpServer;                                                  // ✅ Pointer to TCP server for Android communication
    int signalRefreshTimer = 50;                                        //Multicast Frequency
    QString SWControlmessageBuff = "Manual";                                 //Buffer data to pass incoming ADAS message to
    int ADAState = 0;                                                   //Int to determine current ADAS feature state - 0 = Manual, 1 = SpdLim, 2 = ACC, 3 = L2+
    int setSpeed = 0;                                                   //Int to determine the current setSpeed for the active feature
    QString lastButtonName = "";
public slots:
    void recievedDataSWControl(QByteArray taskData);                             //Function to update when ADAS Features are requested
    void initObjects(UDPMessagingOutput *udpmessagingoutput_ptr, drivingDataInput *drivingDataInput_ptr, Server *server_ptr); // ✅ Added Server parameter
signals:
    void newButtonEvent(QString component, QString event);
private:
    QUdpSocket *senderSocket = nullptr;   // <-- add this
    void forwardToOtherApp(const QByteArray &payload); // <-- add this
};
#endif // SWCONTROL_H
