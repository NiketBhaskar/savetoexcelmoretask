#ifndef LOGGINGDATATOFILE_H
#define LOGGINGDATATOFILE_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include "windows.h"
#include <QVariant>

class filemanagement;
class drivingDataInput;
class hrhDataInput;
class eyeDataInput;
class taskDataInput;
class TTSOTask;
class loggingDataToFile : public QObject
{
    Q_OBJECT

    public:
        explicit loggingDataToFile(QObject *parent = 0);

        ULONGLONG makeTime();                                                   //Function for creating a UNIX timestamp

        filemanagement *fileData;                                               //Pointer to file manager object
        drivingDataInput *draData;                                              //Pointer to driving data object
        hrhDataInput *hrbrData;                                                 //Pointer to heart rate data object
        eyeDataInput *eyeData;                                                  //Pointer to eye data object
        taskDataInput *taskData;                                                //Pointer to task data object
        qint64 pID = 0;
        qint64 rNum = 0;
        QTimer *saveToSingleFile = new QTimer(this);                            //Timer to tell whether to stop logging if in auto log mode
        QTimer *timer = new QTimer(this);                                       //Timer to reset the logging references

        QString runNumber = "1";                                                //Current run number to be recorded in each data file
        QString taskCondition = "0";                                            //Current task number to be recorded in each data file
        QString loggingData = "false";                                          //Parameter that determines whether the system is logging or not

        int signalRefreshTimer = 50;                                            //Single file writing frequency
        int signalRefreshTimerReset = 100;                                      //Single file writing frequency

    signals:
        void signalDraLogging(QVariant status);                                 //Let QML know if data is logging
        void signalEyeLogging(QVariant status);                                 //Let QML know if data is logging
        void signalHRBRLogging(QVariant status);                                //Let QML know if data is logging
        void signalTaskLogging(QVariant status);                                //Let QML know if data is logging

    public slots:
       void updateRunNumber(QString newRunNumber);                              //New run umber value
       void updateTaskCondition(QString newTaskConditon);                       //New task condition
       void updateLoggingData(QString loggingDataUpdate);                       //Update trigger logging of data false = not logging, true = logging
       void initObjects(filemanagement *filemanagement_ptr, eyeDataInput *eyeDataInput_ptr,
                        hrhDataInput *hrhDataInput_ptr, drivingDataInput *drivingDataInput_ptr,
                        taskDataInput *taskDataInput_ptr);
       void dataTriggerCommand();                                               //Timer Trigger
       void writeDataLabel(QString message);                                    //Function to write message to data file
       void onActiveTaskChanged(TTSOTask *task);

   private:
       // ✅ Keep track of currently active task
       TTSOTask *m_currentTask = nullptr;
};

#endif // LOGGINGDATATOFILE_H
