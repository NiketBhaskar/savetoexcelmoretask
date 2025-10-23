#include "adascontrol.h"
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

adasControl::adasControl(QObject *parent) : QObject(parent)
{
    //DATA INCOMING FROM ADAS INTERFACE///////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(dataTriggerCommand()));

    //CENTRE SCREEN INPUT/////////////////////////////////////////////////////////////
    udpADAScomms = new UDPSocket(this);                                           //Setup UDP Socket to communicate ADAS messages
    udpADAScomms->ConnectSingle(7002);                                            //ADAS messages ?
    connect(udpADAScomms,SIGNAL(messageReceived(QByteArray)),this,SLOT(recievedDataADAS(QByteArray)));

}

void adasControl::initObjects(UDPMessagingOutput *udpmessagingoutput_ptr, drivingDataInput *drivingDataInput_ptr)
{
    updOutputData = udpmessagingoutput_ptr;
    draData = drivingDataInput_ptr;
}

//Created in order to receive data from proto and send the correct message to the controler
void adasControl::recievedDataADAS(QByteArray ADASmessage)
{
    ADASmessageBuff = ADASmessage;                                         //Buffer message received

    if(ADASmessageBuff.contains("_")){
        QStringList ADASmessageList = ADASmessageBuff.split("_");           //Split the message using underscore
        qDebug() << "ADAS message received: " <<ADASmessageList[0],ADASmessageList[1];
        controllerADAS(ADASmessageList[0], ADASmessageList[1]);             //Send message of desired state and speed to the controler
    }else{
        qDebug() << "ADAS message received: " <<ADASmessageBuff;
        controllerADAS(ADASmessageBuff);                                    //Send message of desired state with default speed (0 mph)
    }
}

//desiredSetSpeed is an optional parameter, with a default value of 0
void adasControl::controllerADAS(QString desiredState, QString desiredSetSpeed)
{
    switch (desiredState.toInt()) {
    case 0: //Manual
        if (ADAState != 0) {
            ADAState = 0;
            setSpeed = desiredSetSpeed.toInt();
            qDebug() << "ADAS STATE: " << QString::number(ADAState);
            qDebug() << "SET SPEED: " << QString::number(setSpeed);
            updOutputData->sendADASMessage("Manual");
        }
        break;
    case 1: //Speed Limiter
        if (ADAState != 1 || desiredSetSpeed.toInt() != setSpeed) {
            ADAState = 1;
            setSpeed = desiredSetSpeed.toInt();
            qDebug() << "ADAS STATE: " << QString::number(ADAState);
            qDebug() << "SET SPEED: " << QString::number(setSpeed);
            updOutputData->sendADASMessage("SpdLimiter_" + QString::number(convertSpeed(desiredSetSpeed)));
        }
        else {
            if (draData->brake.toInt() > 1000){
                ADAState = 0;
                setSpeed = 0;
                qDebug() << "ADAS STATE: " << QString::number(ADAState);
                qDebug() << "SET SPEED: " << QString::number(setSpeed);
                updOutputData->sendADASMessage("Manual");
            }
        }
        break;
    case 2: //CC
        if (ADAState != 2 || desiredSetSpeed.toInt() != setSpeed) {
            ADAState = 2;
            setSpeed = desiredSetSpeed.toInt();
            qDebug() << "ADAS STATE: " << QString::number(ADAState);
            qDebug() << "SET SPEED: " << QString::number(setSpeed);
            updOutputData->sendADASMessage("CC_" + QString::number(convertSpeed(desiredSetSpeed)));
        }
        else {
            if (draData->brake.toInt() > 1000){
                ADAState = 0;
                setSpeed = 0;
                qDebug() << "ADAS STATE: " << QString::number(ADAState);
                qDebug() << "SET SPEED: " << QString::number(setSpeed);
                updOutputData->sendADASMessage("Manual");
            }
        }
        break;
    case 3: //ACC
        if (ADAState != 3 || desiredSetSpeed.toInt() != setSpeed) {
            ADAState = 3;
            setSpeed = desiredSetSpeed.toInt();
            qDebug() << "ADAS STATE: " << QString::number(ADAState);
            qDebug() << "SET SPEED: " << QString::number(setSpeed);
            updOutputData->sendADASMessage("AutoCC_" + QString::number(convertSpeed(desiredSetSpeed)));
        }
        else {
            if (draData->brake.toInt() > 1000){
                ADAState = 0;
                setSpeed = 0;
                qDebug() << "ADAS STATE: " << QString::number(ADAState);
                qDebug() << "SET SPEED: " << QString::number(setSpeed);
                updOutputData->sendADASMessage("Manual");
            }
        }
        break;
    case 4: //L2+
        if (ADAState != 4 || desiredSetSpeed.toInt() != setSpeed) {
            ADAState = 4;
            setSpeed = desiredSetSpeed.toInt();
            qDebug() << "ADAS STATE: " << QString::number(ADAState);
            qDebug() << "SET SPEED: " << QString::number(setSpeed);
            updOutputData->sendADASMessage("AutoFull_" + QString::number(convertSpeed(desiredSetSpeed)));
        }
        else {
            if (draData->brake.toInt() > 1000){
                ADAState = 0;
                setSpeed = 0;
                qDebug() << "ADAS STATE: " << QString::number(ADAState);
                qDebug() << "SET SPEED: " << QString::number(setSpeed);
                updOutputData->sendADASMessage("Manual");
            }
        }
        break;
    case 5: //SET +
        if (ADAState > 0){
            qDebug() << "ADAS STATE: " << "SET +";
            qDebug() << "SET SPEED: " << QString::number(setSpeed);
            updOutputData->sendADASMessage("SpdUp");
        }
        else{
            qDebug() << "Cannot speed up if in manual mode";
        }
        break;
    case 6: //SET -
        if (ADAState > 0){
            qDebug() << "ADAS STATE: " << "SET -";
            qDebug() << "SET SPEED: " << QString::number(setSpeed);
            updOutputData->sendADASMessage("SpdDown");
        }
        else{
            qDebug() << "Cannot speed down if in manual mode";
        }
        break;
    case 7: //Lane Change Right
        if (ADAState == 4){
            qDebug() << "ADAS STATE: " << "LC Right";
            updOutputData->sendADASMessage("LaneRight");
        }
        else{
            qDebug() << "Cannot chage lanes if not in L2";
        }
        break;
    case 8: //Lane Change Left
        if (ADAState == 4){
            qDebug() << "ADAS STATE: " << "LC Left";
            updOutputData->sendADASMessage("LaneLeft");
        }
        else{
            qDebug() << "Cannot chage lanes if not in L2";
        }
        break;
    default:
        break;
    }
}

int adasControl::convertSpeed(QString speed){
    int finalSpeed = speed.toInt();
    finalSpeed = (finalSpeed - 5) * 1.61;    //Need to check on this conversion
    return finalSpeed;
}

void adasControl::dataTriggerCommand()                                     //Timer Trigger
{
    timer->stop();                                                              //Stop timer and reset
}

//Updates the state of the ADAS state machine, used to allow continous updates of the state machine
void adasControl::updateADAS()
{
    //qInfo() << "ADAS STATE: " << QString::number(ADAState);
    //qInfo() << "SET SPEED: " << QString::number(setSpeed);
    controllerADAS(QString::number(ADAState), QString::number(setSpeed));
}

//Unused
void adasControl::testADAS(QString desiredState){
    updOutputData->sendADASMessage(desiredState);
}


