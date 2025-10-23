#ifndef UDPMESSAGINGOUTPUT_H
#define UDPMESSAGINGOUTPUT_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "windows.h"
#include "udpsocket.h"

class drivingDataInput;
class hrhDataInput;
class eyeDataInput;
class taskDataInput;
class adasControl;
class loggingDataToFile;

class UDPMessagingOutput : public QObject
{
    Q_OBJECT

    public:
        explicit UDPMessagingOutput(QObject *parent = 0);

        loggingDataToFile *logData;                                 //Pointer to file manager object
        drivingDataInput *draData;                                  //Pointer to driving data object
        hrhDataInput *hrbrData;                                     //Pointer to heart rate data object
        eyeDataInput *eyeData;                                      //Pointer to eye data object
        taskDataInput *taskData;                                    //Pointer to task data object
        adasControl *adasData;                                      //Pointer to adas data object

        UDPSocket *udpSocketMulticast;                              //Socket for multicast transmission
        UDPSocket *udpSocketVI;                                     //Socket for Vision Interaction Message
        UDPSocket *udpSocketADAS;                                   //Socket for Vision Interaction Message
        UDPSocket *udpSocketLED;                                    //Socket for LEDS

        QTimer *timer = new QTimer(this);                           //Time for polling the Multicast port

        QString multicastOnOff = "false";                           //Is the multicast port streaming or not
        QString outputDataOnOffFlag = "false";                      //Output data flag false = off / true = on
        QString globalWarningMessage = "";                          //Warning Message

                                           //PTI AS
        int UDPTimer = 50;                                          //Multicast Frequency

    signals:

    public slots:
        void launchUDP();                                           //Launch UDP Multicast Signal
        void closeUDP();                                            //Close UDP Multicast Signal
        void sendCommand();                                         //Send Multicast Command
        void updateUDPRate(QString rate);                           //Update rate
        void sendVIWarningMessage(QString message);                 //Send message to VI program
        void sendADASMessage(QString message);                      //Send message to VI program
        void initObjects(loggingDataToFile *loggingDataToFile_ptr,
                         eyeDataInput *eyeDataInput_ptr,
                         hrhDataInput *hrhDataInput_ptr,
                         drivingDataInput *drivingDataInput_ptr,
                         taskDataInput *taskDataInput_ptr,
                         adasControl *adasData_ptr);

};

#endif // UDPMESSAGINGOUTPUT_H
