#include "taskdatainput.h"
#include <QFile>
#include <QTextStream>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include "SWControl.h"
#include "windows.h"
#include "loggingdatatofile.h"
#include "filemanagement.h"
#include "udpmessagingoutput.h"
#include "TTSOTask.h"
taskDataInput::taskDataInput(QObject *parent) : QObject(parent)
{
    //DATA INCOMING FROM TASK INTERFACE///////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(dataTriggerCommand()));            //Setup and connect timer to check if signal is refreshed

    //Copy them to ADAS Control
    //CENTRE SCREEN INPUT/////////////////////////////////////////////////////////////
    udpTouchScreen = new UDPSocket(this);                                           //Setup UDP Socket to communicate speed and VI region to other programs
    udpTouchScreen->ConnectSingle(7001);                                            //Centre Screen
    connect(udpTouchScreen,SIGNAL(messageReceived(QByteArray)),this,SLOT(recievedDataTouchscreen(QByteArray)));

    //STEERING WHEEL INPUT////////////////////////////////////////////////////////////
    comSteeringWheel = new jercserialcommv10();
    comSteeringWheel->setCOMport(false,"COM3");
    comSteeringWheel->openPort(QSerialPort::Baud9600,QSerialPort::Data8,QSerialPort::NoParity,QSerialPort::OneStop,QSerialPort::HardwareControl);

    connect(comSteeringWheel,SIGNAL(receivedDataAsByte(QByteArray)),this,SLOT(recievedDataSteeringWheel(QByteArray)));
   // connect(timerBuffer, SIGNAL(timeout()), this, SLOT(writeTaskBuffFile()));

    //STEERING WHEEL SWITCH BUFFER PARAMETERS/////////////////////////////////////////
    QString data = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";           //Incoming Data buffer

    sws_buffer = data.split(',');                                                   //setup temporary buffer
}

void taskDataInput::initObjects(loggingDataToFile *loggingDataToFile_ptr, filemanagement *filemanagement_ptr, UDPMessagingOutput *udpmessagingoutput_ptr, SWControl *swControl_ptr)
{
    logData = loggingDataToFile_ptr;                                                //Pointers to logging data object
    fileData = filemanagement_ptr;                                                  //Pointer to file management object
    updOutputData = udpmessagingoutput_ptr;
    swControl = swControl_ptr;
    if (swControl) {
            bool ok = connect(swControl, &SWControl::newButtonEvent,
                              this, &taskDataInput::recievedDataNewSteeringWheel);
            qDebug() << "[taskDataInput] connect to SWControl was" << (ok ? "OK" : "FAILED");
        }
}

void taskDataInput::updateTaskDataOnOff (QString taskOnOff_set)
{
    taskOnOff = taskOnOff_set;                                                      //Update task data to be logged flag
}

void taskDataInput::updatePortSettingsTask(QString ipaddress, QString port)
{
    int port_val = port.toInt();

    if(port_val != taskPort && ipaddress != taskHost)
    {   //m_udpsocket_Eye->ConnectSingleAltAddress(port_val, ipaddress);            //Reset Port Settings
    }
}

void taskDataInput::dataTriggerCommand()                                            //Timer Trigger
{
    timer->stop();                                                                  //Stop timer and reset

    emit signalTaskRefresh("false");                                                //Set interface green
    emit signalTaskLogging("false");                                                //Reset if timer expires
}

void taskDataInput::recievedDataTouchscreen(QByteArray taskData)
{
    timer->stop();                                                                  //Stop timer and reset

    timer->start(signalRefreshTimer);                                               //Start timer

    emit signalTaskRefresh("true");                                                 //Set interface green

    ULONGLONG posixTime = makeTime();                                               //Create Timestamp

    lastComponent = "Touchscreen";                                                  //Last triggered component

    lastEvent = taskData;                                                            //Assign last event to variable

    emit setTerminalWindowTask(lastComponent,lastEvent);                            //Send Message to Terminal

    if(logData->loggingData == "true" && taskOnOff == "true")
    {
        QFile file(fileData->taskFilename);                                         //Setfilename

        if(file.open(QIODevice::Append  | QIODevice::Text))                         //Setup File Header
        {
            QTextStream stream(&file);                                              //Open up a write stream

            stream << posixTime << "," << logData->runNumber << "," << logData->taskCondition  << "," << "Touchscreen" <<"," << lastEvent << Qt::endl; //write line
        }

        emit signalTaskLogging("true");                                             //Signal to confirm data is being recorded
    }
}
//void taskDataInput::startWriteTaskBuffFile(QString fileNum)
//{
//    taskFilenameTask = fileData->fullfolderpath + "/" +
//                       fileData->filename + "_TASK_T" + fileNum + ".csv";

//    QFile fileNew(taskFilenameTask);

//    if (fileNew.open(QIODevice::Append | QIODevice::Text)) {
//        QTextStream stream(&fileNew);

//        // ✅ Unified header (matches DRA and AHEAD first 5 cols)
//        stream << "Frame" << ",ElapsedBufferTime(s)"
//               << ",Timestamp" << ",RunNumber" << ",TaskCondition"
//               // task-specific:
//               << ",Component" << ",Event"
//               << Qt::endl;
//    }

//    task_frameNumberByTask = 0;
//    task_time_buffer_ahead = 0;

//    timerBuffer->start(taskBufferRefresh);

//    qDebug() << "[TASK] Buffer logging started, file:" << taskFilenameTask;
//}

//void taskDataInput::stopWriteTaskBuffFile()
//{
//    timerBuffer->stop();
//    task_time_buffer_ahead = 0;
//    task_frameNumberByTask = 0;

//    qDebug() << "[TASK] Buffer logging stopped.";
//}

//void taskDataInput::writeTaskBuffFile()
//{
//    if (logData->loggingData != "true" || taskOnOff != "true")
//        return;

//    ULONGLONG posixTime = makeTime();

//    QFile file(taskFilenameTask);
//    if (file.open(QIODevice::Append | QIODevice::Text)) {
//        QTextStream stream(&file);

//        // ✅ Unified first 5 columns
//        stream << task_frameNumberByTask << ","
//               << QString::number(task_time_buffer_ahead / 1000.0, 'f', 3) << ","
//               << posixTime << "," << logData->runNumber << "," << logData->taskCondition
//               // task-specific
//               << "," << lastComponent << "," << lastEvent
//               << Qt::endl;
//    }

//    task_frameNumberByTask++;
//    task_time_buffer_ahead += 33.333; // ~30 Hz
//}
void taskDataInput::onActiveTaskChanged(TTSOTask *task)
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
void taskDataInput::recievedDataNewSteeringWheel(QString component, QString event)
{
    qDebug() << "[taskDataInput] Got event from SWControl:" << component << event;
    ULONGLONG posixTime = makeTime();

       lastComponent = component;
       lastEvent = event;

       emit setTerminalWindowTask(lastComponent, lastEvent);

       if (logData->loggingData == "true" && taskOnOff == "true") {
           QFile file(fileData->taskFilename);
           if (file.open(QIODevice::Append | QIODevice::Text)) {
               QTextStream stream(&file);
               if (m_currentTask) {
                   pID = m_currentTask->participantID();
                   rNum = m_currentTask->repNumber();
               stream << posixTime << ","
                                     << m_currentTask->participantID() << ","
                                     << m_currentTask->name() << ","
                                     << m_currentTask->absoluteTaskNum() << ","
                                     << m_currentTask->taskOrder() << "," << m_currentTask->repNumber() << "," << m_currentTask->taskComplexity() << "," <<  m_currentTask->controlTypes()<< ","<< "Steering Wheel" <<"," << lastEvent << Qt::endl; //write line
               }
               else{
                   stream << posixTime << ","
                                         << pID << ","
                                         << "No Task Drive" << ","
                                         << "-1" << ","
                                         << "-1" << "," << rNum << "," << "-1" << "," <<  "-1" << ","<< "Steering Wheel" <<"," << lastEvent << Qt::endl; //write line
               }
           }
           emit signalTaskLogging("true");
       }
}

void taskDataInput::recievedDataSteeringWheel(QByteArray taskData)
{
    timer->stop();                                                                  //Stop timer and reset

    timer->start(signalRefreshTimer);                                               //Start timer

    emit signalTaskRefresh("true");                                                 //Set interface green

    ULONGLONG posixTime = makeTime();                                               //Create Timestamp

    QString data, lastEventBuffer;                                                  //Incoming Data buffer

    QList<QString> sws;                                                             //List to buffer data

    QStringList params;                                                             //Buffer variable

    data.append(taskData);                                                          //Parse Data

    messageBuilder += data;                                                         //Buffer data

    if(messageBuilder.contains("\r\n") && messageBuilder.length() > 8)              //Check if extra characters are added
    {
        params = messageBuilder.split("\r\n");                                      //Remove extra characters from string
    }

    if(params.count() >= 1)
    {
        messageBuilder = "";                                                        //Blank the parameters

        QString temp = params[0];                                                   //Buffer data
        //qDebug()<<temp;
        lastEventBuffer = temp.toStdString().c_str();                               //Set last event

        if(params.count() >= 2){  messageBuilder = params[1];       }

        sws = lastEventBuffer.split(',');                                           //Seperate the data into the list

        // TODO: Needs updating for more data - will be 27 items once the seat belt latch added
        // ALSO!!! Could do with reworking to be backwards compatible with other steering wheel
        // Arduinos that output less bits of information OR just hack it to suit!
        for (int i_sws=0; i_sws<26; i_sws++)
        {
            if(sws[i_sws] != sws_buffer[i_sws])
            {
                switch (i_sws) {
                case 0:                                                 //[0] Stork - Left Indicator - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "LeftIndicatorOff";
                    if(sws[i_sws] == "1") lastEvent = "LeftIndicatorOn";
                    break;
                case 1:                                                 //[1] Stork - Right Indicator - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "RightIndicatorOff";
                    if(sws[i_sws] == "1") lastEvent = "RightIndicatorOn";
                    break;
                case 2:                                                 //[2] Stork - End (i) Button - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "IndicatorEndButtonOff";
                    if(sws[i_sws] == "1") lastEvent = "IndicatorEndButtonOn";
                    break;
                case 3:                                                 //[3] Stork - Front Fogs (not present on the stalk we are using) - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "FrontFogsOff";
                    if(sws[i_sws] == "1") lastEvent = "FrontFogsOn";
                    break;
                case 4:                                                 //[4] Stork - Rear Fogs - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "RearFogsOff";
                    if(sws[i_sws] == "1") lastEvent = "RearFogsOn";
                    break;
                case 5:                                                 //[5] Stork - Lights (0=OFF,1=Side,2=Head,3=Auto) - note: Full Beam not wired
                    if(sws[i_sws] == "0") lastEvent = "LightsOff";
                    else if(sws[i_sws] == "1") lastEvent = "SidelightsOn";
                    else if(sws[i_sws] == "2") lastEvent = "HeadlightsOn";
                    else lastEvent = "AutoOn";
                    break;
                case 6:                                                 //[6] Left Sw - Skip/move left - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "LeftSwitchReleased";
                    if(sws[i_sws] == "1") lastEvent = "LeftSwitchPressed";
                    break;
                case 7:                                                 //[7] Left Sw - Vol/move up - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "UpSwitchReleased";
                    if(sws[i_sws] == "1") lastEvent = "UpSwitchPressed";
                    break;
                case 8:                                                 //[8] Left Sw - Skip/move right - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "RightSwitchReleased";
                    if(sws[i_sws] == "1") lastEvent = "RightSwitchPressed";
                    break;
                case 9:                                                 //[9] Left Sw - Vol/move down - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "DownSwitchReleased";
                    if(sws[i_sws] == "1") lastEvent = "DownSwitchPressed";
                    break;
                case 10:                                               //[10] Left Sw - Mode - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "ModeSwitchReleased";
                    if(sws[i_sws] == "1") lastEvent = "ModeSwitchPressed";
                    break;
                case 11:                                               //[11] Left Sw - Phone - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "PhoneReleased";
                    if(sws[i_sws] == "1") lastEvent = "PhonePressed";
                    break;
                case 12:                                               //[12] Left Sw - Menu/Ok - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "MenuOKReleased";
                    if(sws[i_sws] == "1") lastEvent = "MenuOKPressed";
                    break;
                case 13:                                               //[13] Right Sw - Lim (ASL) button - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "LimitReleased";
                    if(sws[i_sws] == "1") lastEvent = "LimitPressed";
                    break;
                case 14:                                               //[14] Right Sw - Resume (Res) button - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "ResumeReleased";
                    if(sws[i_sws] == "1") lastEvent = "ResumePressed";
                    break;
                case 15:                                               //[15] Right Sw - Set/Increase speed (SET+) button - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "SetReleased";
                    if(sws[i_sws] == "1") lastEvent = "SetPressed";
                    break;
                case 16:                                               //[16] Right Sw - Reduce distance button - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "ReduceDistReleased";
                    if(sws[i_sws] == "1") lastEvent = "ReduceDistPressed";
                    break;
                case 17:                                               //[17] Right Sw - Increase distance button - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "IncreaseDistReleased";
                    if(sws[i_sws] == "1") lastEvent = "IncreaseDistPressed";
                    break;
                case 18:                                               //[18] Right Sw - Decrease speed (-) button - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "DecreaseSpeedReleased";
                    if(sws[i_sws] == "1") lastEvent = "DecreaseSpeedPressed";
                    break;
                case 19:                                               //[19] Right Sw - Cancel (CAN) button
                    if(sws[i_sws] == "0") lastEvent = "CancelReleased";
                    if(sws[i_sws] == "1") lastEvent = "CancelPressed";
                    break;
                case 20:                                               //[20] Left Paddle - Shift Down - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "LeftPaddleReleased";
                    if(sws[i_sws] == "1") lastEvent = "LeftPaddlePressed";
                    break;
                case 21:                                               //[21] Right Paddle - Shift Up - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "RightPaddleReleased";
                    if(sws[i_sws] == "1") lastEvent = "RightPaddlePressed";
                    break;
                case 22:                                               //[22] Right Paddle - Shift Up - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "HeatedSeatReleased";
                    if(sws[i_sws] == "1") lastEvent = "HeatedSeatPressed";
                    break;
                case 23:                                               //[23] Right Paddle - Shift Up - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "LaneKeepReleased";
                    if(sws[i_sws] == "1") lastEvent = "LaneKeepPressed";
                    break;
                case 24:                                               //[24] Right Paddle - Shift Up - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "VoiceControlReleased";
                    if(sws[i_sws] == "1") lastEvent = "VoiceControlPressed";
                    break;
                case 25:                                               //[25] Right Paddle - Shift Up - 0=off, 1=on
                    if(sws[i_sws] == "0") lastEvent = "HandsOnWheelReleased";
                    if(sws[i_sws] == "1") lastEvent = "HandsOnWheelTouched";
                    break;
                default:
                    break;
                }

                lastComponent = "SteeringWheel";                                            //Last triggered component

                emit setTerminalWindowTask(lastComponent,lastEvent);                        //Send data to QML

                if(logData->loggingData == "true" && taskOnOff == "true")
                {
                    QFile file(fileData->taskFilename);                                     //Setfilename

                    if(file.open(QIODevice::Append  | QIODevice::Text))                     //Setup File Header
                    {
                        QTextStream stream(&file);                                          //Open up a write stream
                        if (m_currentTask) {
                            pID = m_currentTask->participantID();
                            rNum = m_currentTask->repNumber();
                        stream << posixTime << ","
                                              << m_currentTask->participantID() << ","
                                              << m_currentTask->name() << ","
                                              << m_currentTask->absoluteTaskNum() << ","
                                              << m_currentTask->taskOrder() << "," << m_currentTask->repNumber() << "," << m_currentTask->taskComplexity() << "," <<  m_currentTask->controlTypes() << ","<< "Steering Wheel" << "," << lastEvent << Qt::endl; //write line
                        } else{

                                stream << posixTime << ","
                                                      << pID << ","
                                                      << "No Task Drive" << ","
                                                      << "-1" << ","
                                                      << "-1" << "," << rNum << "," << "-1" << "," <<  "-1" << ","<< "Steering Wheel" <<"," << lastEvent << Qt::endl; //write line

                        }
                    }

                    emit signalTaskLogging("true");                                         //Reset if timer expires
                }
            }
        }

        if(sws[0] == "1")indicatorStatus = "1";                                             //Set indicator status to 1 if down

        else if(sws[1] == "1")indicatorStatus = "2";                                        //Set indicator status to 2 if up

        else indicatorStatus = "0";

        //Add here mapping for inputs that activate ADAS features

        if(sws[15] == "1" && enableADASfromSWS == "On") updOutputData->sendADASMessage("SpdLimiter");  //SET PLUS

        if(sws[19] == "1" && enableADASfromSWS == "On") updOutputData->sendADASMessage("Manual");    //CANCEL

        //Create message for multicast
        steeringWheelString = sws[0]+"/"+sws[1]+"/"+sws[2]+"/"+sws[3]+"/"+sws[4]+"/"+sws[5]+"/"+sws[6]+"/"+sws[7]+"/"+sws[8]+"/"+sws[9]+"/"+sws[10]+"/"+sws[11]+"/"+sws[12]+"/"+sws[13]+"/"+sws[14]+"/"+sws[15]+"/"+sws[16]+"/"+sws[17]+"/"+sws[18]+"/"+sws[19]+"/"+sws[20]+"/"+sws[21]+"/"+sws[22]+"/"+sws[23]+"/"+sws[24]+"/"+sws[25];

        sws_buffer = sws;                                                                   //buffer the data.
    }

}



void taskDataInput::setADASControlFromSWS(QString state)
{
    enableADASfromSWS = state;
}

void taskDataInput::receivedDataDRTButton(QByteArray taskData)
{
    timer->stop();                                                                  //Stop timer and reset

    timer->start(signalRefreshTimer);                                               //Start timer

    emit signalTaskRefresh("true");                                                 //Set interface green

    ULONGLONG posixTime = makeTime();                                               //Create Timestamp

    QString buffer = taskData;                                                      //Parse button press

    buffer = buffer.at(0);                                                          //Find out first character

    if(buffer == "1")lastEvent = "Pressed";                                         //If button pressed send pressed
    else lastEvent = "Released";

    lastComponent = "DRT Button";                                                   //Last triggered component

    emit setTerminalWindowTask(lastComponent,lastEvent);                            //Send Message to Terminal

    if(logData->loggingData == "true" && taskOnOff == "true")
    {
        QFile file(fileData->taskFilename);                                         //Setfilename

        if(file.open(QIODevice::Append  | QIODevice::Text))                         //Setup File Header
        {
            QTextStream stream(&file);                                              //Open up a write stream

            stream << posixTime << "," << logData->runNumber << "," << logData->taskCondition  << "," << "DRT Button" <<"," << lastEvent << Qt::endl;  //write line
        }
    }
}

ULONGLONG taskDataInput::makeTime()
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
