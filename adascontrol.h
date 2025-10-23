#ifndef ADASCONTROL_H
#define ADASCONTROL_H

#include "udpsocket.h"
#include <QObject>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QTimer>

class UDPMessagingOutput;       //Required for performing the UDP calls to Open Module in STISIM
class drivingDataInput;         //Required to check the braking and throttle to disengage features

//Class that contains all required information about ADAS state and implements the state machine for changing between different stages
class adasControl: public QObject
{
    Q_OBJECT

    public:
        explicit adasControl(QObject *parent = 0);
        UDPSocket *udpADAScomms;                                            //New socket for ADAS Comms
        UDPMessagingOutput *updOutputData;                                  //Pointer to output messaging
        drivingDataInput *draData;                                          //Pointer to driving data object
        QTimer *timer = new QTimer(this);                                   //Time for polling the Multicast port
        int signalRefreshTimer = 50;                                        //Multicast Frequency
        QString ADASmessageBuff = "Manual";                                 //Buffer data to pass incoming ADAS message to
        int ADAState = 0;                                                   //Int to determine current ADAS feature state - 0 = Manual, 1 = SpdLim, 2 = ACC, 3 = L2+
        int setSpeed = 0;                                                   //Int to determine the current setSpeed for the active feature

    public slots:                          
        void controllerADAS(QString desiredState, QString desiredSetSpeed = "0");     //State Machine for ADAS features
        void recievedDataADAS(QByteArray taskData);                             //Function to update when ADAS Features are requested
        int  convertSpeed(QString speed);
        void updateADAS();                                                      //Update for ADAS State Machine
        void testADAS(QString desiredState);
        void dataTriggerCommand();                                              //Timer Trigger
        void initObjects(UDPMessagingOutput *udpmessagingoutput_ptr, drivingDataInput *drivingDataInput_ptr);


};

#endif // ADASCONTROL_H
