#include "swcontrol.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include "udpmessagingoutput.h"
#include "drivingdatainput.h"
#include <QVariant>

SWControl::SWControl(QObject *parent)
    : QObject{parent}
{
    //DATA INCOMING FROM ADAS INTERFACE///////////////////////////////////////////////
    /// \brief connect


    //CENTRE SCREEN INPUT/////////////////////////////////////////////////////////////
    SWControlComms = new UDPSocket(this);                                           //Setup UDP Socket to communicate ADAS messages
    SWControlComms->ConnectSingle(7778);                                            //ADAS messages ?
    connect(SWControlComms,SIGNAL(messageReceived(QByteArray)),this,SLOT(recievedDataSWControl(QByteArray)));
    // <-- initialize sender socket
    senderSocket = new QUdpSocket(this);
}

void SWControl::initObjects(UDPMessagingOutput *udpmessagingoutput_ptr, drivingDataInput *drivingDataInput_ptr)
{
    updOutputData = udpmessagingoutput_ptr;
    draData = drivingDataInput_ptr;
}

void SWControl::forwardToOtherApp(const QByteArray &payload)
{
    // Send to localhost:9999
    if (senderSocket) {
        senderSocket->writeDatagram(payload, QHostAddress::LocalHost, 7113);
    }
}
//Created in order to receive data from proto and send the correct message to the controler
void SWControl::recievedDataSWControl(QByteArray ADASmessage)
{
    // Print raw bytes just to see what they look like
       //qDebug() << "raw bytes:" << ADASmessage.toHex(' ');
//        qDebug() << "[SWControl] Received UDP message, size:" << ADASmessage.size();
       // Make sure we have at least 10 bytes (5 * 2)
       if (ADASmessage.size() < 10) {
           qWarning() << "Message too short:" << ADASmessage.size() << "bytes";
           return;
       }

       // Read as little-endian 16-bit values
       QDataStream ds(ADASmessage);
       ds.setByteOrder(QDataStream::LittleEndian);

       quint16 f1, f2, f3, f4, f5;
       ds >> f1 >> f2 >> f3 >> f4 >> f5;
//       emit newButtonEvent("SteeringWheel", QString("Raw f1=%1 f2=%2 f3=%3 f4=%4 f5=%5")
//                                             .arg(f1).arg(f2).arg(f3).arg(f4).arg(f5));
//       qDebug() << "[SWControl] Emitted newButtonEvent with fields:" << f1 << f2 << f3 << f4 << f5;

       static const QMap<quint16, QString> buttonMap = {
                 {783, "CAN"},
                 {1039, "RES"},
                 {271, "Set+"},
                 {527, "Set-"},
                 {1295, "RightCentre"},
                 {2319, "Mode"},
                 {2063, "SteeringWheel"},
                 {2575, "ADAS Setting"}
             };

             QString currentButton = buttonMap.value(f4, "");

             // Button pressed
             if (f4 != 15 && currentButton != "" && currentButton != lastButtonName) {
                 emit newButtonEvent("SteeringWheel", currentButton + "Pressed");
                 qDebug() << "[SWControl] Emitted:" << currentButton + "Pressed";
                 lastButtonName = currentButton;
             }

             // Button released
             if (f4 == 15 && lastButtonName != "") {
                 emit newButtonEvent("SteeringWheel", lastButtonName + "Released");
                 qDebug() << "[SWControl] Emitted:" << lastButtonName + "Released";
                 lastButtonName = "";
             }

  //     qDebug() << "Decoded fields:" << f1 << f2 << f3 << f4 << f5;
       // <-- forward raw data to another app
       forwardToOtherApp(ADASmessage);
}
