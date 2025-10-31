#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QQmlContext>
#include <QObject>
#include <QQuickWindow>
#include <QFont>
#include <QDebug>

#include "filemanagement.h"
#include "drivingdatainput.h"
#include "loggingdatatofile.h"
#include "udpmessagingoutput.h"
#include "hrhdatainput.h"
#include "taskdatainput.h"
#include "eyedatainput.h"
#include "adascontrol.h"
#include "ledcontrol.h"
#include "androidmulticast.h"
#include "imocast.h"
#include <QApplication>
#include "server.h"
#include "QQmlContext"
#include "SWControl.h"
#include "TTSOTaskManager.h"
#include "TTSOTask.h"
#include "TTSOTaskListModel.h"
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    qmlRegisterType<TTSOTask>("App", 1, 0, "TTSOTask");
    qmlRegisterType<TTSOTaskManager>("App", 1, 0, "TTSOTaskManager");
    qmlRegisterType<TTSOTaskListModel>("App", 1, 0, "TTSOTaskListModel");

    QApplication app(argc, argv);
    Server server(8888);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("tcpServer", &server);
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    QObject *topLevel = engine.rootObjects().value(0);

    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    TTSOTaskManager taskManager;
    taskManager.loadHardcodedTasks();

    TTSOTaskListModel taskModel;
    taskModel.setTasks(taskManager.tasks());

    // Expose to QML (so you can access in ListView, etc.)
    engine.rootContext()->setContextProperty("taskModel", &taskModel);
    app.setWindowIcon(QIcon("C:\\Users\\lskrypch\\Dropbox\\Lee\\Logging\\SimEyeLog\\logo.ico"));

    //Object declaration
    filemanagement manageFile;              //File Management done
    drivingDataInput draInput;              //Driving Data done
    loggingDataToFile logData;              //Logging Data done
    taskDataInput taskInput;                //Task Input done
    eyeDataInput eyeInput;                  //Eye Data Done
    UDPMessagingOutput udpOutput;           //Messaging Done
    hrhDataInput hrhInput;                  //
    adasControl adasControl;                //ADAS
    ledcontrol ledcontrol;                  //LEDs//
    androidmulticast androidCast;      //android multicast
    imocast imoCast;      //iMotions multicast
    SWControl swcontrol;
    engine.rootContext()->setContextProperty("cppListModel", &manageFile);

    draInput.initObjects(&logData,&manageFile,&adasControl,&eyeInput);
    taskInput.initObjects(&logData,&manageFile,&udpOutput,&swcontrol);
    eyeInput.initObjects(&logData,&manageFile);
    hrhInput.initObjects(&logData,&manageFile);
    adasControl.initObjects(&udpOutput, &draInput);
    swcontrol.initObjects(&udpOutput, &draInput, &server);
    logData.initObjects(&manageFile,&eyeInput,&hrhInput,&draInput,&taskInput);
    udpOutput.initObjects(&logData,&eyeInput,&hrhInput,&draInput,&taskInput,&adasControl);
    //PASSING DATA BETWEEN OBJECTS - UPDATE LED CONTROL FROM QML
    QObject::connect(&taskModel, &TTSOTaskListModel::activeTaskChanged,
                     &eyeInput,   &eyeDataInput::onActiveTaskChanged);
    QObject::connect(&taskModel, &TTSOTaskListModel::activeTaskChanged,
                     &draInput,   &drivingDataInput::onActiveTaskChanged);
    QObject::connect(&taskModel, &TTSOTaskListModel::activeTaskChanged,
                     &taskInput,   &taskDataInput::onActiveTaskChanged);
    QObject::connect(&taskModel, &TTSOTaskListModel::activeTaskChanged,
                     &logData,   &loggingDataToFile::onActiveTaskChanged);
    // for baseline logging
    // DEMO / EXP logging toggles
    QObject::connect(window, SIGNAL(startDemoLoggingQML()), &eyeInput, SLOT(startDemoLogging()));
    QObject::connect(window, SIGNAL(startDemoLoggingQML()), &draInput, SLOT(startDemoLogging()));
    QObject::connect(window, SIGNAL(stopDemoLoggingQML()),  &eyeInput, SLOT(stopDemoLogging()));
    QObject::connect(window, SIGNAL(stopDemoLoggingQML()),  &draInput, SLOT(stopDemoLogging()));

    QObject::connect(window, SIGNAL(startExpLoggingQML()),  &eyeInput, SLOT(startExpLogging()));
    QObject::connect(window, SIGNAL(startExpLoggingQML()),  &draInput, SLOT(startExpLogging()));
    QObject::connect(window, SIGNAL(stopExpLoggingQML()),   &eyeInput, SLOT(stopExpLogging()));
    QObject::connect(window, SIGNAL(stopExpLoggingQML()),   &draInput, SLOT(stopExpLogging()));
    QObject::connect(&server, &Server::stateUpdated,
                         &draInput, &drivingDataInput::onAndroidInteraction);

    qDebug() << "✅ Android interaction tracking connected";

    //for leds //PKR
    QObject::connect(window, SIGNAL(changeLEDstate(QString)),&ledcontrol, SLOT(changePattern(QString)));
    //RECEIVING DATA FROM DRIVINGINPUT CPP TO ANDROIDMULTICAST CPP //PKR
    QObject::connect(&draInput, SIGNAL(setTerminalWindowDriving(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
                     &androidCast, SLOT(getMulticastData(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));
    //RECEIVING DATA FROM DRIVINGINPUT CPP TO IMOTIONS CPP //PKR
    QObject::connect(&draInput, SIGNAL(setTerminalWindowDriving(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
                     &imoCast, SLOT(receiveMulticastData(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));

    //OBJECTS FROM QML TO FILEMANAGEMENT.H//

    //RECEIVING DATA FROM QML - SET NEW FOLDER LOCATION - SIGNAL QML TO FROM FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(setNewFolderQML(QString)),&manageFile, SLOT(setNewFolder(QString)));
    //RECEIVING DATA FROM QML - SET NEW FILE NAME - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(updateFilenameQML(QString,QString)),&manageFile, SLOT(updateFilename(QString, QString)));
    //RECEIVING DATA FROM QML - GENERATE FOLDER AND FILE - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(makeDirQML()),&manageFile, SLOT(makeDir()));
    //RECEIVING DATA FROM QML - LAUNCH INTERFACE EXE - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(launchExecutableQML()),&manageFile, SLOT(launchInterface()));
    //RECEIVING DATA FROM QML - CLOSE INTERFACE EXE - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(closeExecutableQML()),&manageFile, SLOT(closeInterface()));
    //RECEIVING DATA FROM QML - LAUNCH UPDATE EXECUTABLE URL - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(updateExeUrlQML(QString,QString)),&manageFile, SLOT(updateExeUrl(QString,QString)));
    //RECEIVING DATA FROM QML - LAUNCH CLUSTER ON OFF SETTING - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(updateClusterOnOffQML(QString)),&manageFile, SLOT(updateClusterOnOff(QString)));
    //RECEIVING DATA FROM QML - LAUNCH HUD ON OFF SETTING - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(updateHUDOnOffQML(QString)),&manageFile, SLOT(updateHUDOnOff(QString)));
    //RECEIVING DATA FROM QML - LAUNCH TOUCHSCREEN ON OFF SETTING - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(updateTouchOnOffQML(QString)),&manageFile, SLOT(updateTouchOnOff(QString)));
    //RECEIVING DATA FROM QML - Message Trigger - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(readTestScript(QString)), &manageFile, SLOT(readTestScript(QString)));
    //RECEIVING DATA FROM QML - Message Trigger - SIGNAL FROM QML TO FILEMANAGEMENT.H
    QObject::connect(window, SIGNAL(deleteTestScriptItem(QString)), &manageFile, SLOT(deleteTestScriptItem(QString)));

    //OBJECTS FROM QML TO DRIVINGDATAINPUT.H//

    //RECEIVING DATA FROM QML - Message Trigger
    QObject::connect(window, SIGNAL(sendSTISIMTriggerQML()), &draInput, SLOT(sendSTISIMTrigger()));
    //RECEIVING DATA FROM QML - Update Driving Simulator Logging On Off
    QObject::connect(window, SIGNAL(updateDrivingDataOnOffQML(QString)), &draInput, SLOT(updateDrivingDataOnOff(QString)));
    //RECEIVING DATA FROM QML - Update Driving IP and Port Settings
    QObject::connect(window, SIGNAL(updatePortSettingsDraQML(QString,QString)), &draInput, SLOT(updatePortSettingsDra(QString,QString)));
    //RECEIVING DATA FROM QML - Update Driving IP and Port Settings
    QObject::connect(window, SIGNAL(updateLogOnSimDataQML(QString)), &draInput, SLOT(updateLogOnSimDataOnOff(QString)));

    //OBJECTS FROM QML TO LOGGINGDATATOFILE.H//

    //RECEIVING DATA FROM QML - Turn logging on manually
    QObject::connect(window, SIGNAL(updateLoggingStatusQML(QString)), &logData, SLOT(updateLoggingData(QString)));
    //RECEIVING DATA FROM QML - Update Run Number Value
    QObject::connect(window, SIGNAL(updateRunNumberQML(QString)), &logData, SLOT(updateRunNumber(QString)));
    //RECEIVING DATA FROM QML - Update Task Condition Value
    QObject::connect(window, SIGNAL(updateTaskConditionQML(QString)), &logData, SLOT(updateTaskCondition(QString)));
    //RECEIVING DATA FROM QML - Update whether data is logging or not
    QObject::connect(window, SIGNAL(updateLoggingDataQML(QString)),&logData, SLOT(updateLoggingData(QString)));
    //RECEIVING DATA FROM QML - Write Label to Text Files
    QObject::connect(window, SIGNAL(writeDataLabelQML(QString)), &logData, SLOT(writeDataLabel(QString)));

    //OBJECTS FROM QML TO TASKDATAINPUT.H//

    //RECEIVING DATA FROM QML - Update Task Data Logging On Off
    QObject::connect(window, SIGNAL(updateTaskDataOnOffQML(QString)), &taskInput, SLOT(updateTaskDataOnOff(QString)));
    //RECEIVING DATA FROM QML - Update Task IP and Port Settings
    QObject::connect(window, SIGNAL(updatePortSettingsTaskQML(QString,QString)), &taskInput, SLOT(updatePortSettingsTask(QString,QString)));
    //PASSING DATA BETWEEN OBJECTS - UPDATE ADAS CONTROL FROM SWS
    QObject::connect(window, SIGNAL(setADASControlFromSWS(QString)),&taskInput, SLOT(setADASControlFromSWS(QString)));
    //QObject::connect(window, SIGNAL(startWriteTaskBuffFile(QString)),&taskInput, SLOT(startWriteTaskBuffFile(QString)));
    //QObject::connect(window, SIGNAL(stopWriteTaskBuffFile()),&taskInput, SLOT(stopWriteTaskBuffFile()));

    //OBJECTS FROM QML TO EYEDATAINPUT.H//

    //RECEIVING DATA FROM QML - Update Eye Tracking Logging On Off
    QObject::connect(window, SIGNAL(updateEyeTrackingOnOffQML(QString)), &eyeInput, SLOT(updateEyeTrackingDataOnOff(QString)));
    //RECEIVING DATA FROM QML - Update Eye IP and Port Settings
    QObject::connect(window, SIGNAL(updatePortSettingsEyeQML(QString,QString)), &eyeInput, SLOT(updatePortSettingsEye(QString,QString)));
    //RECEIVING DATA FROM QML - Update Eye IP and Port Settings
    QObject::connect(window, SIGNAL(start_AHEAD_DataFile(QString)), &eyeInput, SLOT(startWriteAttBuffFile(QString)));
     QObject::connect(window, SIGNAL(start_DRA_DataFile(QString)), &draInput, SLOT(startWriteDRABuffFile(QString)));
    //RECEIVING DATA FROM QML - Update Eye IP and Port Settings
    QObject::connect(window, SIGNAL(stop_AHEAD_DataFile()), &eyeInput, SLOT(stopWriteAttBuffFile()));
        QObject::connect(window, SIGNAL(stop_DRA_DataFile(QString)), &draInput, SLOT(stopWriteDRABuffFile(QString)));

    //OBJECTS FROM QML TO UDPMESSAGINGOUTPUT.H//

    //RECEIVING DATA FROM QML - LAUNCH UDP
    QObject::connect(window, SIGNAL(launchUDPQML()),&udpOutput, SLOT(launchUDP()));
    //RECEIVING DATA FROM QML - CLOSE UDP
    QObject::connect(window, SIGNAL(closeUDPQML()),&udpOutput, SLOT(closeUDP()));
    //RECEIVING DATA FROM QML - SET UDP RATE
    QObject::connect(window, SIGNAL(updateUDPRateQML(QString)),&udpOutput, SLOT(updateUDPRate(QString)));
    //PASSING DATA BETWEEN OBJECTS - SELECT SINGLE OR MULTIPLE LOGGING FILE
    QObject::connect(window, SIGNAL(sendWarningMessage(QString)),&udpOutput, SLOT(sendVIWarningMessage(QString)));
    //PASSING DATA BETWEEN OBJECTS - SEND MESSAGE TO STISIM FOR ADAS CONTROL
    QObject::connect(window, SIGNAL(sendADAScontrolMessage(QString)),&udpOutput, SLOT(sendADASMessage(QString)));

    //OBJECTS FROM QML TO HRHDATAINPUT.H//

    //RECEIVING DATA FROM QML - Update HR BR Logging On Off
    QObject::connect(window, SIGNAL(updateHRBROnOffQML(QString)), &hrhInput, SLOT(updateHRBROnOff(QString)));
    //RECEIVING DATA FROM QML - Update HRBR IP and Port Settings
    QObject::connect(window, SIGNAL(updatePortSettingsHRBRQML(QString,QString)), &hrhInput, SLOT(updatePortSettingsHRBR(QString,QString)));

    //TODO: OBJECTS FROM QML TO ADASCONTROL.H//
    //PASSING DATA BETWEEN OBJECTS - SEND MESSAGE TO STISIM FOR ADAS CONTROL
    QObject::connect(window, SIGNAL(changeADAState(QString,QString)),&adasControl, SLOT(controllerADAS(QString,QString))); //NEW
    //QObject::connect(window, SIGNAL(changeADAState(QString)),&adasControl, SLOT(testADAS(QString)));


    //OBJECTS FROM FILEMANAGEMENT.H TO QML//

    //PASSING DATA TO QML - Send current folder location - SIGNAL FROM FILEMANAGEMENT.H TO QML
    QObject::connect(&manageFile, SIGNAL(setFolderLocation(QVariant)), window, SLOT(setFolderLocationQML(QVariant)));
    //PASSING DATA TO QML - Send list of current interface folder locations - SIGNAL FROM FILEMANAGEMENT.H TO QML
    QObject::connect(&manageFile, SIGNAL(currentInterfaceExeLocations(QVariant,QVariant,QVariant)), window, SLOT(currentInterfaceExeLocationsQML(QVariant,QVariant,QVariant)));
    //PASSING DATA TO QML - Send current folder location - SIGNAL FROM FILEMANAGEMENT.H TO QML
    QObject::connect(&manageFile, SIGNAL(sendTestScript(QVariant)), window, SLOT(sendTestScript(QVariant)));

    //OBJECTS FROM DRIVING DRIVINGDATAINPUT.H TO QML//

    //PASSING DATA TO QML - Is SIMEYELOG recieving Driving Input Data?
    QObject::connect(&draInput, SIGNAL(signalDraRefresh(QVariant)), window, SLOT(signalDraRefreshQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG Driving data being logged - multi file
    QObject::connect(&draInput, SIGNAL(signalDraLogging(QVariant)), window, SLOT(signalDraLoggingQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG Set logging started to true
    QObject::connect(&draInput, SIGNAL(setLoggingOnSimData(QVariant)), window, SLOT(setLoggingOnSimDataQML(QVariant)));
    //PASSING DATA TO QML - Send Driving Data to QML?
    QObject::connect(&draInput, SIGNAL(setTerminalWindowDriving(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
                                window, SLOT(setTerminalWindowDrivingQML(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));


    //OBJECTS FROM LOGGINGDATATOFILE.H TO QML//

    //PASSING DATA TO QML - Is SIMEYELOG Logging Eye Glance Data?
    QObject::connect(&logData, SIGNAL(signalEyeLogging(QVariant)), window, SLOT(signalEyeLoggingQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG Logging Task Input Data?
    QObject::connect(&logData, SIGNAL(signalTaskLogging(QVariant)), window, SLOT(signalTaskLoggingQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG logging Heart Rate Data?
    QObject::connect(&logData, SIGNAL(signalHRBRLogging(QVariant)), window, SLOT(signalHRBRLoggingQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG Driving data being logged - single file
    QObject::connect(&logData, SIGNAL(signalDraLogging(QVariant)), window, SLOT(signalDraLoggingQML(QVariant)));

    //OBJECTS FROM TASKDATAINPUT.H TO QML//

    //PASSING DATA TO QML - Is SIMEYELOG recieving Task Input Data?
    QObject::connect(&taskInput, SIGNAL(signalTaskRefresh(QVariant)), window, SLOT(signalTaskRefreshQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG Logging Task Input Data?
    QObject::connect(&taskInput, SIGNAL(signalTaskLogging(QVariant)), window, SLOT(signalTaskLoggingQML(QVariant)));
    //PASSING DATA TO QML - Send Task Data to QML?
    QObject::connect(&taskInput, SIGNAL(setTerminalWindowTask(QVariant,QVariant)), window, SLOT(setTerminalWindowTaskQML(QVariant,QVariant)));


    //OBJECTS FROM EYEDATAINPUT.H TO QML//

    //PASSING DATA TO QML - Is SIMEYELOG recieving Eye Glance Data?
    QObject::connect(&eyeInput, SIGNAL(signalEyeRefresh(QVariant)), window, SLOT(signalEyeRefreshQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG Logging Eye Glance Data?
    QObject::connect(&eyeInput, SIGNAL(signalEyeLogging(QVariant)), window, SLOT(signalEyeLoggingQML(QVariant)));
    //PASSING DATA TO QML - Send Eye Glance Data to QML? //old tracker
//    QObject::connect(&eyeInput, SIGNAL(setTerminalWindowGlance(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
//                                window, SLOT(setTerminalWindowGlanceQML(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));
    //PASSING DATA TO QML - Send Eye Glance Data to QML? //new tracker
    QObject::connect(&eyeInput, SIGNAL(setTerminalWindowGlance(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
                                window, SLOT(setTerminalWindowGlanceQML(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));

    //OBJECTS FROM UDPMESSAGINGOUTPUT.H TO QML//
    //NONE

    //OBJECTS FROM HRHDATAINPUT.H TO QML//

    //PASSING DATA TO QML - Is SIMEYELOG recieving Heart Rate Data?
    QObject::connect(&hrhInput, SIGNAL(signalHRBRRefresh(QVariant)), window, SLOT(signalHRBRRefreshQML(QVariant)));
    //PASSING DATA TO QML - Is SIMEYELOG logging Heart Rate Data?
    QObject::connect(&hrhInput, SIGNAL(signalHRBRLogging(QVariant)), window, SLOT(signalHRBRLoggingQML(QVariant)));



    //Initialise Functions
    manageFile.initialisation();

    return app.exec();
}
