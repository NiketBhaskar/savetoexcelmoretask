#ifndef EYEDATAINPUT_OLD_H
#define EYEDATAINPUT_OLD_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "windows.h"
#include "udpsocket.h"
#include <QVariant>

class loggingDataToFile;
class filemanagement;

class eyeDataInput: public QObject
{
    Q_OBJECT

    public:
        explicit eyeDataInput(QObject *parent = 0);

        loggingDataToFile *logData;                                             //Pointer to file manager object
        filemanagement *fileData;                                               //Pointer to file manager object

        QTimer *timer = new QTimer(this);                                       //Time for polling the Multicast port

        ULONGLONG makeTime();                                                   //Function to create timestamp
        ULONGLONG posixTimePreviousGlance = 0;                                  //Length of previous glance time
        ULONGLONG ShortGlanceThreshold = 175;                                   //Threshold to eliminate short glances
        ULONGLONG posixTimePrevious = 0;                                        //Previous glance time stamp
        ULONGLONG GlanceTimer = 0;                                              //Glance time for Attention Buffer calculations

        UDPSocket *m_udpsocket_Eye;                                             //Receiver for Eyetracker Data - SmartEye

        QString eyeHost = "192.168.101.22";                                     //Modify this to IP-adress of server if not same as client
        QString GlanceLocation = "X";                                           //Current glance location buffer
        QString Blink = "0";                                                    //Flag for blink events
        QString Horiz_Eye_Poz = "0";                                            //Current Horizontal Eye Position parameter
        QString PreviousGlanceLocation = "0";                                   //Previous glance location for comaprison
        QString FilteredGlanceLocation = "0";                                   //Glance location filtered to eliminate short glances
        QString eyeOnOff = "true";                                              //Status signal for whether Eye glance data needs to be recorded.

        float eyetrackerBrand = 0;                                              //Current Eye tracker brand (0=Smart Eye, 1=SeeingMachines)
        float blinkThreshold = 0.98;                                            //For SMartEye system, blink threshold parameter
        float eyequality = 0;                                                   //Parameter to track the current eye quality data
        float EyeQualityCheck(ULONGLONG time);                                  //Function to check current quality of eye glance data
        float Q_B[9];                                                           //Data for the eye quality check
        ULONGLONG mSecsStart = 0;                                                   //Milliseconds from start of run

        double KA_Buffer_Val_float = 0;                                         //Current Value of the Kircher Ahlstrom Buffer
        double AHEADV1_Buffer_Val_float = 0;                                    //Current Value of the AHEAD V1 Buffer
        double CogV0_Buffer_Val_float = 0;                                      //Current Value of the Cog V0 Buffer

        int signalRefreshTimer = 100;                                           //Multicast Frequency
        int GlanceCounter = 0;                                                  //Current Glance Counter
        int frameNumber = 0;                                                    //Count of frame

        const int eyePort = 8080;                                               //This is the port number you choose in Smart Eye Pro

        void AttentionBuffer(QString GlanceLoc2, ULONGLONG time);               //Function to calculate attention buffer parameters

    signals:
        void signalEyeRefresh(QVariant status);                                 //Let QML know if data is streaming
        void signalEyeLogging(QVariant status);                                 //Let QML know if data is logging
        void setTerminalWindowGlance(QVariant param1, QVariant param2,
                                     QVariant param3, QVariant param4,
                                     QVariant param5, QVariant param6,
                                     QVariant param7, QVariant param8);         //Signal to synchronise glance data with QML



    public slots:
        void updateEyeTrackingDataOnOff(QString draOnOff);                          //Status update for driving data logging on/off
        void recievedData(QByteArray eyeData);                                  //Update function for incoming data from Hear Rate Sensor
        void updatePortSettingsEye(QString ipaddress,QString port);             //update port settings for eye tracker
        void dataTriggerCommand();                                              //Timer Trigger
        void initObjects(loggingDataToFile *loggingDataToFile_ptr,
                                filemanagement *filemanagement_ptr);            //Function to setup objects for logging to data file

};

#endif // EYEDATAINPUT_OLD_H
