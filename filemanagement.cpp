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
#include <QVariant>
#include <iostream>

using namespace std;

filemanagement::filemanagement(QObject *parent) : QObject(parent)
{
    //setMyListModel_participant({"P1", "P2", "P3", "P4"});
    //setMyListModel_number({"N1", "N2", "N3", "N4"});
    //setMyListModel_timing({"T1", "T2", "T3", "T4"});
    //setMyListModel_concept({"C1", "C2", "C3", "C4"});
    setMyListModel_audio({"A1", "A2", "A3", "A4"});
    //setMyListModel_scenario({"S1", "S2", "S3", "S4"});
    //setMyListModel_message({"M1", "M2", "M3", "M4"});
}

void filemanagement::initialisation()
{
    //Setup current path of Saved Data Files//////////////////////////

    QFile file(ConfigFileLoc);                                      //Read current folder location

    QString textbuffer1, textbuffer2, textbuffer3, textbuffer4;     //Buffer line

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))           //Open File
    {
        QTextStream in(&file);                                      //Input stream

        textbuffer1 = in.readLine();                                //Read in current file location
        textbuffer2 = in.readLine();                                //Read in current file location
        textbuffer3 = in.readLine();                                //Read in current file location
        textbuffer4 = in.readLine();                                //Read in current file location
    }

    foldername = textbuffer1;                                       //Setup folder location

    emit setFolderLocation(foldername);                             //Send line to QML

    //Setup current path of Saved Data Files//////////////////////////

    clusterUrl = textbuffer2;                                       //Setup location of executables - cluster program
    hudUrl = textbuffer3;                                           //Setup location of executables - HUD program
    touchUrl = textbuffer4;                                         //Setup location of executables - touch program

    emit currentInterfaceExeLocations(clusterUrl, hudUrl, touchUrl);
}

void filemanagement::setNewFolder(QString newfolder)
{
    foldername = newfolder.remove(0, 8);                            //Setup new folder location

    QFile file("C:\\Assets\\ConfigFiles\\SimEyeLogConfig.txt");     //Write back to config file
    //QFile file("C:\\Assets\\ConfigFiles\\SimEyeLogConfig.txt");     //Write back to config file
    //QFile file("F:\\Assets\\ConfigFiles\\SimEyeLogConfig.txt");     //Write back to config file

    if(file.open(QIODevice::Append  | QIODevice::Text)){            //Setup File Header
        file.resize(0);                                             //Resize file
        QTextStream stream(&file);                                  //Open up a write stream
        stream << foldername << "\n";
        stream << clusterUrl << "\n";
        stream << hudUrl << "\n";
        stream << touchUrl << "\n";
    }
}

void filemanagement::updateExeUrl(QString exename, QString exelocation){

    if(exename == "clu")clusterUrl = exelocation;
    if(exename == "hud")hudUrl = exelocation;
    if(exename == "tou")touchUrl = exelocation;

    QFile file("C:\\Assets\\ConfigFiles\\SimEyeLogConfig.txt");     //Write back to config file

    if(file.open(QIODevice::Append  | QIODevice::Text)){            //Setup File Header
        file.resize(0);                                             //Resize file
        QTextStream stream(&file);                                  //Open up a write stream
        stream << foldername << "\n";
        stream << clusterUrl << "\n";
        stream << hudUrl << "\n";
        stream << touchUrl << "\n";
    }
}

void filemanagement::updateFilename(QString currentfilename, QString currentParticipantNum)
{
    filename = currentfilename;                                                                     //Update current filename
    participantNum = currentParticipantNum;
}

void filemanagement::makeDir()                                                                      //Create a Directory for the results
{
    ULONGLONG posixTime = makeTime();                                                               //Create Timestamp

    posixTimeStart = posixTime;                                                                     //Set Start Time

    fullfolderpath = foldername + "\\" + filename;                                                  //Create participant folder

    QByteArray temp = (fullfolderpath.toStdString().c_str());                                       //Convert to QByteArray

    const char *folder = temp.data();                                                               //Create folder name

    mkdir( + folder);                                                                               //Make Folder

    drivingFilename = fullfolderpath + "/" + filename + "_DRA.csv";                                 //Set Filename for Driving Data
    glanceFilename = fullfolderpath + "/" + filename + "_EYE.csv";                                  //Set Filename for Eye Glance Data
    aheadFilename = fullfolderpath + "/" + filename + "_AHEAD.csv";
    hrbrFilename = fullfolderpath + "/" + filename + "_HRBR.csv";                                   //Set Filename for Heart Rate and Breathing Rate data
    taskFilename = fullfolderpath + "/" + filename + "_UIEvents.csv";                               //Set Filename for task / UI events

    QFile file1(drivingFilename);                                                                   //Create file for Driving Information

    //WRITE DRIVING DATA FILE
    if(file1.open(QIODevice::Append  | QIODevice::Text)){                                           //Setup File Header
        QTextStream stream(&file1);                                                                 //Open up a write stream
        /*stream << "Participant ID: " << filename << ",Start Time: " << posixTime << Qt::endl;       //write line
        stream << "Unix Timestamp" << ",Participant Number" << ",Task Conditons" << ",STISIMTime";
        stream << ",LanePos" << ",StrWhl10" << ",Crashes" << ",Speed" << ",Throttle" << ",Brake";
        stream << ",Distance" << ",Nearest vehicle" << ",Left blind spot" << ",Right blind spot";
        stream << ",Indicator" <<  Qt::endl;       */
        stream << "Participant ID: " << filename << ",Start Time: " << posixTime << Qt::endl;//Write DRT events
        stream << "Unix Timestamp" << ",Participant ID" << ",TaskName" << ",AbsTaskNum" << ",TaskOrder" << ",RepNumber"
               << ",TaskComplexity" << ",Control Types" << ",SimTime"  << ",LanePos" << ",StrWhl" << ",Crashes"
               << ",Speed" << ",Throttle" << ",Brake" << ",DistanceTravelled"
               << ",DistToVehAhead"
               << Qt::endl;
    }

    QFile file2(glanceFilename);                                                                    //Create file for Eye Tracker

//    //WRITE EYE DATA FILE
//    if(file2.open(QIODevice::Append  | QIODevice::Text)){                                           //Setup File Header

//        QTextStream stream(&file2);                                                                 //Open up a write stream
//        stream << "Participant ID: " << filename << ",Start Time: " << posixTime << Qt::endl;        //write line
//        stream << "Unix Timestamp" << ",Run Number" << ",Task Conditons" << ",gaze quality";
//        stream << ",glance_location_id" << ",filtered_glance_location_id" << ",Buffer Val KA";
//        stream << ",Buffer Val Av1" << ",Buffer Val Cogv0" << ",Horizontal Eye Pos";
//        stream << ",Blink event" << Qt::endl;
//     }
    if(file2.open(QIODevice::Append  | QIODevice::Text)){                                           //Setup File Header

        QTextStream stream(&file2);                                                                 //Open up a write stream
        stream << "Participant ID: " << filename << ",Start Time: " << posixTime << Qt::endl;
        stream << "Unix Timestamp" << ",Participant ID" << ",TaskName" << ",AbsTaskNum" << ",TaskOrder" << ",RepNumber"
               << ",TaskComplexity" << ",Control Types" << ",GazeQuality" << ",Current" << ",Filtered"
               << ",Precise" << ",Estimated"
               << ",Current_G_Time(s)" << ",LiveAttenD" << ",X" << ",Y" << ",Z"
               << Qt::endl;
//        stream << "Participant ID: " << filename << ",Start Time: " << posixTime << Qt::endl;        //write line
//        stream << "Unix Timestamp" << ",Participant Number" << ",Task Conditons" << ",gaze quality";
//        stream << ",Current_Location"<<",Filtered_Location"<<",Precise_Location"<<",Estimate_Location" << ",Current_Glance_Time";
//        stream << ",Buffer Val KA"<< ",Buffer Val Av1" << ",Buffer Val Cogv0" << ",Horizontal Eye Pos" << ",Blinks"<<",Buffer val KA/1000" << ",Live_AttenDValue" <<  ",resolutionX" << ",resolutionY" << ",resolutionZ"
//               << Qt::endl;

     }

    QFile file3(hrbrFilename);                                                                      //Create file for Heart Rate / Breathing Rate


    //WRITE HEART DATA FILE
    if(file3.open(QIODevice::Append  | QIODevice::Text)){                                           //Setup File Header
      QTextStream stream(&file3);                                                                   //Open up a write stream
      stream << "Participant ID: " << filename << ",Start Time: " << posixTime << Qt::endl;             //write line
      stream << "Unix Timestamp" << ",Participant Number" << ",Task Conditons";
      stream << ",Heart Rate" << ",Breathing Rate" << Qt::endl;
    }

    QFile file4(taskFilename);                                                                      //Create file for Task Data

    //WRITE TASK DATA FILE
    if(file4.open(QIODevice::Append  | QIODevice::Text)){                                           //Setup File Header
      QTextStream stream(&file4);                                                                   //Open up a write stream
      stream << "Participant ID: " << filename << ",Start Time: " << posixTime << Qt::endl;             //write line
      stream << "Unix Timestamp" << ",Participant ID" << ",TaskName" << ",AbsTaskNum" << ",TaskOrder" << ",RepNumber"
             << ",TaskComplexity" << ",Control Types";
      stream << ",UI Device" << ",UI event" << Qt::endl;
    }

    QFile file5(aheadFilename);                                                                      //Create file for Task Data

    //WRITE TASK DATA FILE
    if(file5.open(QIODevice::Append  | QIODevice::Text)){                                           //Setup File Header
      QTextStream stream(&file5);                                                                   //Open up a write stream
      stream << "frame" << ",TIME" << ",label" << Qt::endl;
    }
}

void filemanagement::updateClusterOnOff(QString Clusterstate){

    ClusterSetOnOff = Clusterstate;
}

void filemanagement::updateHUDOnOff(QString HUDstate){

    HUDSetOnOff = HUDstate;
}

void filemanagement::updateTouchOnOff(QString Touchstate){

    TouchSetOnOff = Touchstate;
}

void filemanagement::launchInterface(){

    if(ClusterSetOnOff == "true")
    {
        const char* test;
        QString buffer;

        //Start Cluster Module
        buffer = "start " + clusterUrl;

        qDebug() << "ClusterURL: " <<clusterUrl;

        QByteArray temp = buffer.toLocal8Bit();

        test = temp.data();
        system(test);

        qDebug() << "Concept 1: " <<test;
    }

    if(HUDSetOnOff == "true")
    {
        const char* test;
        QString buffer;

        //Start Cluster Module
        buffer = "start " + hudUrl;

        QByteArray temp = buffer.toLocal8Bit();

        test = temp.data();
        system(test);

        qDebug() << "Concept 2: " <<test;
    }

    if(TouchSetOnOff == "true")
    {
        const char* test;
        QString buffer;

        //Start Touch Module
        buffer = "start " + touchUrl;

        QByteArray temp = buffer.toLocal8Bit();

        test = temp.data();
        system(test);
    }
}

void filemanagement::closeInterface(){

    if(ClusterSetOnOff == "true")
    {
        const char* test;
        QString buffer;

        //Kill Cluster Module
        buffer = "taskkill /F /IM MessageLoc.exe";

        QByteArray temp = buffer.toLocal8Bit();

        test = temp.data();
        system(test);
    }

    if(HUDSetOnOff == "true")
    {
        const char* test;
        QString buffer;

        //Kill HUD Module
        buffer = "taskkill /F /IM ADASCenter(Infotainment_Carousel).exe";

        QByteArray temp = buffer.toLocal8Bit();

        test = temp.data();
        system(test);
    }

    if(TouchSetOnOff == "true")
    {
        const char* test;
        QString buffer;

        //Kill Touch Module
        buffer = "taskkill /F /IM centrescreen.exe";

        QByteArray temp = buffer.toLocal8Bit();

        test = temp.data();
        system(test);
    }
}

void filemanagement::readTestScript(QString filename){
    pList.clear();
    nList.clear();
    tList.clear();
    cList.clear();
    aList.clear();
    sList.clear();
    mList.clear();

    setMyListModel_participant(pList);
    setMyListModel_number(nList);
    setMyListModel_timing(tList);
    setMyListModel_concept(cList);
    setMyListModel_audio(aList);
    setMyListModel_scenario(sList);
    setMyListModel_message(mList);

    QString filenamebuffer = filename.remove(0, 8);

    if (filename.isEmpty()){
        qDebug() << "No file";
    }

    QFile file(filename);
    QStringList listed;

    int a = 0;
    int b = 0;

    if ( file.open(QIODevice::ReadOnly) ) {
        QString line1;
        QTextStream t( &file );

        do {
            line1 = t.readLine();
            listed.clear();
            listed = line1.split(',');

            if(listed[0] != ""){
                pList.append(listed[0]);
                nList.append(listed[1]);
                tList.append(listed[2]);
                cList.append(listed[3]);
                aList.append(listed[4]);
                sList.append(listed[5]);
                mList.append(listed[6]);
            }

            b++;

        } while (!line1.isNull());

        file.close();
    }

    emit sendTestScript(tList[0]);

    setMyListModel_participant(pList);
    setMyListModel_number(nList);
    setMyListModel_timing(tList);
    setMyListModel_concept(cList);
    setMyListModel_audio(aList);
    setMyListModel_scenario(sList);
    setMyListModel_message(mList);
}

void filemanagement::deleteTestScriptItem(QString location){

    int removalLoc = location.toInt();

    int listLength =  pList.length();

    if(listLength > 0){

        pList.remove(removalLoc);
        nList.remove(removalLoc);
        tList.remove(removalLoc);
        cList.remove(removalLoc);
        aList.remove(removalLoc);
        sList.remove(removalLoc);
        mList.remove(removalLoc);

        setMyListModel_participant(pList);
        setMyListModel_number(nList);
        setMyListModel_timing(tList);
        setMyListModel_concept(cList);
        setMyListModel_audio(aList);
        setMyListModel_scenario(sList);
        setMyListModel_message(mList);
    }
}

ULONGLONG filemanagement::makeTime(){

    SYSTEMTIME systemTime;                                                              // Setup the timestamp
    FILETIME fileTime;                                                                  // the current file time
    ULONGLONG fileTimeNano100, posixTime;                                               // filetime in 100 nanosecond resolution

    GetSystemTime( &systemTime );
    SystemTimeToFileTime( &systemTime, &fileTime );
    fileTimeNano100 = (((ULONGLONG) fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime;
    posixTime = fileTimeNano100/10000 - 11644473600000;                                  //to milliseconds and unix windows epoche offset removed

    return posixTime;
   // qDebug()<<posixTime;
}
