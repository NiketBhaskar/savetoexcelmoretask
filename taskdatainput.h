#ifndef TASKDATAINPUT_H
#define TASKDATAINPUT_H

#include <QObject>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include "windows.h"
#include "udpsocket.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QTimer>
#include "TTSOTask.h"
#include "jercserialcommv10.h"

class loggingDataToFile;
class filemanagement;
class UDPMessagingOutput;
class SWControl;
class TTSOTask;
class taskDataInput: public QObject
{
    Q_OBJECT

    public:
        explicit taskDataInput(QObject *parent = 0);

        loggingDataToFile *logData;                                             //Pointer to file manager object
        filemanagement *fileData;                                               //Pointer to file manager object
        UDPMessagingOutput *updOutputData;                                      //Pointer to output messaging
        SWControl *swControl;
        //QTimer *timerBuffer = new QTimer(this);
        ULONGLONG makeTime();                                                   //Function to create timestamp
        //QString taskFilenameTask;
        //int task_frameNumberByTask = 0;
        //double task_time_buffer_ahead = 0;
        //int taskBufferRefresh = 33;   // ms, same as others
        qint64 pID = 0;
        qint64 rNum = 0;
        QString taskHost = "192.168.101.22";                                    //Modify this to IP-adress of server if not same as client
        QString taskOnOff = "true";                                             //Flag to tell whether task data is to be logged or not.
        QString lastComponent = "0";                                            //Last triggered component
        QString lastEvent = "0";                                                //Buffer data to pass incoming UI event to
        QString indicatorStatus = "0";                                          //Current indicator status - 0=off, 1=Up, 2=Down
        QString steeringWheelString = "";                                       //Message to send to multicast string
        QString messageBuilder = "";                                            //Test string buffer for inco,ing data
        QList<QString> sws_buffer;                                              //QString List for steering wheel button input
        QTimer *timer = new QTimer(this);                                       //Time for polling the Multicast port
        jercserialcommv10 *comSteeringWheel;                                    //Socket for simulator
        UDPSocket *udpTouchScreen;                                              //New socket for STISIM
        int counter = 0;
        int signalRefreshTimer = 50;                                            //Multicast Frequency
        QString enableADASfromSWS = "Off";
        const int taskPort = 6060;                                              // This is the port number you choose for task data

    signals:
        void signalTaskRefresh(QVariant status);                                //Let QML know if data is streaming
        void signalTaskLogging(QVariant status);                                //Let QML know if data is logging
        void setTerminalWindowTask(QVariant device, QVariant event);            //Send events to QML

    public slots:
        //void startWriteTaskBuffFile(QString);
        //void stopWriteTaskBuffFile();
        //void writeTaskBuffFile();
        void recievedDataNewSteeringWheel(QString component, QString event);
        void updateTaskDataOnOff(QString draOnOff);                             //Status update for task input data logging on/off
        void recievedDataTouchscreen(QByteArray taskData);                      //Function to update when touchscreen buttons are pressed
        void recievedDataSteeringWheel(QByteArray taskData);                    //Function to update when steering wheel buttons are pressed
        void receivedDataDRTButton(QByteArray taskData);                        //Data from DRT button
        void updatePortSettingsTask(QString ipaddress,QString port);            //update port settings for task data
        void dataTriggerCommand();                                              //Timer Trigger
        void setADASControlFromSWS(QString state);                              //Set ADAS trigger from SWS
        void initObjects(loggingDataToFile *loggingDataToFile_ptr,
                                filemanagement *filemanagement_ptr,
                                    UDPMessagingOutput *udpmessagingoutput_ptr,
                         SWControl *swControl_ptr);            //Function to setup objects for logging to data file
        void onActiveTaskChanged(TTSOTask *task);

    private:
        // ✅ Keep track of currently active task
        TTSOTask *m_currentTask = nullptr;
};

#endif // TASKDATAINPUT_H
