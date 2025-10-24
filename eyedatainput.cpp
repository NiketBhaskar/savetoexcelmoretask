#include "eyedatainput.h"
#include <QFile>
#include <QTextStream>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include "windows.h"
#include "loggingdatatofile.h"
#include "filemanagement.h"
#include <QDir>
#include <QRegularExpression>
#include <QVariant>

#include "include_SE/SEDataTypes.h"
#include "include_SE/SEPacketAPI.h"
#include "include_SE/SEOutputDataIds.h"
#include "include_SE/SEErrors.h"
#include "include_SE/SETrackerStates.h"
#include "include_SE/TimeService.h"
#include "TTSOTask.h"
//#include "SEinclude/SEDataTypes.h"
//#include "SEinclude/SEPacketAPI.h"
//#include "SEinclude/SEOutputDataIds.h"
//#include "SEinclude/SEErrors.h"
//#include "SEinclude/SETrackerStates.h"
//#include "SEinclude/TimeService.h"


eyeDataInput::eyeDataInput(QObject *parent) : QObject(parent)
{
    //UPD PORT RECIEVER////////////////////////////////////////////////////////////
    m_udpsocket_Eye = new UDPSocket(this);                                       //Port for Eye Tracker
    m_udpsocket_Eye->ConnectSingleAltAddress(eyePort, eyeHost);
    connect(m_udpsocket_Eye,SIGNAL(messageReceived(QByteArray)),this,SLOT(recievedData(QByteArray)));
    //DATA INCOMING FROM EYE TRACKER//////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(dataTriggerCommand()));        //Setup and connect timer to check if signal is refreshed
    //TIME FOR WRITING BUFFER EYE TRACKER/////////////////////////////////////////
    connect(timerBuffer, SIGNAL(timeout()), this, SLOT(writeAttBuffFile()));    //Setup and connect timer to check if signal is refreshed
}
static inline QString buildSimpleTaskName(TTSOTask *task) {
    if (!task) return "UnknownTask";

    int absNum = task->absoluteTaskNum();
    int rep = task->repNumber();
    int pID = task->participantID();

    // ✅ FIX: Baseline drive (absNum = 121, rep = 0) - WAS CHECKING absNum == 0
    if (absNum == -1 && rep == -1) {
        return QString("P%1_Baseline").arg(pID);
    }

    // ✅ FIX: Experienced drive (absNum = 122, rep = 11) - WAS CHECKING absNum == -1
    if (absNum == -2 && rep == -2) {
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
void eyeDataInput::initObjects(loggingDataToFile *loggingDataToFile_ptr, filemanagement *filemanagement_ptr)
{
    logData = loggingDataToFile_ptr;                                            //Pointers to logging data object
    fileData = filemanagement_ptr;                                              //Pointer to file management object
}

void eyeDataInput::updateEyeTrackingDataOnOff(QString eyeOnOffUpdate)
{    eyeOnOff = eyeOnOffUpdate;
}

void eyeDataInput::updatePortSettingsEye(QString ipaddress, QString port)
{
    int port_val = port.toInt();

    if(port_val != eyePort && ipaddress != eyeHost)
    {
        m_udpsocket_Eye->ConnectSingleAltAddress(port_val, ipaddress);          //Reset Port Settings
    }
}

void eyeDataInput::dataTriggerCommand()                                         //Timer Trigger
{
    timer->stop();                                                              //Stop timer and reset

    emit signalEyeRefresh("false");                                             //Set interface green

    emit signalEyeLogging("false");                                             //Reset if timer expires
}
void eyeDataInput::startCustomLogging(const QString &label) {
    m_customMode = true;
    m_customLabel = label;        // "demo" or "exp"

    taskStartEpochTime = makeTime();
    frameNumberByTask = 0;
    resetMeanLiveAttenD();
    glanceSampleBuffer.clear();

    QDir dir(fileData->fullfolderpath + "/EYETASKDATA");
    if (!dir.exists()) dir.mkpath(".");

    QString base = sanitize(label); // "demo" / "exp"
    aheadFilenameTask        = dir.filePath(base + "_EYE.csv");
    aheadFilenameTaskChopped = dir.filePath(base + "_AHEAD.csv");

    QFile f1(aheadFilenameTask);
    if (f1.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream s(&f1);
        s << "Unix Timestamp,Participant ID,RunType,AbsTaskNum,TaskOrder,RepNumber,ElapsedBufferTime(s),"
             "TaskComplexity,Control Types,GazeQuality,CurrentGlanceLocation,FilteredGlanceLocation,"
             "PreciseGlanceLocation,EstimatedGlanceLocation,Current_G_Time(s),LiveAttenD,X,Y,Z"
          << Qt::endl;
    }
    QFile f2(aheadFilenameTaskChopped);
    if (f2.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream s(&f2);
        s << "frame,TIME,label" << Qt::endl;
    }

    timerBuffer->start(attentionBufferRefresh);
}

void eyeDataInput::stopCustomLogging() {
    timerBuffer->stop();
    writeInterpolatedAHEAD(); // flush last frames
    glanceSampleBuffer.clear();
    taskStartEpochTime = 0;
    frameNumberByTask = 0;
    m_customMode = false;
    m_customLabel.clear();
}
void eyeDataInput::startDemoLogging() { startCustomLogging("demo"); }
void eyeDataInput::stopDemoLogging()  { stopCustomLogging(); }
void eyeDataInput::startExpLogging()  { startCustomLogging("exp"); }
void eyeDataInput::stopExpLogging()   { stopCustomLogging(); }

void eyeDataInput::onActiveTaskChanged(TTSOTask *task)
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

void eyeDataInput::startWriteAttBuffFile(QString fileNum)
{
    logData->taskCondition = fileNum;
    ULONGLONG posixTime = makeTime();
    taskStartEpochTime = posixTime;  // ✅ Store task start epoch time
    frameNumberByTask = 0;
    resetMeanLiveAttenD();
    glanceSampleBuffer.clear();  // ✅ Clear buffer
    QDir dir(fileData->fullfolderpath + "/EYETASKDATA");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString baseName;
    if (m_currentTask)
        baseName = buildSimpleTaskName(m_currentTask);
    else
        baseName = "Baseline";
//    aheadFilenameTask = dir.absolutePath() + "/" + fileData->fullfolderpath + "/" +
//                        fileData->filename + "_EYE_T" + fileNum + ".csv";
    //aheadFilenameTask = dir.filePath(fileData->filename + "_EYE_T" + fileNum + ".csv");
    aheadFilenameTask = dir.filePath(baseName + "_EYE.csv");
    QFile fileNew(aheadFilenameTask);

    if (fileNew.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&fileNew);
          stream << "Participant ID: " << fileData->filename << ",Start Time: " << posixTime << Qt::endl;
        stream << "Unix Timestamp" << ",Participant ID" << ",TaskName"<< ",AbsTaskNum" << ",TaskOrder" << ",RepNumber" << ",ElapsedBufferTime(s)"
               << ",TaskComplexity" << ",Control Types" << ",GazeQuality" << ",CurrentGlanceLocation" << ",FilteredGlanceLocation"
               << ",PreciseGlanceLocation" << ",EstimatedGlanceLocation"
               << ",Current_G_Time(s)" << ",LiveAttenD" << ",X" << ",Y" << ",Z"
               << Qt::endl;
    }

    //aheadFilenameTaskChopped = dir.filePath(fileData->filename + "_AHEAD_T" + fileNum + ".csv");
    aheadFilenameTaskChopped = dir.filePath(baseName + "_AHEAD.csv");
    QFile file10(aheadFilenameTaskChopped);
    if (file10.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file10);
         stream << "frame" << ",TIME" << ",label" << Qt::endl;
    }
    frameNumberByTask = 0;

    timerBuffer->start(attentionBufferRefresh);
}



void eyeDataInput::stopWriteAttBuffFile()                                   //Function to calculate attention buffer parameters
{
    timerBuffer->stop();                                                    //Start timer

    // ✅ Write any remaining complete intervals
    writeInterpolatedAHEAD();

    // ✅ Clear buffer and reset
    glanceSampleBuffer.clear();
    taskStartEpochTime = 0;  // ✅ Reset
    frameNumberByTask = 0;
}


void eyeDataInput::writeAttBuffFile()
{
    ULONGLONG posixTime = makeTime();

    if (taskStartEpochTime == 0) return;

    double elapsedTime = (posixTime - taskStartEpochTime) / 1000.0;

    QFile file3(aheadFilenameTask);
    if (file3.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file3);

        if (CurrentGlanceLocation == "headupdisplay") CurrentGlanceLocation = "road";
        if (filteredGlanceLocation == "headupdisplay") filteredGlanceLocation = "road";
        if (PreciseGlanceLocation == "headupdisplay") PreciseGlanceLocation = "road";
        if (EstimatedLocation == "headupdisplay") EstimatedLocation = "road";

        if (m_currentTask) {
            // Normal task logging
            pID = m_currentTask->participantID();
            rNum = m_currentTask->repNumber();
            stream << posixTime << ","
                   << m_currentTask->participantID() << ","
                   << m_currentTask->name() << ","
                   << m_currentTask->absoluteTaskNum() << ","
                   << m_currentTask->taskOrder() << ","
                   << m_currentTask->repNumber() << ",";
            stream << QString::number(elapsedTime, 'f', 3) << ","
                   << m_currentTask->taskComplexity() << ","
                   << m_currentTask->controlTypes() << ","
                   << quality << ","
                   << CurrentGlanceLocation << ","
                   << filteredGlanceLocation << ","
                   << PreciseGlanceLocation << ","
                   << EstimatedLocation << ","
                   << QString::number(currentGlanceTime / 1000.0, 'f', 3) << ","
                   << QString::number(KA_Buffer_Val_float / 1000.0, 'f', 3) << ","
                   << resolutionX << "," << resolutionY << "," << resolutionZ
                   << Qt::endl;
        } else if (m_customMode) {
            QString runType = m_customLabel; // "demo" or "exp"
            stream << posixTime << ","
                   << pID << ","               // keep if you have it; else ""
                   << runType << ","
                   << "0" << ","               // AbsTaskNum
                   << "0" << ","               // TaskOrder
                   << "0" << ",";              // RepNumber
            stream << QString::number(elapsedTime, 'f', 3) << ","
                   << runType << "," << runType << ","
                   << quality << ","
                   << CurrentGlanceLocation << "," << filteredGlanceLocation << ","
                   << PreciseGlanceLocation  << "," << EstimatedLocation << ","
                   << QString::number(currentGlanceTime / 1000.0, 'f', 3) << ","
                   << QString::number(KA_Buffer_Val_float / 1000.0, 'f', 3) << ","
                   << resolutionX << "," << resolutionY << "," << resolutionZ
                   << Qt::endl;
        } else {
            // Baseline or End Capture logging
//            QString taskName = "Baseline";
//            if (aheadFilenameTask.contains("EndCapture")) {
//                taskName = "End Capture";
//            }

//            stream << posixTime << ","
//                   << pID << ","
//                   << taskName << ","
//                   << "0" << ","
//                   << "0" << ","
//                   << "0" << ",";
//            stream << QString::number(elapsedTime, 'f', 3) << ","
//                   << taskName << ","
//                   << taskName << ","
//                   << quality << ","
//                   << CurrentGlanceLocation << ","
//                   << filteredGlanceLocation << ","
//                   << PreciseGlanceLocation << ","
//                   << EstimatedLocation << ","
//                   << QString::number(currentGlanceTime / 1000.0, 'f', 3) << ","
//                   << QString::number(KA_Buffer_Val_float / 1000.0, 'f', 3) << ","
//                   << resolutionX << "," << resolutionY << "," << resolutionZ
//                   << Qt::endl;
            return; // do nothing
        }
    }

    m_liveAttenDSum   += (KA_Buffer_Val_float / 1000.0);
    m_liveAttenDCount += 1;

    // Add sample to buffer
    GlanceSample sample;
    sample.elapsedTime = elapsedTime;
    sample.label = filteredGlanceLocation;
    if (sample.label == "headupdisplay") sample.label = "road";
    sample.epochTime = posixTime;
    glanceSampleBuffer.append(sample);

    // Write interpolated AHEAD data
    writeInterpolatedAHEAD();
}

void eyeDataInput::writeInterpolatedAHEAD()
{
    if (glanceSampleBuffer.isEmpty()) return;

    double lastSampleTime = glanceSampleBuffer.last().elapsedTime;
    int maxFrame = static_cast<int>(lastSampleTime / 0.033);

    QFile file10(aheadFilenameTaskChopped);
    if (!file10.open(QIODevice::Append | QIODevice::Text)) return;

    QTextStream stream(&file10);

    // ✅ FIX: Use consistent 33.33ms intervals instead of special "every 3rd frame" logic
    while (frameNumberByTask <= maxFrame) {
        double targetTime = frameNumberByTask * 0.033;  // Consistent 33.33ms intervals

        // Find closest sample to targetTime
        QString closestLabel = glanceSampleBuffer.first().label;
        double minDistance = qAbs(glanceSampleBuffer.first().elapsedTime - targetTime);

        for (const auto& sample : glanceSampleBuffer) {
            double distance = qAbs(sample.elapsedTime - targetTime);
            if (distance < minDistance) {
                minDistance = distance;
                closestLabel = sample.label;
            }
        }

        // Write the frame with 3 decimal precision for time
        stream << frameNumberByTask << ","
               << QString::number(targetTime, 'f', 3) << ","
               << closestLabel << Qt::endl;

        frameNumberByTask++;
    }
}
void eyeDataInput::recievedData(QByteArray eyeData)
{
    timer->stop();                                                              //Stop timer and reset

    timer->start(signalRefreshTimer);                                           //Start timer

    emit signalEyeRefresh("true");                                              //Set interface green

    emit signalEyeLogging("false");                                             //Reset if timer expires

    ULONGLONG posixTime = makeTime();                                           //Create Timestamp

    const char *test = eyeData.data();                                          //Message buffer

    SEWorldIntersectionStruct worldIntersection;                                //Reference to be used in Smart Eye Library

    SEVect3D gazeDirectionVectors;                                              //Reference to be used in Smart Eye Library

    GlanceLocation = "Z";                                                       //Reset Glance Location



    //Gaze direction quality

    if(findDataInPacket(SEFilteredGazeDirectionQ, test, quality))                        //Find Eyelid Position
    {
        //
        //qDebug() << quality/2;
    }




    if(findDataInPacket(SEEyelidOpening, test, lefteye))                        //Find Eyelid Position
    {
        if(lefteye < blinkThreshold)
        {
            Blink = "1";                                                        //Set as blink if below blink threshold
        }
        else
        {
            Blink = "0";                                                        //Set as blink if below blink threshold
        }
    }


    if(findDataInPacket(SEGazeDirection, test, gazeDirectionVectors))           //Find Gaze Direction
    {
        float temp = gazeDirectionVectors.x;                                    //Buffer gaze direction Vectors

        Horiz_Eye_Poz = QString::number(temp);                                  //Turn into QString
    }
    else
    {
        Horiz_Eye_Poz = "NA";                                                   //Change parameter to NA
    }

    //precise

    if(findDataInPacket(SEFilteredClosestWorldIntersection, test, worldIntersection))           //Find Intersection
    {
        QString temp = worldIntersection.objectName.ptr;                                           //Buffer intersection data
        //  ULONG pointx = worldIntersection.worldPoint.x;
         // ULONG pointy = worldIntersection.worldPoint.y;
         // ULONG pointz = worldIntersection.worldPoint.z;
          resolutionX = worldIntersection.objectPoint.x;
          resolutionY = worldIntersection.objectPoint.y;
          resolutionZ = worldIntersection.objectPoint.z;

       // qDebug()<<pointx<<":"<<pointy<<":"<<pointz<<":"<<pointx1<<":"<<pointy1<<":"<<pointz1;
        //temp.remove(0,8);                                                     //Remove superfluos data

        if(temp == "" || temp == " " || temp.isNull() == true)
        {
          PreciseGlanceLocation = "U";                                                 //If No region, set as unknown
        }
        else
        {
          PreciseGlanceLocation = temp;                                                //Else assign correct number
        }

    }

    //estimated

    if(findDataInPacket(SEEstimatedClosestWorldIntersection, test, worldIntersection))           //Find Intersection
    {
        QString temp = worldIntersection.objectName.ptr;                        //Buffer intersection data

        //temp.remove(0,8);                                                     //Remove superfluos data

        if(temp == "" || temp == " " || temp.isNull() == true)
        {
          EstimatedLocation = "U";                                              //If No region, set as unknown
        }
        else
        {
          EstimatedLocation = temp;                                             //Else assign correct number
        }

    }

    //Decide which parameter to use

    if(PreciseGlanceLocation == "U")
    {

        if(EstimatedLocation == "U"){

           CurrentGlanceLocation = "U";

           if(quality > 0.5){

               filteredGlanceLocation = PreviousFilteredGlanceLocation;

           }else{

               if(Blink == "0"){
                   //filteredGlanceLocation = "Low_Quality";
                   filteredGlanceLocation = PreviousFilteredGlanceLocation;
               }
               else
               {
                   filteredGlanceLocation = PreviousFilteredGlanceLocation;
               }
            }

        }
        else{

            CurrentGlanceLocation = EstimatedLocation;

            filteredGlanceLocation = EstimatedLocation;
        }
    }
    else{

        CurrentGlanceLocation = PreciseGlanceLocation;

        filteredGlanceLocation = PreciseGlanceLocation;
    }
    AttentionBuffer(filteredGlanceLocation, posixTime);
    //Filtering Locations

    if(CurrentGlanceLocation != PreviousGlanceLocation)                         //If non change in glance location.....
    {
        posixTimePreviousGlance = posixTime;                                    //Buffer previous glance time

        PreviousGlanceLocation = CurrentGlanceLocation;                         //Provisionally set new previous glance location

        if(PreviousFilteredGlanceLocation != filteredGlanceLocation){

            GlanceCounter++;                                                    //Increment glance counter

            currentGlanceTime = 0;                                              //Current glance time
        }

        PreviousFilteredGlanceLocation = filteredGlanceLocation;
    }

    //Calculate attention buffer metrics

    //AttentionBuffer(CurrentGlanceLocation, posixTime);                          //Set new Buffer Value

    //AttentionBuffer(filteredGlanceLocation, posixTime);                          //Set new Buffer Value

    //Log appropriate data

    if(logData->loggingData == "true" && eyeOnOff == "true" && posixTime != posixTimePrevious)             //Check for new glance locations
    {
        QFile file(fileData->glanceFilename);                                    //Set filename
       if(file.open(QIODevice::Append  | QIODevice::Text)){                      //Setup Filename and open

           QTextStream stream(&file);
           //Open up a write stream
           if (CurrentGlanceLocation == "headupdisplay") CurrentGlanceLocation = "road";
           if (filteredGlanceLocation == "headupdisplay") filteredGlanceLocation = "road";
           if (PreciseGlanceLocation == "headupdisplay") PreciseGlanceLocation = "road";
           if (EstimatedLocation == "headupdisplay") EstimatedLocation = "road";
           if (m_currentTask) {
              pID = m_currentTask->participantID();
              rNum = m_currentTask->repNumber();
           stream << posixTime << ","
                  << m_currentTask->participantID() << ","
                  << m_currentTask->name() << ","
                  << m_currentTask->absoluteTaskNum() << ","
                  << m_currentTask->taskOrder() << "," << m_currentTask->repNumber() << ",";
           stream << m_currentTask->taskComplexity() << "," << m_currentTask->controlTypes() << "," << quality << ","
                  << CurrentGlanceLocation << "," << filteredGlanceLocation << ","
                  << PreciseGlanceLocation << "," << EstimatedLocation << ","
                  << QString::number(currentGlanceTime / 1000.0, 'f', 3) << ","
                  << QString::number(KA_Buffer_Val_float / 1000.0, 'f', 3) << ","
                  << resolutionX << "," << resolutionY << "," << resolutionZ
                  << Qt::endl;
           }else {
               stream << posixTime << ","
                      << pID << ","
                      << "No Task Drive" << ","
                      << "-1"<< ","
                      << "-1" << "," << rNum << ",";
               stream << "-1" << "," << "-1" << "," << quality << ","
                      << CurrentGlanceLocation << "," << filteredGlanceLocation << ","
                      << PreciseGlanceLocation << "," << EstimatedLocation << ","
                      << QString::number(currentGlanceTime / 1000.0, 'f', 3) << ","
                      << QString::number(KA_Buffer_Val_float / 1000.0, 'f', 3) << ","
                      << resolutionX << "," << resolutionY << "," << resolutionZ
                      << Qt::endl;
           }
                  //       "Unix timestamp"      "run Number                  "task condition"           "gaze_quality"           "glance_location"               "Filtered without blinks"
//                  stream << posixTime << "," << logData->runNumber << "," << logData->taskCondition << "," << quality << "," <<   CurrentGlanceLocation << "," << filteredGlanceLocation
//                  //       "Precise_glance_location"          "EstimateGlanceLocation"    "Current Glance Time"        "BUFFER VAL KA"               "BUFFER VAL AV1"                 "BUFFER VAL CogV0"
//                  << "," << PreciseGlanceLocation  << "," << EstimatedLocation << "," << QString::number(currentGlanceTime/1000) <<  "," << KA_Buffer_Val_float << "," << AHEADV1_Buffer_Val_float << "," << CogV0_Buffer_Val_float
//                  //       "Horizontal Eye Pos"  "blink events"     "Live AttenD Value"                             "Average buffer time"
//                  << "," << Horiz_Eye_Poz << "," << Blink << "," << QString::number(KA_Buffer_Val_float/1000) << "," << QString::number(Mean_AttenD) <<","<< resolutionX <<","<< resolutionY << ","<< resolutionZ << "\n";               //write line


           emit signalEyeLogging("true");                                         //Let QML know data is being logged
       }

       //This needs to be every 30 ms

       QFile file2(fileData->aheadFilename);                                       //Set filename

       if(file2.open(QIODevice::Append  | QIODevice::Text)){                       //Setup Filename and open

          QTextStream stream(&file2);                                             //Open up a write stream

          //     "Frame"                  "timestamp from start of run"        "glance_location"
          if (filteredGlanceLocation == "headupdisplay") filteredGlanceLocation = "road";
          stream << frameNumber << "," << QString::number(eye_time_buffer/1000) << "," << filteredGlanceLocation << "\n";              //write line

          frameNumber++;                                                          //Increment Framenumber

       }

    }

    // Setup Glance time parameters
    if(frameNumber>0){

        eye_time_buffer = eye_time_buffer + (posixTime - posixTimePrevious);
    }



    currentGlanceTime = currentGlanceTime + (posixTime - posixTimePrevious);

    // To the road
    if(CurrentGlanceLocation == "road"){
        toTheRoadTime = toTheRoadTime + (posixTime - posixTimePrevious);
        fromTheRoadTime = 0;
    }

    // from the road
    if(CurrentGlanceLocation != "road"){
        toTheRoadTime = 0;
        fromTheRoadTime = fromTheRoadTime + (posixTime - posixTimePrevious);
    }

    // Driver State Calculations

    if(toTheRoadTime > 15000){

        DriverStateCondition = "Absorbed";
    }
    else
    {
        DriverStateCondition = "Managing";
    }

    if(fromTheRoadTime > 3000){

        DriverStateCondition = "Distracted";
    }
    else
    {
        DriverStateCondition = "Managing";
    }


    //Send Data to Terminal Window

    //Emit setTerminalWindowGlance(FilteredGlanceLocation, KA_Buffer_Val_float, lefteye, GlanceCounter, eyequality, Horiz_Eye_Poz, Blink);
    emit setTerminalWindowGlance(PreciseGlanceLocation, EstimatedLocation, filteredGlanceLocation, QString::number(currentGlanceTime/1000), QString::number(GlanceCounter), QString::number(KA_Buffer_Val_float/1000), QString::number(Mean_AttenD), DriverStateCondition, Blink);  //Display on terminal

    //Save last sample time
    posixTimePrevious = posixTime;                                          //Store previous time for next dat input

}




ULONGLONG eyeDataInput::makeTime(){

    FILETIME fileTime;                                                          // The current file time
    ULONGLONG fileTimeNano100, posixTime;                                       // filetime in 100 nanosecond resolution
    SYSTEMTIME systemTime;                                                      // Setup the timestamp

    GetSystemTime( &systemTime );
    SystemTimeToFileTime( &systemTime, &fileTime );
    fileTimeNano100 = (((ULONGLONG) fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime;
    posixTime = fileTimeNano100/10000 - 11644473600000;                                             //to milliseconds and unix windows epoche offset removed

    return posixTime;
}

float eyeDataInput::EyeQualityCheck(ULONGLONG time)                             //Function to check current quality of eye glance data
{
//NEED TO UPDATE - MAYBE USE GAZE QUALITY PARAMETER FROM SMARTEYE

    float temp;

    Q_B[0] = Q_B[1];                                                            //Fill like a shift register
    Q_B[1] = Q_B[2];
    Q_B[2] = Q_B[3];
    Q_B[3] = Q_B[4];
    Q_B[4] = Q_B[5];
    Q_B[5] = Q_B[6];
    Q_B[6] = Q_B[7];
    Q_B[7] = Q_B[8];
    Q_B[8] = Q_B[9];
    Q_B[9] = (time-posixTimePreviousGlance);

    temp = (Q_B[0]+Q_B[1]+Q_B[2]+Q_B[3]+Q_B[4]+Q_B[5]+Q_B[6]+Q_B[7]+Q_B[8]+Q_B[9])/10;              //Calcluate Average
    temp = temp/1000;                                                                               //Restore to second
    //temp = floor(temp*100+0.5)/100;
    return temp;
}

void eyeDataInput::AttentionBuffer(QString GlanceLoc2, ULONGLONG time)                              //Function to calculate attention buffer parameters
{
    ULONGLONG posixTime = time;                                                                     //Create Timestamp

    QString GlanceLoc = GlanceLoc2;                                                                 //Setup temporary glance location buffer
    QString PrevGlanceLoc = "";                                                                     //Setup temporary glance location buffer

    if(eyetrackerBrand == 0)                                                                        //--GLANCE LOCATIONS FOR SE--//
    {
        if(GlanceLoc2 == "U") GlanceLoc = "U";                                                      //"0" = Unknown
        if(GlanceLoc2 == "centrescreen") GlanceLoc = "CS";                                          //"1" = Centre Stack
        if(GlanceLoc2 == "cluster") GlanceLoc = "IC";                                               //"2" = Instrument Cluster
        if(GlanceLoc2 == "left") GlanceLoc = "LM";                                                  //"3" = Left (Mirror)
        //if(GlanceLoc2 == "left") GlanceLoc = "LB";                                                  //"4" = Left Blind Spot
        if(GlanceLoc2 == "rearview_mirror") GlanceLoc = "RV";                                       //"5" = Rear View Mirror
        if(GlanceLoc2 == "right") GlanceLoc = "RM";                                                 //"6" = Right (Mirror)
        //if(GlanceLoc2 == "right") GlanceLoc = "RB";                                                 //"7" = Right Blind Spot
        if(GlanceLoc2 == "road") GlanceLoc = "RC";                                                  //"8" = Road Centre (ahead of driver)
        if(GlanceLoc2 == "road_left") GlanceLoc = "RO";                                              //"9" = Road Other side
        if(GlanceLoc2 == "headupdisplay") GlanceLoc = "RC";                                         //"10" = Head Up Display
        if(GlanceLoc2 == "steeringwheel") GlanceLoc = "SW";                                         //"11" = Steering Wheel


        if(PreviousFilteredGlanceLocation == "U") PrevGlanceLoc = "U";
        if(PreviousFilteredGlanceLocation == "centrescreen") PrevGlanceLoc = "CS";
        if(PreviousFilteredGlanceLocation == "cluster") PrevGlanceLoc = "IC";
        if(PreviousFilteredGlanceLocation == "left") PrevGlanceLoc = "LM";
        //if(PreviousFilteredGlanceLocation == "left") PrevGlanceLoc = "LB";
        if(PreviousFilteredGlanceLocation == "rearview_mirror") PrevGlanceLoc = "RV";
        if(PreviousFilteredGlanceLocation == "right") PrevGlanceLoc = "RM";
        //if(PreviousFilteredGlanceLocation == "right") PrevGlanceLoc = "RB";
        if(PreviousFilteredGlanceLocation == "road") PrevGlanceLoc = "RC";
        if(PreviousFilteredGlanceLocation == "road_left") PrevGlanceLoc = "RO";
        if(PreviousFilteredGlanceLocation == "headupdisplay") PrevGlanceLoc = "RC";
        if(PreviousFilteredGlanceLocation == "steeringwheel") PrevGlanceLoc = "SW";
    }

    //KIRCHER ALSTROM ATTEND BUFFER//////////////////////////////////////////////////////////////////////////

    if(PrevGlanceLoc == GlanceLoc)                                                                  //Timer for length of glances to a specific location
    {
        GlanceTimer = GlanceTimer+(posixTime-posixTimePrevious);                                    //If same glance location increment the timer
    }
    else
    {
        GlanceTimer = 0;                                                                            //If different glance location reset the timer
    }

    if(posixTimePrevious == 0)                                                                      //Calculate new buffer value//
    {}
    else
    {
       if(GlanceLoc == "RC" || GlanceLoc == "RO")                                                   //On Road Behaviour
       {
           if(GlanceTimer > 100)                                                                    //Wait 0.1 seconds before incrementing
           {
                KA_Buffer_Val_float = KA_Buffer_Val_float+((posixTime-posixTimePrevious));          //Increment the buffer
           }
       }
       else if(GlanceLoc == "IC" || GlanceLoc == "LM" || GlanceLoc == "LB" || GlanceLoc == "RM" || GlanceLoc == "RB" || GlanceLoc == "RV" || GlanceLoc == "HU")  //Cluster & Mirror Behaviour
       {
           if(GlanceTimer > 1000)
//                          if((PrevGlanceLoc != "RC" && PrevGlanceLoc != "RO") && GlanceTimer > 1000)
           {
               KA_Buffer_Val_float = KA_Buffer_Val_float-((posixTime-posixTimePrevious));           //Decrement the buffer
           }
           else
           {}                                                                                       //Do nothing
       }
       else if(GlanceLoc == "HU")  //Cluster & Mirror Behaviour
       {
           if((PrevGlanceLoc != "RC" && PrevGlanceLoc != "RO") && GlanceTimer > 2000)
           {
               KA_Buffer_Val_float = KA_Buffer_Val_float-((posixTime-posixTimePrevious));           //Decrement the buffer
           }
           else
           {}                                                                                       //Do nothing
       }
       else if(GlanceLoc == "U")                                                                    //Unknown Behaviour
       {}
       else                                                                                         //Off Road Behaviour
       {
           KA_Buffer_Val_float = KA_Buffer_Val_float-((posixTime-posixTimePrevious));               //Decrement the buffer
       }

       if(KA_Buffer_Val_float>2000)
       {   KA_Buffer_Val_float = 2000;         }                                                    //Reset value if over 2000

       if(KA_Buffer_Val_float<0)
       {   KA_Buffer_Val_float = 0;            }                                                    //Reset value if below 0

    }

        //           qDebug() << BufferArray[9] << ":" << BufferArray[8] << ":" << BufferArray[7]<< ":" << BufferArray[6]<< ":" << BufferArray[5]<< ":" << BufferArray[4];
    //AHEAD BUFFER VERSION 1//////////////////////////////////////////////////////////////////////////

    if(posixTimePrevious == 0)
    {}
    else
    {
       if(GlanceLoc == "RC" || GlanceLoc == "RO")                                                   //On Road Behaviour
       {
           AHEADV1_Buffer_Val_float = AHEADV1_Buffer_Val_float+((posixTime-posixTimePrevious));     //Increment the buffer
       }
       else if(GlanceLoc == "U")                                                                    //Unknown Behaviour
       {}
       else                                                                                         //Off Road Behaviour
       {
           AHEADV1_Buffer_Val_float = AHEADV1_Buffer_Val_float-((posixTime-posixTimePrevious));     //Decrement the buffer
       }

       if(AHEADV1_Buffer_Val_float>2000)
       {   AHEADV1_Buffer_Val_float = 2000;         }                                               //Reset value if over 2000

       if(AHEADV1_Buffer_Val_float<0)
       {   AHEADV1_Buffer_Val_float = 0;            }                                               //Reset value if below 0
    }

    //AHEAD BUFFER COG V0/////////////////////////////////////////////////////////////////////////////

    if(posixTimePrevious == 0)
    {}
    else
    {
       if(GlanceLoc == "RC"|| GlanceLoc == "RO")                                                    //WHAT TO DO WITH ROAD OTHER SIDE
       {
           if(GlanceTimer < 15000)                                                                  //On Road Behaviour
           {
               if(GlanceTimer > 200 || PrevGlanceLoc == "RO")
               {
                    CogV0_Buffer_Val_float = CogV0_Buffer_Val_float+((posixTime-posixTimePrevious)*0.33);    //Increment the buffer
               }
           }
           else
           {
               CogV0_Buffer_Val_float = CogV0_Buffer_Val_float-((posixTime-posixTimePrevious)*0.25);//Decrement the buffer

               if(CogV0_Buffer_Val_float<1000)
               {   CogV0_Buffer_Val_float = 1000;              }                                    //Reset value
           }
       }
       else if(GlanceLoc == "IC" || GlanceLoc == "LM" || GlanceLoc == "LB" || GlanceLoc == "RM" || GlanceLoc == "RB" || GlanceLoc == "RV" || GlanceLoc == "HU")  //Cluster & Mirror Behaviour
       {
           if(GlanceTimer < 1000)
           {
               CogV0_Buffer_Val_float = CogV0_Buffer_Val_float-((posixTime-posixTimePrevious));     //Increment the buffer
           }
       }
       else if(GlanceLoc == "U")                                                                    //Unknown Behaviour
       {
       }
       else                                                                                         //Off Road Behaviour
       {
           if(GlanceTimer > 100)
           {                                                                                        //Wait 0.1 seconds before incrementing
               CogV0_Buffer_Val_float = CogV0_Buffer_Val_float-((posixTime-posixTimePrevious));     //Decrement the buffer
           }
       }

       if(CogV0_Buffer_Val_float>2000)
       {   CogV0_Buffer_Val_float = 2000;           }                                               //Reset value if over 2000

       if(CogV0_Buffer_Val_float<0)
       {   CogV0_Buffer_Val_float = 0;              }                                               //Reset value
    }



    double Val = KA_Buffer_Val_float;

    storeVal.push_back(Val);

    if(storeVal.size() >= 1000)
    {
        double sum =0.0;

        for(const auto& value : storeVal)
        {
            sum+= value/1000;

        }
            average =sum/1000;

            //std::cout<<"mean: "<< average << std::endl;
            Mean_AttenD = average;

            storeVal.erase(storeVal.begin(), storeVal.begin()+1000);
    }
 //   qDebug() << "KA_Buffer_Val_float : " << KA_Buffer_Val_float << " Glance : " << GlanceTimer << " GlanceLoc : " << GlanceLoc << " PrevGlanceLoc : " << PrevGlanceLoc;

}
