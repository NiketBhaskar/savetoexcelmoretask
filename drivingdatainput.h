#ifndef DRIVINGDATAINPUT_H
#define DRIVINGDATAINPUT_H

#include <QObject>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include "windows.h"
#include "udpsocket.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QTimer>
#include "OBSController.h"
#include "jercserialcommv10.h"
#include <QVariant>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
class loggingDataToFile;
class filemanagement;
class adasControl;
class TTSOTask;
class eyeDataInput;
class drivingDataInput: public QObject
{
    Q_OBJECT

    public:
        explicit drivingDataInput(QObject *parent = 0);
        int DRA_frameNumberByTask = 0;
        QString DRAFilenameTask;
        loggingDataToFile *logData;                                             //Pointer to file manager object
        filemanagement *fileData;                                               //Pointer to file manager object
        adasControl *adasData;                                                  //Pointer to adas data object
        OBSController *m_obsController = nullptr;

        qint64 pID = 0;
        qint64 rNum = 0;
        ULONGLONG makeTime();                                                   //Function to create timestamp
        ULONGLONG taskStartTime = 0;
        ULONGLONG taskEndTime   = 0;

        QTimer *timer = new QTimer(this);                                       //Time for polling the Multicast port
        QTimer *timerBuffer = new QTimer(this);
        QByteArray lastDrivingPacket;
        jercserialcommv10 *sim_data_receiver;                                   //Socket for simulator
        UDPSocket *DRA_udpsocket;                                               //New socket for STISIM
        QTcpServer *m_tcpserver_lct;                                            //Socket for LCT
        QHash<QTcpSocket*, QByteArray*> buffers;                                //We need a buffer to store data until block has completely received
        QHash<QTcpSocket*, qint32*> sizes;                                      //We need to store the size to verify if a block has received completely
        float DRA_time_buffer_ahead = 0;
        QByteArray sim_data_buffer;                                             //Data array for message buffer

        QString DrivingOnOff = "true";                                          //Is driving data being logged or not
        QString draHost = "192.168.101.22";                                     //Modify this to IP-adress of server if not same as client
        //QString draHost = "127.0.0.1";
        QString draOnOff = "true";                                              //Status signal for whether driving data needs to be recorded.
        QString logOnSimData = "false";                                         //Trigger to log based upon incoming sim data
        QString simBrandFlag = "0";                                             //Setting for simulator softwar brand 0=STISIM, 1=LCT
        QString indicatorStatus = "0";                                          //Current Indicator Setting
        QString simTime = "";                                                   //STISIMTime
        QString LanePos = "";                                                   //LanePos
        QString strWhl = "";                                                    //StrWhl10
        QString crashes = "";                                                   //Crashes
        QString speed = "";                                                     //Speed
        QString throttle = "";                                                  //Throttle
        QString brake = "";                                                     //Brake
        QString distance = "";                                                  //Distance
        QString nearestcar = "";                                                //Nearest car
        QString lbs = "";                                                       //Left blind spot
        QString rbs = "";                                                       //Right blind spot
        QString vehicleHeadingAngle = "";
        QString worldposX = "";
        QString worldposY = "";
        QString worldposZ = "";
        QString worldVehicleHeadingAngle = "";

        int draPort = 7070;                                                     //This is the port number you choose in the simulator
        int signalRefreshTimer = 100;                                           //Multicast Frequency
        int leftBlindSpot = 0;                                                  //Left blind spot
        int rightBlindSpot = 0;                                                 //Right blind spot
        int SD_LP_Counter = 0;                                                  //SD Counter
        int SD_S_Counter = 0;                                                   //SD Counter

        float param[200];                                                       //Data Buffer for parameters
        QStringList paramName[200];                                             //Data Buffer for parameters
        float vehicle_param[50][3];                                             //Vehicle data
        float closest_vehicle_distance = 100000;                                //Closest Vehicle Param
        float SD_LP = 0;                                                        //Calculate Standard Deviation of Lane Position
        float SD_LP_Buffer = 0;                                                 //Buffer
        float SD_S = 0;                                                         //Calculate Standard Deviation of Speed
        float SD_S_Buffer = 0;                                                  //Buffer
        float lp_mean, lp_sd, lp_var, lp_dev, lp_sdev = 0.0;                    //Lane Position Params
        float s_mean, s_sd, s_var, s_dev, s_sdev = 0.0;                         //Speed Params
        float SDLP(float LanePosition);                                         //Calculate SD of Lane Position
        float SDS(float Speed);                                                 //Calculate SD of Speed
        float DRABufferRefresh = 33.33333333;

    signals:
        void signalDraRefresh(QVariant status);                                 //Let QML know if data is streaming
        void signalDraLogging(QVariant status);                                 //Let QML know if data is logging
        void setLoggingOnSimData(QVariant status);                              //Trigger when logging
        void setTerminalWindowDriving(QVariant p1,QVariant p2,QVariant p3,
                                      QVariant p4,QVariant p5,QVariant p6,
                                      QVariant p7,QVariant p8,QVariant p9,
                                      QVariant p10,QVariant p11,QVariant p12,
                                      QVariant p13,QVariant p14,QVariant p15);

    public slots:
        void computeStatsFromDRA(const QString &draFile,
                                                   double &meanHeadway,
                                                   double &sdLanePos,
                                                   double &meanSpeed);
        void startWriteDRABuffFile(QString fileNum);                            //Start stop write data to file
        void stopWriteDRABuffFile();
        void writeDRABuffFile();
        void updateDrivingDataOnOff(QString draOnOff);                           //Status update for driving data logging on/off
        void recievedDataCOM(QByteArray drivingDataCOM);                         //Update function for incoming data from Hear Rate Sensor
        void recievedDataTCP();                                                  //Update function for incoming data from Hear Rate Sensor
        void updatePortSettingsDra(QString ipaddress,QString port);              //update port settings for simulator
        void dataTriggerCommand();                                               //Timer Trigger
        void updateLogOnSimDataOnOff(QString logOnOff);                          //UPdate log on SimData
        void initObjects(loggingDataToFile *loggingDataToFile_ptr,
                                filemanagement *filemanagement_ptr,
                                adasControl *adasData_ptr,
                         eyeDataInput *eyeData_ptr);                      //Function to setup objects for logging to data file
        void resetSDVals();
        QString setParamName(QString data);                                      //Function to setup driving sim parameter names
        void sendSTISIMTrigger();                                                //Send Signal to trigger STISIM
        // ✅ New slot to receive active task info from TTSOTaskListModel
        void onActiveTaskChanged(TTSOTask *task);
//        void startDemoLogging();
//            void stopDemoLogging();
//            void startExpLogging();
//            void stopExpLogging();
    private:
        // ✅ Keep track of currently active task
        TTSOTask *m_currentTask = nullptr;
        eyeDataInput *eyeData = nullptr;
        QWebSocketServer *m_server = nullptr;
        QList<QWebSocket*> m_clients;
        void sendTaskStarted();
//        void startCustomLogging(const QString &label);
//            void stopCustomLogging();
//            bool m_customMode = false;
//            QString m_customLabel; // "demo" or "exp"
    private slots:
        void onNewConnection();
        void onTextMessageReceived(QString message);

};

#endif // DRIVINGDATAINPUT_H
