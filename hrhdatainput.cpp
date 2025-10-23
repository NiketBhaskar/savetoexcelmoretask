#include "hrhdatainput.h"
#include <QFile>
#include <QTextStream>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include "windows.h"
#include "loggingdatatofile.h"
#include "filemanagement.h"
#include <QVariant>

hrhDataInput::hrhDataInput(QObject *parent) : QObject(parent)
{
    //CONNECT TO HEART RATE SENSOR - NEED TO INTEGRATE INTO THIS PROGRAM//////////////////////////////
    hrbr_udpsocket = new UDPSocket(this);
    hrbr_udpsocket->ConnectSingleAltAddress(hrbrPort, hrbrHost);
    connect(hrbr_udpsocket,SIGNAL(messageReceived(QByteArray)),this,SLOT(recievedData(QByteArray)));

    //DATA INCOMING FROM HRBR TRACKER/////////////////////////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(dataTriggerCommand()));                            //Setup and connect timer to check if signal is refreshed
}

void hrhDataInput::initObjects(loggingDataToFile *loggingDataToFile_ptr, filemanagement *filemanagement_ptr)
{
    logData = loggingDataToFile_ptr;                                                                //Pointers to logging data object
    fileData = filemanagement_ptr;                                                                  //Pointer to file management object
}

void hrhDataInput::updateHRBROnOff (QString hrbrFlag)
{
    hrbrOnOff = hrbrFlag;                                                                           //Is Heart rate and breathing rate going to be logged this time?

    qDebug() << "HrBr Line 21";
}

void hrhDataInput::dataTriggerCommand()                                                             //Timer Trigger
{
    timer->stop();                                                                                  //Stop timer and reset

    emit signalHRBRRefresh("false");                                                                //Set interface green

    emit signalHRBRLogging("false");                                                                //Reset if timer expires
}

void hrhDataInput::updatePortSettingsHRBR(QString ipaddress, QString port)
{
    int port_val = port.toInt();                                                                    //Buffer port settings data

    if(port_val != hrbrPort && ipaddress != hrbrHost)                                               //If address and port are new
    {
        hrbr_udpsocket->ConnectSingleAltAddress(port_val, ipaddress);                               //Reset Port Settings
    }
}

void hrhDataInput::updateStartStopHRBR (const QString startstopflag)                                //Function for updating start to log flag
{
    startStopHRBR = startstopflag;                                                                  //Set flag to start stop
}

void hrhDataInput::recievedData(QByteArray heartRate)
{
    ULONGLONG posixTime = makeTime();                                                               //Create Timestamp

    timer->stop();                                                                                  //Stop timer and reset

    timer->start(signalRefreshTimer);                                                               //Start timer

    emit signalHRBRRefresh("true");                                                                 //Set interface green

    s_data = QString::fromUtf8(heartRate.data());                                                   //Decode Heart Rate data

    heartRateVal = s_data.split(",")[1];                                                            //Buffer current heart rate
    breathingRateVal = s_data.split(",")[2];                                                        //Buffer current breathing rate

    if(startStopHRBR == "true"  && hrbrOnOff == "true")                                             //Saving data to single file and logging enabled
    {
        QFile file(fileData->hrbrFilename);                                                         //Setfilename

        if(file.open(QIODevice::Append  | QIODevice::Text))                                         //Setup File Header
        {
            QTextStream stream(&file);                                                              //Open up a write stream

            stream << posixTime << "," << logData->runNumber << "," << logData->taskCondition << "," << heartRateVal << "," << breathingRateVal << Qt::endl;          //write line
        }

        emit signalHRBRLogging("true");                                                             //Reset if timer expires
    }

    emit setTerminalWindowHRBR(s_data.split(",")[1], s_data.split(",")[2]);
}

ULONGLONG hrhDataInput::makeTime(){

    SYSTEMTIME systemTime;                                                                          // Setup the timestamp
    FILETIME fileTime;                                                                              // The current file time
    ULONGLONG fileTimeNano100, posixTime;                                                           // filetime in 100 nanosecond resolution

    GetSystemTime( &systemTime );
    SystemTimeToFileTime( &systemTime, &fileTime );
    fileTimeNano100 = (((ULONGLONG) fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime;
    posixTime = fileTimeNano100/10000 - 11644473600000;                                             //to milliseconds and unix windows epoche offset removed

    return posixTime;
}
