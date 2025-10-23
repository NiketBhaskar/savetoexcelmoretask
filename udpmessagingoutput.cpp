#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include <direct.h>
#include "udpmessagingoutput.h"
#include "taskdatainput.h"
#include "drivingdatainput.h"
#include "hrhdatainput.h"
#include "eyedatainput.h"
#include "loggingdatatofile.h"
#include "adascontrol.h"

UDPMessagingOutput::UDPMessagingOutput (QObject *parent) : QObject(parent)
{
    //INITIALISE OUTPUT PORTS/////////////////////////////////////////////////////

    //MULTICAST SIGNAL TRANSMITTER////////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(sendCommand()));               //Setup and connect timer

    //MULTICAST SIGNAL OUTPUT/////////////////////////////////////////////////////
    udpSocketMulticast = new UDPSocket(this);                                   //Setup UDP Socket to recieve info from Cluster
    udpSocketMulticast->ConnectMultiCast(5000);                                 //Port Number
    udpSocketMulticast->SetAsMultiCast();                                       //Change to multicast sender

    //SINGLE OUTPUT FOR VISION INTERACTION////////////////////////////////////////
    udpSocketVI = new UDPSocket(this);                                          //Setup UDP Socket to communicate with Questionnaires
    udpSocketVI->ConnectSingle(8001);                                           //Connect

    //SINGLE OUTPUT FOR ADAS///////////////////////////////////////
    udpSocketADAS = new UDPSocket(this);                                        //Setup UDP Socket to communicate with Questionnaires
    udpSocketADAS->ConnectSingle(7071);

    //Connect
//    //SINGLE OUTPUT FOR VISION INTERACTION////////////////////////////////////////
//    udpSocketLEDS = new UDPSocket(this);                                        //Setup UDP Socket to communicate with Questionnaires
//    udpSocketLEDS->ConnectSingle(7777);

}

void UDPMessagingOutput::initObjects(loggingDataToFile *loggingDataToFile_ptr, eyeDataInput *eyeDataInput_ptr, hrhDataInput *hrhDataInput_ptr, drivingDataInput *drivingDataInput_ptr, taskDataInput *taskDataInput_ptr, adasControl *adasData_ptr  )
{   logData = loggingDataToFile_ptr;                                            //Pointers to logging data object
    draData = drivingDataInput_ptr;                                             //Pointer to driving data object
    eyeData = eyeDataInput_ptr;                                                 //Pointer to eye data object
    hrbrData = hrhDataInput_ptr;                                                //Pointer to hrbr data object
    taskData = taskDataInput_ptr;                                               //Pointer totask data object
    adasData = adasData_ptr;
}

void UDPMessagingOutput::launchUDP()                                            //Launch UDP timer - need to find out why this is here.
{
    udpSocketMulticast = new UDPSocket(this);                                   //Setup UDP Socket to recieve info from Cluster

    udpSocketMulticast->ConnectMultiCast(5000);                                 //Cluster

    udpSocketMulticast->SetAsMultiCast();                                       //Change to multicast sender

    timer->start(UDPTimer);                                                     //Start Multicast Output
}

void UDPMessagingOutput::closeUDP()                                             //Close UDP Port
{
    timer->stop();                                                              //Time stop

    delete udpSocketMulticast;                                                  //CLUSTER / HUD / CENTRE SCREEN OUT
}

void UDPMessagingOutput::updateUDPRate(QString rate)                            //Update rate
{
    UDPTimer = rate.toInt();                                                    //Convert string to int
}


void UDPMessagingOutput::sendCommand() //We should modify this if we want to report the ADAS Status to the prototype
{
    QString message;                                                            //Message to send
    QString time = draData->simTime;
    QString lanepos = draData->LanePos;
    QString steeringangle = draData->strWhl;
    QString collisions = draData->crashes;
    QString speed = draData->speed;
    QString steeringcounts = draData->strWhl;
    QString throttle = draData->throttle;
    QString braking = draData->brake;
    QString minrange = draData->nearestcar;
    QString distancetravel = draData->distance;
    QString leftBlindSpot = draData->lbs;
    QString rightBlindSpot = draData->rbs;
    QString vehicleHeadingAngle = draData->vehicleHeadingAngle;
    QString worldposX = draData->worldposX;
    QString worldposY = draData->worldposY;
    QString worldposZ = draData->worldposY;
    QString worldVehicleHeadingAngle = draData->worldVehicleHeadingAngle;
    QString startUpStatus = "0";
    QString aad_state = QString::number(adasData->ADAState);

    float mph_speed = speed.toFloat() *  0.621371;

    QString mph_speed_str = QString::number(mph_speed);



    message.append(time + "," + lanepos + "," + steeringangle + ",");           //1. TIME //2. LANE POSITION //3. STEERING ANGLE
    message.append(collisions + "," + mph_speed_str + "," + steeringcounts + ",");      //4. COLLISIONS //5. SPEED //6. STEERING COUNTS
    message.append(throttle + "," + braking + "," + minrange + ",");            //7. THROTTLE //8. BRAKING //9. MINRANGE
    message.append(distancetravel + "," + globalWarningMessage + ",NULL" + ",");//10. DISTANCE TRAVELLED //11. TRIG EVENTS //12. PARTICIPANT RESPONSE
    message.append(eyeData->GlanceLocation + ",");                              //13. GLANCE LOCATION
    message.append(logData->runNumber + "_" + logData->taskCondition + ",");    //14. CURRENT EXPERIMENT RUN
    message.append(startUpStatus + "," + taskData->steeringWheelString + ",");  //15. START UP INDICATOR //16. STEERING WHEEL CONTROLS
    draData->indicatorStatus = taskData->indicatorStatus;                       //Send Indicator Status to Driving information
    message.append(leftBlindSpot + "," + rightBlindSpot + ",");                 //17. LEFT BLIND SPOT WARNING //18. RIGHT BLIND SPOT WARNING
    message.append(vehicleHeadingAngle + ",");                                  //19.VEHICLE HEADING ANGLE
    message.append(worldposX + ","+ worldposY + "," + worldposZ + ",");         //20.WORLD VEHICLE POSITION X //21.WORLD VEHICLE POSITION Y //22.WORLD VEHICLE POSITION Z
    message.append(worldVehicleHeadingAngle + ",");                             //23.APPEND VEHICLE HEADING ANGLE ONTO DATA TRACE
    message.append(aad_state+","+ taskData->lastEvent+ ",");   //PTI AS               //24.ADAS STATE//25.UI_touch_event


    udpSocketMulticast->sendCommand(message,"224.0.0.0",5000);                  //Send data on Multicast Port
}

void UDPMessagingOutput::sendVIWarningMessage(QString message)
{
    QString messageBuffer = message;                                            //Define signal message

    globalWarningMessage = message;

    udpSocketVI->sendCommand(messageBuffer,"127.0.0.1",8001);                   //Send data on Port - HUD VI project
}

void UDPMessagingOutput::sendADASMessage(QString message)
{
    qInfo() << "Sending ADAS message: " << message;
    QString messageBuffer = message;                                            //Define signal message

    globalWarningMessage = message;

    udpSocketVI->sendCommand(messageBuffer,"192.168.101.21",7071);              //Send data on Port - HUD VI project
}

