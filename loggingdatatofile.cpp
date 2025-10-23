#include "loggingdatatofile.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include <direct.h>
#include "windows.h"
#include "filemanagement.h"
#include "taskdatainput.h"
#include "drivingdatainput.h"
#include "hrhdatainput.h"
#include "eyedatainput.h"
#include <QVariant>
#include "TTSOTask.h"
loggingDataToFile::loggingDataToFile(QObject *parent) : QObject(parent)
{
    //DATA INCOMING FROM EYE TRACKER//////////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(dataTriggerCommand()));            //Setup and connect timer to check if signal is refreshed
}

void loggingDataToFile::initObjects(filemanagement *filemanagement_ptr, eyeDataInput *eyeDataInput_ptr, hrhDataInput *hrhDataInput_ptr, drivingDataInput *drivingDataInput_ptr, taskDataInput *taskDataInput_ptr)
{
    fileData = filemanagement_ptr;                                                  //Pointer to file management object
    draData = drivingDataInput_ptr;                                                 //Pointer to driving data object
    eyeData = eyeDataInput_ptr;                                                     //Pointer to eye data object
    hrbrData = hrhDataInput_ptr;                                                    //Pointer to hrbr data object
    taskData = taskDataInput_ptr;                                                   //Pointer totask data object
}

void loggingDataToFile::updateRunNumber(QString newRunNumber)                       //New run umber value
{
    runNumber = newRunNumber;                                                       //Update run number value
}

void loggingDataToFile::updateTaskCondition(QString newTaskCondition)               //New task condition
{
    taskCondition = newTaskCondition;                                               //Update task condition value
}

void loggingDataToFile::dataTriggerCommand()                                        //Timer Trigger
{
    timer->stop();                                                                  //Stop timer and reset

    emit signalDraLogging("false");                                                 //Signal to confirm data is being recorded
    emit signalHRBRLogging("false");                                                //Signal to confirm data is being recorded
    emit signalTaskLogging("false");                                                //Signal to confirm data is being recorded
    emit signalEyeLogging("false");                                                 //Signal to confirm data is being recorded
}

void loggingDataToFile::updateLoggingData(QString loggingDataUpdate)                //Update trigger logging of data false = not logging, true = logging
{
    loggingData = loggingDataUpdate;                                                //Update logging data flag

    if(loggingData == "true")                                                       //If logging data activated, start to log
    {
        timer->start(signalRefreshTimerReset);                                      //Start timer
    }
    else
    {
    }
}
void loggingDataToFile::onActiveTaskChanged(TTSOTask *task)
{
    if (!task) {
        qDebug() << "eyeDataInput: No active task set.";
        m_currentTask = nullptr;
        return;
    }

    m_currentTask = task;

    qDebug() << "eyeDataInput: Active task updated ->"
             << "Participant:" << task->participantID()
             << "Rep:" << task->repNumber()
             << "Order:" << task->taskOrder()
             << "AbsNum:" << task->absoluteTaskNum()
             << "Name:" << task->name()
             << "Complexity:" << task->taskComplexity()
             << "Controls:" << task->controlTypes();
}
void loggingDataToFile::writeDataLabel(QString message)                                 //Function to write data label into text files.
{
    ULONGLONG posixTime = makeTime();                                                   //Create Timestamp

    if(loggingData == "true")                                                           //Trigger data collection if enabled and active
    {
        QFile file1(fileData->drivingFilename);                                         //Set filename parameter

        if(file1.open(QIODevice::Append  | QIODevice::Text))                            //Setup File Header
        {
            QTextStream stream(&file1);                                                 //Open up a write stream

            //stream << "Timestamp"      "run number"        "Task condition"        "message"
            stream << posixTime << "," << runNumber << "," << taskCondition << "," << message << Qt::endl;

        }

        QFile file2(fileData->glanceFilename);                                          //Set filename parameter

        if(file2.open(QIODevice::Append  | QIODevice::Text))                            //Setup File Header
        {
            QTextStream stream(&file2);                                                 //Open up a write stream

            //stream << "Timestamp"      "run number"        "Task condition"        "message"
            stream << posixTime << "," << runNumber << "," << taskCondition << "," << message << Qt::endl;
        }

        QFile file3(fileData->hrbrFilename);                                            //Set filename parameter

        if(file3.open(QIODevice::Append  | QIODevice::Text))                            //Setup File Header
        {
            QTextStream stream(&file3);                                                 //Open up a write stream

            //stream << "Timestamp"      "run number"        "Task condition"        "message"
            stream << posixTime << "," << runNumber << "," << taskCondition << "," << message << Qt::endl;
        }

        QFile file4(fileData->taskFilename);                                            //Set filename parameter

        if(file4.open(QIODevice::Append  | QIODevice::Text))                            //Setup File Header
        {
            QTextStream stream(&file4);                                                 //Open up a write stream

            if (m_currentTask) {
                pID = m_currentTask->participantID();
                rNum = m_currentTask->repNumber();
            stream << posixTime << ","
                                  << m_currentTask->participantID() << ","
                                  << m_currentTask->absoluteTaskNum() << ","
                                  << m_currentTask->taskOrder() << "," << m_currentTask->repNumber() << "," << m_currentTask->taskComplexity() << "," <<  m_currentTask->controlTypes()<< ","<< "Message" <<"," << message << Qt::endl; //write line
            }
            else{
                stream << posixTime << ","
                                      << pID << ","
                                      << "-1" << ","
                                      << "-1" << "," << rNum << "," << "-1" << "," <<  "-1" << ","<< "Message" <<"," << message << Qt::endl; //write line
            }
        }
    }
}


ULONGLONG loggingDataToFile::makeTime()
{
    SYSTEMTIME systemTime;                                                              // Setup the timestamp
    FILETIME fileTime;                                                                  // The current file time
    ULONGLONG fileTimeNano100, posixTime;                                               // filetime in 100 nanosecond resolution

    GetSystemTime( &systemTime );
    SystemTimeToFileTime( &systemTime, &fileTime );
    fileTimeNano100 = (((ULONGLONG) fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime;
    posixTime = fileTimeNano100/10000 - 11644473600000;                                 //to milliseconds and unix windows epoche offset removed

    return posixTime;
}
