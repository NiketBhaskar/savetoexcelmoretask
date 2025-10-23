#ifndef HRHDATAINPUT_H
#define HRHDATAINPUT_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "windows.h"
#include "udpsocket.h"
#include <QVariant>

class loggingDataToFile;
class filemanagement;

class hrhDataInput: public QObject
{
    Q_OBJECT

    public:
        explicit hrhDataInput(QObject *parent = 0);

        loggingDataToFile *logData;                                             //Pointer to file manager object
        filemanagement *fileData;                                               //Pointer to file manager object

        UDPSocket *hrbr_udpsocket;                                              //UDP SOcket to connect to HRBR

        QTimer *timer = new QTimer(this);                                       //Time for polling the Multicast port

        ULONGLONG makeTime();                                                   //Function to create timestamp

        QString hrbrOnOff = "false";                                            //Status signal for whether HRBR needs to be recorded.
        QString startStopHRBR = "false";                                        //Start or stop the recording of data
        QString hrbrHost = "192.168.101.22";                                    //Modify this to IP-adress of server if not same as client
        QString s_data;                                                         //Buffer for heart rate dat
        QString heartRateVal = "0";                                             //Current Heart Rate
        QString breathingRateVal = "0";                                         //Current breathing rate

        int signalRefreshTimer = 100;                                           //Refresh time for the timer

        const int hrbrPort = 5050;                                              //This is the port number you choose in Smart Eye Pro

    signals:
        void signalHRBRRefresh(QVariant status);                                //Let QML know if data is streaming
        void signalHRBRLogging(QVariant status);                                //Let QML know if data is logging
        void setTerminalWindowHRBR(QVariant heartRate, QVariant breathingRate); //Current Data being sent by Heart Rate Monitor

    public slots:
        void updateHRBROnOff(QString hrbrOnOff);                                //Status update for heart rate breathing rate
        void recievedData(QByteArray heartRate);                                //Update function for incoming data from Hear Rate Sensor
        void updatePortSettingsHRBR(QString ipaddress,QString port);            //update port settings for HRBR data
        void dataTriggerCommand();                                              //Timer Trigger
        void updateStartStopHRBR (const QString startstopflag);                 //Function for updating start to log flag
        void initObjects(loggingDataToFile *loggingDataToFile_ptr,
                                filemanagement *filemanagement_ptr);            //Function to setup objects for logging to data file

};

#endif // HRHDATAINPUT_H
