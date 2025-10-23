#include "drivingdatainput.h"
#include <QFile>
#include <QTextStream>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include "eyedatainput.h"
#include "windows.h"
#include "loggingdatatofile.h"
#include "filemanagement.h"
#include "adascontrol.h"
#include <QDir>
#include <QVariant>
#include "OBSController.h"
#include "TTSOTask.h"
#include <QRegularExpression>
drivingDataInput::drivingDataInput(QObject *parent) : QObject(parent)
{
    //COM PORT FOR DATA FROM SIMULATOR - STISIM///////////////////////////////////////////////////////////
    m_obsController = new OBSController(this);
    m_server = new QWebSocketServer(QStringLiteral("QtTaskServer"),
                                        QWebSocketServer::NonSecureMode,
                                        this);
        if (m_server->listen(QHostAddress::Any, 22345)) {
            qDebug() << "✅ WebSocket server listening on port 22345";
            connect(m_server, &QWebSocketServer::newConnection,
                    this, &drivingDataInput::onNewConnection);
        } else {
            qWarning() << "❌ Failed to start WebSocket server";
        }
    // Connect to OBS WebSocket (change IP/port/password to your OBS settings)
    m_obsController->connectToOBS(QUrl("ws://127.0.0.1:4444"));
    sim_data_buffer.reserve(255);
    sim_data_receiver = new jercserialcommv10();
    sim_data_receiver->setCOMport(false,"COM5");                                                        //MAKE COM PORT SELECTABLE
    sim_data_receiver->openPort(QSerialPort::Baud115200,QSerialPort::Data8,QSerialPort::NoParity,QSerialPort::OneStop,QSerialPort::HardwareControl);
    connect(sim_data_receiver,SIGNAL(receivedDataAsByte(QByteArray)),this,SLOT(recievedDataCOM(QByteArray)));

    //UDP PORT VERSION OF DATA FROM SIMULATOR - STISIM////////////////////////////////////////////////////
    DRA_udpsocket = new UDPSocket(this);                                                                //Port for Driving Data
    DRA_udpsocket->ConnectSingleAltAddress(draPort, draHost);                                           //Host and port for simulator
    connect(DRA_udpsocket,SIGNAL(messageReceived(QByteArray)),this,SLOT(recievedDataCOM(QByteArray)));

    //TCPIP PORT FOR DATA FROM SIMULATOR - LCT////////////////////////////////////////////////////////////
    //[Logging] TCP/IP-Logging IP=127.0.0.1 / Port=4955                                                 //Data from Simulator
    m_tcpserver_lct = new QTcpServer(this);
    connect(m_tcpserver_lct, SIGNAL(newConnection()), SLOT(recievedDataTCP()));

    //DATA INCOMING FROM DRIVING SIMULATOR////////////////////////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(dataTriggerCommand()));                                //Setup and connect timer to check if signal is refreshed
    connect(timerBuffer, SIGNAL(timeout()), this, SLOT(writeDRABuffFile()));

}
//static inline QString labelToBaselineName(const QString& label) {
//    if (label.compare("demo", Qt::CaseInsensitive) == 0) return "Baseline Drive One";
//    if (label.compare("exp",  Qt::CaseInsensitive) == 0) return "Baseline Drive Two";
//    return "Baseline"; // fallback
//}
static inline QString buildSimpleTaskName(TTSOTask *task) {
    if (!task) return "UnknownTask";

    int absNum = task->absoluteTaskNum();
    int rep = task->repNumber();
    int pID = task->participantID();

    // Baseline drive (absNum = 0, rep = 0)
    if (absNum == 0 && rep == 0) {
        return QString("P%1_Baseline").arg(pID);
    }

    // Experienced drive (absNum = -1, rep = 11)
    if (absNum == -1 && rep == 11) {
        return QString("P%1_Experienced").arg(pID);
    }

    // Regular tasks (absNum = 1-23, rep = 1-10)
    return QString("P%1_T%2_R%3")
            .arg(pID)
            .arg(absNum)
            .arg(rep);
}
static inline QString sanitize(const QString &s) {
    QString out = s.trimmed();
    out.replace(QRegularExpression("[\\s]+"), "_");
    out.remove(QRegularExpression("[^A-Za-z0-9_\\-]"));
    return out.toLower();
}
void drivingDataInput::initObjects(loggingDataToFile *loggingDataToFile_ptr, filemanagement *filemanagement_ptr, adasControl *adasData_ptr,
                                   eyeDataInput *eyeData_ptr)
{
    logData = loggingDataToFile_ptr;                                            //Pointers to logging data object

    fileData = filemanagement_ptr;                                              //Pointer to file management object

    adasData = adasData_ptr;                                                    //Pointer to adas data object
    eyeData = eyeData_ptr;   // NEW: save pointer to Eye input
}
void drivingDataInput::onNewConnection() {
    QWebSocket *client = m_server->nextPendingConnection();
    connect(client, &QWebSocket::textMessageReceived,
            this, &drivingDataInput::onTextMessageReceived);
    connect(client, &QWebSocket::disconnected, client, &QObject::deleteLater);
    m_clients << client;
    qDebug() << "Client connected from" << client->peerAddress().toString();
}
void drivingDataInput::sendTaskStarted() {
    if (!m_currentTask) return;
    QJsonObject obj;
    obj["event"] = "taskStarted";
    obj["participantId"] = m_currentTask->participantID();
    obj["absTaskNum"] = m_currentTask->absoluteTaskNum();
    obj["taskOrder"] = m_currentTask->taskOrder();
    obj["repNum"] = m_currentTask->repNumber();
    obj["taskName"] = m_currentTask->name();
    obj["x"] = 0;
    obj["y"] = 0;
    obj["z"] = 0;

    QJsonDocument doc(obj);
    QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    for (QWebSocket *c : m_clients) {
        c->sendTextMessage(msg);
    }
    qDebug() << "📤 Sent to Android:" << msg;
}
void drivingDataInput::onTextMessageReceived(QString message) {
    qDebug() << "📩 Message from Android:" << message;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) return;

    QJsonObject obj = doc.object();
    QString event = obj["event"].toString();

    if (event == "taskEnded") {
        int absTaskNum = obj["absTaskNum"].toInt();
        int repNum = obj["repNum"].toInt();
        qDebug() << "Android signaled task end:" << absTaskNum << repNum;
        stopWriteDRABuffFile();   // directly trigger stop here
    }
}
void drivingDataInput::updateDrivingDataOnOff (QString draOnOff)
{
    DrivingOnOff = draOnOff;                                                    //Driving data to be logged
}

void drivingDataInput::updateLogOnSimDataOnOff (QString logOnOff)
{
    logOnSimData = logOnOff;                                                    //Driving data to be logged when simulator starts
}

void drivingDataInput::updatePortSettingsDra(QString ipaddress, QString port)
{
    int port_val = port.toInt();                                                //Convert port val to int

    if(port_val != draPort && ipaddress != draHost)                             //Check if port is new
    {
        draPort = port_val;                                                     //update values

        draHost = ipaddress;                                                    //address for host

        DRA_udpsocket->ConnectSingleAltAddress(port_val, ipaddress);            //Reset Port Settings - Update when switch to UDP
    }
}

void drivingDataInput::dataTriggerCommand()                                     //Timer Trigger
{
    timer->stop();                                                              //Stop timer and reset

    emit signalDraRefresh("false");                                             //Set interface red

    logData->loggingData = "false";                                             //Stop logging function

    emit setLoggingOnSimData(false);                                            //Enable buttons

    emit signalDraLogging("false");                                             //Reset if timer expires
}

void drivingDataInput::recievedDataTCP()
{

}

void drivingDataInput::sendSTISIMTrigger()
{
    DRA_udpsocket->sendCommand("Go!","192.168.101.21",1234);
}
//void drivingDataInput::startCustomLogging(const QString &label) {
//    m_customMode = true;
//    m_customLabel = label; // "demo" or "exp"
//    // ✅ Ensure writes are allowed
//      logData->loggingData = "true";
//      DrivingOnOff = "true";
//    taskStartTime = makeTime();

//    QDir dir(fileData->fullfolderpath + "/DRATASKDATA");
//    if (!dir.exists()) dir.mkpath(".");

//    QString base = sanitize(label); // "demo" or "exp"
//    DRAFilenameTask = dir.filePath(base + "_DRA.csv");

//    if (m_obsController) m_obsController->startRecording(base);

//    QFile f(DRAFilenameTask);
//    if (f.open(QIODevice::Append | QIODevice::Text)) {
//        QTextStream s(&f);
//        s << "Unix TimeStamp,Participant ID,RunType,AbsTaskNum,TaskOrder,RepNumber,ElapsedBufferTime(s),"
//             "TaskComplexity,Control Types,SimTime,LanePos,StrWhl,Crashes,Speed,Throttle,Brake,"
//             "DistanceTravelled,DistToVehAhead"
//          << Qt::endl;
//    }

//    DRA_frameNumberByTask = 0;
//    DRA_time_buffer_ahead = 0;
//    timerBuffer->start(DRABufferRefresh);
//}

//void drivingDataInput::stopCustomLogging() {
//    timerBuffer->stop();
//        taskEndTime = makeTime();

//        // ---- NEW: compute stats from the just-recorded custom run ----
//        double meanHeadway = -1, sdLanePos = -1, meanSpeed = -1;
//        if (!DRAFilenameTask.isEmpty()) {
//            computeStatsFromDRA(DRAFilenameTask, meanHeadway, sdLanePos, meanSpeed);
//        }

//        // Get attentional demand mean from eye tracker, if available (mirrors your task summary flow)
//        double meanLiveAttenD = 0.0;
//        if (eyeData) {
//            meanLiveAttenD = eyeData->getMeanLiveAttenD();
//            eyeData->resetMeanLiveAttenD();
//        }

//        // Prepare the summary row
//        const double taskDurationSec = (taskEndTime - taskStartTime) / 1000.0; // ms -> s
//        const QString baselineName = labelToBaselineName(m_customLabel);

//        // Append into SUMMARY.csv (create header if needed)
//        const QString summaryPath = fileData->fullfolderpath + "/" + fileData->filename + "_SUMMARY.csv";
//        QFile summaryFile(summaryPath);
//        if (summaryFile.open(QIODevice::Append | QIODevice::Text)) {
//            QTextStream out(&summaryFile);
//            if (summaryFile.size() == 0) {
//                out << "UnixTimestamp,ParticipantID,AbsTaskNum,TaskOrder,TaskName,RepNumber,"
//                       "TaskComplexity,ControlTypes,TaskDuration(Sec),TaskStartTime,TaskEndTime,NumberOfErrors,"
//                       "MeanAttenD,MeanHeadway,SDOLanePos,MeanSpeed"
//                    << Qt::endl;
//            }

//            // For baselines, we don’t have a TTSO task active: use zeros / tags as you prefer
//            const QString participantId = "baseLine";          // or fileData->filename if you prefer the file code
//            const QString taskComplexity = "baseline";
//            const QString controlTypes   = "baseline";

//            out << taskStartTime << ","
//                << participantId << ","
//                << "0" << ","               // AbsTaskNum
//                << "0" << ","               // TaskOrder
//                << baselineName << ","      // TaskName
//                << "0" << ","               // RepNumber
//                << taskComplexity << ","
//                << controlTypes << ","
//                << taskDurationSec  << ","
//                << taskStartTime     << ","
//                << taskEndTime       << ","
//                << ""                << "," // NumberOfErrors (unknown/NA)
//                << meanLiveAttenD    << ","
//                << meanHeadway       << ","
//                << sdLanePos         << ","
//                << meanSpeed
//                << Qt::endl;
//        }


//    // (optional) write summary row for custom runs if you want
//    if (m_obsController) m_obsController->stopRecordingDelayed(2000);

//    DRA_time_buffer_ahead = 0;
//    DRA_frameNumberByTask = 0;
//    logData->loggingData = "false";
//       DrivingOnOff = "false";
//       m_customMode = false;
//       m_customLabel.clear();
//}
//void drivingDataInput::startDemoLogging() { startCustomLogging("demo"); }
//void drivingDataInput::stopDemoLogging()  { stopCustomLogging(); }
//void drivingDataInput::startExpLogging()  { startCustomLogging("exp"); }
//void drivingDataInput::stopExpLogging()   { stopCustomLogging(); }

void drivingDataInput::onActiveTaskChanged(TTSOTask *task)
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
void drivingDataInput::startWriteDRABuffFile(QString fileNum)
{
    logData->taskCondition = fileNum;

     ULONGLONG posixTime = makeTime();
     taskStartTime = makeTime();   // Unix ms timestamp
     QDir dir(fileData->fullfolderpath + "/DRATASKDATA");
     if (!dir.exists()) {
         dir.mkpath(".");
     }
     QString baseName;
     if (m_currentTask)
         baseName = buildSimpleTaskName(m_currentTask);
     else
         baseName = "Baseline";
     DRAFilenameTask = dir.filePath(baseName + "_DRA.csv");
     if (m_obsController) {
         m_obsController->startRecording(baseName);
     }
    QFile fileNew(DRAFilenameTask);

    if (fileNew.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&fileNew);
        stream << "Participant ID: " << fileData->filename << ",Start Time: " << posixTime << Qt::endl;

        stream << "Unix TimeStamp" << ",Participant ID" << ",TaskName" << ",AbsTaskNum" << ",TaskOrder" << ",RepNumber" << ",ElapsedBufferTime(s)"
               << ",TaskComplexity" << ",Control Types" << ",SimTime"  << ",LanePos" << ",StrWhl" << ",Crashes"
               << ",Speed" << ",Throttle" << ",Brake" << ",DistanceTravelled"
               << ",DistToVehAhead"
               << Qt::endl;
    }

    DRA_frameNumberByTask = 0;
    DRA_time_buffer_ahead = 0;

    timerBuffer->start(DRABufferRefresh);

    qDebug() << "[DRA] Buffer logging started, file:" << DRAFilenameTask;
}


void drivingDataInput::stopWriteDRABuffFile()
{
    timerBuffer->stop();
    taskEndTime = makeTime();
    double meanLiveAttenD = 0.0;
    if (eyeData) {
        meanLiveAttenD = eyeData->getMeanLiveAttenD();
        eyeData->resetMeanLiveAttenD();  // ready for next task
    }
    double taskDurationSec = (taskEndTime - taskStartTime) / 1000.0;  // ms → seconds
    DRA_time_buffer_ahead = 0;
    DRA_frameNumberByTask = 0;
    double meanHeadway = -1, sdLanePos = -1, meanSpeed = -1;
    computeStatsFromDRA(DRAFilenameTask, meanHeadway, sdLanePos, meanSpeed);
    // ✅ Append into SUMMARY.csv
        QString summaryPath = fileData->fullfolderpath + "/" + fileData->filename + "_SUMMARY.csv";
        QFile summaryFile(summaryPath);
        if (summaryFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&summaryFile);
            if (summaryFile.size() == 0) {
                out << "UnixTimestamp,ParticipantID,AbsTaskNum,TaskOrder,TaskName,RepNumber,"
                    << "TaskComplexity,ControlTypes,TaskDuration(Sec),TaskStartTime,TaskEndTime,NumberOfErrors,"
                    << "MeanAttenD,MeanHeadway,SDOLanePos,MeanSpeed"
                    << Qt::endl;
            }
            if (m_currentTask) {
                out << taskStartTime << ","
                    << m_currentTask->participantID() << ","
                    << m_currentTask->absoluteTaskNum() << ","
                    << m_currentTask->taskOrder() << ","
                    << m_currentTask->name() << ","
                    << m_currentTask->repNumber() << ","
                    << m_currentTask->taskComplexity() << ","
                    << m_currentTask->controlTypes() << ","
                    << taskDurationSec  << ","
                    << taskStartTime  << ","
                    << taskEndTime  << ","
                    << "NeedToBeDefined" << ","   // errors
                    << meanLiveAttenD << ","   // attentional demand
                    << meanHeadway << ","
                    << sdLanePos << ","
                    << meanSpeed
                    << Qt::endl;
            }
        }

        taskStartTime = 0;
        taskEndTime   = 0;
        if (m_obsController) {

                       m_obsController->stopRecordingDelayed(2000);
        }
    qDebug() << "[DRA] Buffer logging stopped.";
}
void drivingDataInput::computeStatsFromDRA(const QString &draFile,
                                           double &meanHeadway,
                                           double &sdLanePos,
                                           double &meanSpeed)
{
    QFile file(draFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    QString header = in.readLine(); // skip first line(s)
    if (header.contains("Participant ID")) header = in.readLine(); // skip metadata line

    QList<double> headways, lanePos, speeds;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList cols = line.split(",");
        if (cols.size() < 16) continue;
        headways.append(cols[17].toDouble());
        lanePos.append(cols[10].toDouble());
        speeds.append(cols[13].toDouble());
    }

    auto mean = [](const QList<double> &vals) {
        if (vals.isEmpty()) return -1.0;
        double sum = 0;
        for (double v : vals) sum += v;
        return sum / vals.size();
    };
    auto stdev = [](const QList<double> &vals) {
        if (vals.size() < 2) return -1.0;
        double mean = 0;
        for (double v : vals) mean += v;
        mean /= vals.size();
        double sumSq = 0;
        for (double v : vals) sumSq += (v - mean)*(v - mean);
        return sqrt(sumSq / (vals.size()-1));
    };

    meanHeadway = mean(headways);
    sdLanePos = stdev(lanePos);
    meanSpeed = mean(speeds);
}

void drivingDataInput::writeDRABuffFile()
{
    if (lastDrivingPacket.isEmpty())
        return;

    if (logData->loggingData != "true" || DrivingOnOff != "true")
        return;

    ULONGLONG posixTime = makeTime();

    QString buffer = lastDrivingPacket;
    QStringList params = buffer.split(",");
    QStringList paramNames;
    QStringList paramData;

    for (int a = 0; a < params.length(); a++) {
        QStringList data = params[a].split(":");
        if (data.size() >= 2) {
            paramNames << setParamName(data[0]);
            paramData << data[1];
        }
    }

    if (!paramNames.isEmpty()) {
        simTime   = paramData[paramNames.indexOf("Elapsed time for current run")];
        LanePos   = paramData[paramNames.indexOf("Drivers lat'l lane pos'n")];
        strWhl    = paramData[paramNames.indexOf("Steering wheel angle input")];
        crashes   = paramData[paramNames.indexOf("Compilation of crashes")];
        speed     = paramData[paramNames.indexOf("Drivers long'l vel'y")];
        throttle  = paramData[paramNames.indexOf("Gas pedal input counts")];
        brake     = paramData[paramNames.indexOf("Brake pedal input counts")];
        distance  = paramData[paramNames.indexOf("Total long'l dist")];
        nearestcar= paramData[paramNames.indexOf("Closest Vehicle Distance")];
    }

    QFile file(DRAFilenameTask);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        if (nearestcar == "100000") nearestcar = "0";

        if (m_currentTask) {
            // Normal task logging
            pID = m_currentTask->participantID();
            rNum = m_currentTask->repNumber();
            stream << posixTime << ","
                   << m_currentTask->participantID() << ","
                   << m_currentTask->name() << ","
                   << m_currentTask->absoluteTaskNum() << ","
                   << m_currentTask->taskOrder() << ","
                   << m_currentTask->repNumber() << ","
                   << QString::number(DRA_time_buffer_ahead / 1000.0, 'f', 3) << ","
                   << m_currentTask->taskComplexity() << ","
                   << m_currentTask->controlTypes() << ",";
            stream << simTime << "," << LanePos << "," << strWhl << "," << crashes << ","
                   << speed << "," << throttle << "," << brake << "," << distance << ","
                   << nearestcar
                   << Qt::endl;
        } /*else if (m_customMode) {
            QString runType = m_customLabel; // "demo" or "exp"
            stream << posixTime << ","
                   << pID << ","      // if you have it; otherwise leave blank or put session id
                   << runType << ","  // RunType
                   << "0" << ","      // AbsTaskNum
                   << "0" << ","      // TaskOrder
                   << "0" << ","      // Rep
                   << QString::number(DRA_time_buffer_ahead / 1000.0, 'f', 3) << ","
                   << runType << "," << runType << ",";
            stream << simTime << "," << LanePos << "," << strWhl << "," << crashes << ","
                   << speed << "," << throttle << "," << brake << "," << distance << ","
                   << nearestcar
                   << Qt::endl;
        }*/ else {
            // Baseline or End Capture logging
//            QString taskName = "Baseline";
//            if (DRAFilenameTask.contains("EndCapture")) {
//                taskName = "End Capture";
//            }

//            stream << posixTime << ","
//                   << pID << ","
//                   << taskName << ","
//                   << "0" << ","
//                   << "0" << ","
//                   << "0" << ","
//                   << QString::number(DRA_time_buffer_ahead / 1000.0, 'f', 3) << ","
//                   << taskName << ","
//                   << taskName << ",";
//            stream << simTime << "," << LanePos << "," << strWhl << "," << crashes << ","
//                   << speed << "," << throttle << "," << brake << "," << distance << ","
//                   << nearestcar
//                   << Qt::endl;
            return; // do nothing
        }
    }

    DRA_frameNumberByTask++;
    DRA_time_buffer_ahead += 33.333;
}

void drivingDataInput::recievedDataCOM(QByteArray drivingDataCOM)
{
    lastDrivingPacket = drivingDataCOM;
    timer->stop();                                                              //Stop timer and reset

    timer->start(signalRefreshTimer);                                           //Start timer

    emit signalDraRefresh("true");                                              //Set interface green

    ULONGLONG posixTime = makeTime();                                           //Create Timestamp

    timer->stop();                                                              //Stop the watchdog timer

    if(logOnSimData == "true")                                                  //If Logging ON, start logging when data is recieved
    {
        logData->loggingData = "true";                                          //Start logging function

        emit setLoggingOnSimData(true);                                         //Disable buttons
    }

    timer->start(1000);                                                         //Timer start

    QString buffer = drivingDataCOM;
    QStringList params = buffer.split(",");
    QStringList paramNames;
    QStringList paramData;

    for(int a = 0; a < params.length(); a++)
    {
        QStringList data = params[a].split(":");
        QString data_1 = data[0];
        QString data_2 = data[1];

        paramNames << setParamName(data_1);
        paramData << data_2;
    }

    QFile file(fileData->drivingFilename);                                                  //Set filename parameter

    simTime = paramData[paramNames.indexOf("Elapsed time for current run")];                //STISIMTime
    LanePos = paramData[paramNames.indexOf("Drivers lat'l lane pos'n")];                    //LanePos
    strWhl = paramData[paramNames.indexOf("Steering wheel angle input")];                   //StrWhl10
    crashes = paramData[paramNames.indexOf("Compilation of crashes")];                      //Crashes
    speed = paramData[paramNames.indexOf("Drivers long'l vel'y")];                          //Speed
    throttle = paramData[paramNames.indexOf("Gas pedal input counts")];                     //Throttle
    brake = paramData[paramNames.indexOf("Brake pedal input counts")];                      //Brake
    distance = paramData[paramNames.indexOf("Total long'l dist")];                          //Distance
    nearestcar = paramData[paramNames.indexOf("Closest Vehicle Distance")];                 //Nearest car
    lbs = paramData[paramNames.indexOf("Left Blind Spot")];                                 //Left blind spot
    rbs = paramData[paramNames.indexOf("Right Blind Spot")];                                //Right blind spot
    vehicleHeadingAngle = "Null";
    worldposX = "Null";
    worldposY = "Null";
    worldposZ = "Null";
    worldVehicleHeadingAngle = "Null";

    //Update ADAS state based on driving data
    adasData->updateADAS();


    if(logData->loggingData == "true" && DrivingOnOff == "true")         //Trigger data collection if enabled and active
    {
          if(file.open(QIODevice::Append  | QIODevice::Text))                                 //Setup File Header
        {
            QTextStream stream(&file);                                                      //Open up a write stream
            if (nearestcar == "100000") nearestcar = "0";

            //stream << "Timestamp"         "run number"                 "Task condition"
            if (m_currentTask) {
                pID = m_currentTask->participantID();
                rNum = m_currentTask->repNumber();
              stream << posixTime << ","
                     << m_currentTask->participantID() << ","
                     << m_currentTask->name() << ","
                     << m_currentTask->absoluteTaskNum() << ","
                     << m_currentTask->taskOrder() << "," << m_currentTask->repNumber() << ","
                    << m_currentTask->taskComplexity() << "," << m_currentTask->controlTypes()<< ",";
              stream << simTime << "," << LanePos << "," << strWhl << "," << crashes << ","
                     << speed << "," << throttle << "," << brake << "," << distance << ","
                     << nearestcar
                     << Qt::endl;
            }else {
                stream << posixTime << ","
                       << pID << ","
                       << "No Task Drive" << ","
                       << "-1" << ","
                       << "-1" << "," << rNum << ","
                      << "-1" << "," << "-1"<< ",";
                stream << simTime << "," << LanePos << "," << strWhl << "," << crashes << ","
                       << speed << "," << throttle << "," << brake << "," << distance << ","
                       << nearestcar
                       << Qt::endl;
            }
//              stream  << posixTime << "," << logData->runNumber << "," << logData->taskCondition << ","
//                      << simTime << ","                                                       //STISIMTime
//                      << LanePos << ","                                                       //LanePos
//                      << strWhl  << ","                                                       //StrWhl10
//                      << crashes  << ","                                                      //Crashes
//                      << speed << ","                                                         //Speed
//                      << throttle << ","                                                      //Throttle
//                      << brake << ","                                                         //Brake
//                      << distance << ","                                                      //Distance
//                      << nearestcar << ","                                                    //Nearest car
//                      << lbs << ","                                                           //Left blind spot
//                      << rbs << ","                                                           //Right blind spot
//                      << indicatorStatus                                                      //Indicator
//                      << Qt::endl;
        }


       emit signalDraLogging("true");                                                         //Signal to confirm data is being recorded                                                                                  //Signal to confirm data is being recorded
    }

    SD_LP = SDLP(LanePos.toFloat());                                                          //Calculate new SD Lane Position value  //ADD IN CORRECTION AS PER CALCS
    SD_S = SDS(speed.toFloat());                                                              //Calculate SD Speed

    QVariant unixTime = QVariant::fromValue(posixTime);

    emit setTerminalWindowDriving(simTime,LanePos,strWhl,crashes,speed,throttle,brake,distance,nearestcar,lbs,rbs,indicatorStatus,SD_LP,SD_S,unixTime);

    //Param[0] 1 Elapsed time since the beginning of the run (seconds)
    //Param[1] 2 Driver's lateral lane position with respect to the roadway dividing line
    //Param[2] 3 Steering wheel angle input (degrees)
    //Param[3] 4 Running compilation of the crashes that the driver has been involved in.
    //Param[4] 5 Driver's longitudinal velocity (km/hour)
    //Param[6] 7 Throttle input counts
    //Param[7] 8 Braking input counts.
    //Param[8] 9 Distance travelled in journey
    //
    //1  [0] 1 Elapsed time since the beginning of the run (seconds)
    //7  [1] 2 Driver's lateral lane position with respect to the roadway dividing line
    //11 [2] 3 Steering wheel angle input (degrees)
    //18 [3] 4 Running compilation of the crashes that the driver has been involved in.
    //23 [4] 5 Driver's longitudinal velocity (km/hour)
    //26 [5] 6 Steering input counts. Actual raw steering wheel input directly from the steering wheel.
    //27 [6] 7 Throttle input counts
    //28 [7] 8 Braking input counts.
    //6  [8] 9 Distance travelled in journey
    //10 [9] 10 Vehicle Heading Angle
    //47 [10] 11 World X for vehicle position (comes as 3 values) - NOT SENT ON UDP
    //47 [11] 12 World Y for vehicle position (comes as 3 values) - NOT SENT ON UDP
    //47 [12] 13 World Z for vehicle position (comes as 3 values) - NOT SENT ON UDP
    //41 [13] 14 World Vehicle Heading Angle - NOT SENT ON UDP
    //46 [14] 15 Individual Vehicle Data - NOT SENT ON UDP
    //   [--] 15 Closest Vehicle Param
    //   [--] 16 Right Blind Spot
    //   [--] 17 Left Blind Spot
    //1, 7, 11, 18, 23, 26, 27, 28, 6, 10, 47, 41, 46
}

ULONGLONG drivingDataInput::makeTime(){

    SYSTEMTIME systemTime;                                                  // Setup the timestamp
    FILETIME fileTime;                                                      // the current file time
    ULONGLONG fileTimeNano100, posixTime;                                   // filetime in 100 nanosecond resolution

    GetSystemTime( &systemTime );
    SystemTimeToFileTime( &systemTime, &fileTime );
    fileTimeNano100 = (((ULONGLONG) fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime;
    posixTime = fileTimeNano100/10000 - 11644473600000;                     //to milliseconds and unix windows epoche offset removed

    return posixTime;
}

float drivingDataInput::SDLP(float LanePosition)                            //Function to calculate Standard Deviation of Lane Position
{
    SD_LP_Counter++;                                                        //Increment SD Counter

    SD_LP_Buffer = SD_LP_Buffer + LanePosition;                             //Buffer additional lane position

    lp_mean = SD_LP_Buffer / SD_LP_Counter;                                 //Calculate mean value

    lp_dev = (LanePosition - lp_mean)*(LanePosition - lp_mean);             //Calculate SD

    lp_sdev = lp_sdev + lp_dev;                                             //

    lp_var = lp_sdev / (SD_LP_Counter - 1);                                 //

    lp_sd = sqrt(lp_var);                                                   //

    return lp_sd;                                                           //Return current SD_LP value
}

float drivingDataInput::SDS(float Speed)                                    //Function to calculate Standard Deviation of Lane Position
{
    SD_S_Counter++;                                                         //Increment SD Counter

    SD_S_Buffer = SD_S_Buffer + Speed;                                      //Add total to buffer value

    s_mean = SD_S_Buffer / SD_S_Counter;                                    //Calculate mean value

    s_dev = (Speed - s_mean)*(Speed - s_mean);                              //Calculate standard deviation

    s_sdev = s_sdev + s_dev;                                                //add to mean buffer

    s_var = s_sdev / (SD_S_Counter - 1);                                    //Calculate mean standard deviation

    s_sd = sqrt(s_var);                                                     //

    return s_sd;                                                            //Return current SD_S value
}

void drivingDataInput::resetSDVals()                                      //Function to reset standard deviation parameters
{
    SD_S_Counter = 0;                                                       //Increment SD Counter

    SD_S_Buffer = 0;                                                        //

    s_sdev = 0;                                                             //

    SD_LP_Counter = 0;                                                      //Increment SD Counter

    SD_LP_Buffer = 0;                                                       //

    lp_sdev = 0;                                                            //
}

QString drivingDataInput::setParamName(QString data)
{
    QString buffer = "";

    if(data == "1")buffer = "Elapsed time for current run";
    else if(data == "2")buffer = "Long'l acc'n";
    else if(data == "3")buffer = "Lat'l acc'n";
    else if(data == "4")buffer = "Long'l vel'y";
    else if(data == "5")buffer = "Lateral vel'y";
    else if(data == "6")buffer = "Total long'l dist";
    else if(data == "7")buffer = "Drivers lat'l lane pos'n";
    else if(data == "8")buffer = "Veh' curvature";
    else if(data == "10")buffer = "Veh' heading angle";
    else if(data == "11")buffer = "Steering wheel angle input";
//    else if(data == "9")buffer = "Veh' heading angle";
//    else if(data == "10")buffer = "Steering wheel angle input";
//    else if(data == "11")buffer = "Throttle input";
    else if(data == "12")buffer = "Brake pedal force";
    else if(data == "13")buffer = "Current traffic signal";
    else if(data == "14")buffer = "Current roadway curvature";
    else if(data == "15")buffer = "Horn indicator";
    else if(data == "16")buffer = "Left indicator";
    else if(data == "17")buffer = "Right indicator";
    else if(data == "18")buffer = "Compilation of crashes";
    else if(data == "19")buffer = "Overall min time to collision";
    else if(data == "20")buffer = "Overall min range for current frame";
    else if(data == "21")buffer = "Drivers head yaw angle";
    else if(data == "22")buffer = "Data marker flag";
    else if(data == "23")buffer = "Drivers long'l vel'y";
    else if(data == "24")buffer = "Vehicle yaw rate";
    else if(data == "25")buffer = "Current transmission gear";
    else if(data == "26")buffer = "Steering input counts";
    else if(data == "27")buffer = "Gas pedal input counts";
    else if(data == "28")buffer = "Brake pedal input counts";
    else if(data == "29")buffer = "Scene viewing angle (degrees)";
    else if(data == "30")buffer = "Total pitching angle";
    else if(data == "31")buffer = "Total rolling angle";
    else if(data == "32")buffer = "Steering wheel angular rate";
    else if(data == "33")buffer = "Val of pan scene to left button";
    else if(data == "34")buffer = "Val of pan scene to right button";
    else if(data == "35")buffer = "Min time to collision (seconds)";
    else if(data == "36")buffer = "Min range (feet or meters) between the driver’s vehicle and all vehicles ahead of the driver";
    else if(data == "37")buffer = "Min time to collision (seconds) between the driver’s vehicle and all vehicles opposing the driver’s direction";
    else if(data == "38")buffer = "Min range (feet or meters) between the driver’s vehicle and all vehicles opposing the driver’s direction.";
    else if(data == "39")buffer = "Driver response to the most recent divided attention symbol";
    else if(data == "40")buffer = "Driver response time (seconds) to the most recent divided attention symbol";
    else if(data == "41")buffer = "Total inertial heading angle (radians)";
    else if(data == "42")buffer = "Current status of all of the input buttons";
    else if(data == "43")buffer = "Current status of the digital input port on the secondary I/O card";
    else if(data == "44")buffer = "Current speed limit (feet/second or meters/second)";
    else if(data == "45")buffer = "Number of the most recently activated triggered event";
    else if(data == "46")buffer = "Save data for a specific numbered roadway object";
    else if(data == "47")buffer = "Vehicle inertial position";
    else if(data == "48")buffer = "Roadway centerline inertial position";
    else if(data == "49")buffer = "Not used";
    else if(data == "50")buffer = "Current speed limit (mile/hour or kilometers/hour)";
    else if(data == "51")buffer = "Engine RPM";
    else if(data == "52")buffer = "Clutch pedal input counts.";
    else if(data == "53")buffer = "Hand wheel torque that is output to the steering system on STISIM Drive Model 300";
    else if(data == "54")buffer = "Fuel use (gallons)";
    else if(data == "55")buffer = "Not used";
    else if(data == "56")buffer = "Wind gust sine wave value";
    else if(data == "57")buffer = "Current roadway centerline striping option";
    else if(data == "58")buffer = "Left turn indicator state";
    else if(data == "59")buffer = "Right turn indicator state";
    else if(data == "60")buffer = "Absolute value of the lateral lane position (feet or meters)";
    else if(data == "61")buffer = "Minimum time to collision (seconds)";
    else if(data == "62")buffer = "Minimum range (feet or meters) between the driver’s vehicle and all cross traffic vehicles in the driver’s direction";
    else if(data == "63")buffer = "Minimum time to collision (seconds) between the driver’s vehicle and all pedestrians that are within the extents of the driver’s vehicle";
    else if(data == "64")buffer = "Minimum range (feet or meters) between the driver’s vehicle and all pedestrians that are within the extents of the driver’s vehicle";
    else if(data == "65")buffer = "Running compilation of driver tickets";
    else if(data == "66")buffer = "Absolute value of lateral acceleration (feet/second² or meters/second2)";
    else if(data == "67")buffer = "Absolute value of steering wheel angle (degrees)";
    else if(data == "Closest Vehicle Distance")buffer = "Closest Vehicle Distance";
    else if(data == "Left Blind Spot")buffer = "Left Blind Spot";
    else if(data == "Right Blind Spot")buffer = "Right Blind Spot";
    else if(data == "Timestamp")buffer = "Timestamp";

    return buffer;
}

