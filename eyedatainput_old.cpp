#include "eyedatainput_old.h"
#include <QFile>
#include <QTextStream>
#include <ctime>
#include <iostream>
#include <time.h>
#include <QDateTime>
#include "windows.h"
#include "loggingdatatofile.h"
#include "filemanagement.h"
#include <QVariant>

#include "include_SE/SEDataTypes.h"
#include "include_SE/SEPacketAPI.h"
#include "include_SE/SEOutputDataIds.h"
#include "include_SE/SEErrors.h"
#include "include_SE/SETrackerStates.h"
#include "include_SE/TimeService.h"

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

    float lefteye = 0;                                                          //Eye Closure Value

    float eyeLogging = 0;                                                       //Has the data been logged

    //--GLANCE LOCATIONS FOR SE--//
    //"X" = No Valid Data sent
    //"B" = During Blink
    //" " = No Region or not tracking
    //"0" = Unknown
    //"1" = Centre Stack
    //"2" = Instrument Cluster
    //"3" = Left (Mirror)
    //"4" = Left Blind Spot
    //"5" = Rear View Mirror
    //"6" = Right (Mirror)
    //"7" = Right Blind Spot
    //"8" = Road Centre (ahead of driver)
    //"9" = Road Other side
    //SEGazeDirection = 0x0021,
    //SEGazeDirectionQ = 0x0022,

    if(eyetrackerBrand == 0)                                                    //Check Eye Tracker Brand = SmartEye
    {
        if(findDataInPacket(SEEyelidOpening, test, lefteye))                    //Find Eyelid Position
        {
            if(lefteye < blinkThreshold)
            {
                Blink = "1";                                                    //Set as blink if below blink threshold
            }
            else
            {
                Blink = "0";                                                    //Set as blink if below blink threshold
            }
        }

        if(findDataInPacket(SEGazeDirection, test, gazeDirectionVectors))       //Find Gaze Direction
        {
            float temp = gazeDirectionVectors.x;                                //Buffer gaze direction Vectors

            Horiz_Eye_Poz = QString::number(temp);                              //Turn into QString
        }
        else
        {
            Horiz_Eye_Poz = "NA";                                               //Change parameter to NA
        }

        if(findDataInPacket(SEFilteredClosestWorldIntersection, test, worldIntersection))           //Find Intersection
        {
            QString temp = worldIntersection.objectName.ptr;                    //Buffer intersection data

            //temp.remove(0,8);                                                   //Remove superfluos data

            if(temp == "" || temp == " " || temp.isNull() == true)
            {
              GlanceLocation = "U";                                             //If No region, set as unknown
            }
            else
            {
              GlanceLocation = temp;                                            //Else assign correct number
            }


        }


       // if(findDataInPacket(SEEstimatedClosestWorldIntersection, test, worldIntersection))           //Find Intersection
       // {
       //     QString temp = worldIntersection.objectName.ptr;                    //Buffer intersection data

            //temp.remove(0,8);                                                   //Remove superfluos data

         //   if(temp == "" || temp == " " || temp.isNull() == true)
         //   {
         //     GlanceLocation = "U";                                             //If No region, set as unknown
         //   }
         //   else
         //   {
         //     GlanceLocation = temp;                                            //Else assign correct number
         //   }


        //}


        if(GlanceLocation != PreviousGlanceLocation)                            //If non change in glance location.....
        {
            GlanceCounter++;                                                    //Increment glance counter

            eyequality = EyeQualityCheck(posixTime);                            //Test Eye Quality

            posixTimePreviousGlance = posixTime;                                //Buffer previous glance time

            PreviousGlanceLocation = GlanceLocation;                            //Provisionally set new previous glance location
        }

        if((posixTime - posixTimePreviousGlance) > ShortGlanceThreshold)        //If not a short glance
        {
            FilteredGlanceLocation = GlanceLocation;                            //Pas to new glance value
        }

        AttentionBuffer(FilteredGlanceLocation, posixTime);                     //Set new Buffer Value

    }

    if(logData->loggingData == "true" && eyeOnOff == "true" && posixTime != posixTimePrevious)             //Check for new glance locations
    {
        QFile file(fileData->glanceFilename);                                    //Set filename

       if(file.open(QIODevice::Append  | QIODevice::Text)){                     //Setup Filename and open

           QTextStream stream(&file);                                           //Open up a write stream

           //     "Unix timestamp"      "run Number                  "task condition"                "gaze_quality"       "glance_location"
           stream << posixTime << "," << logData->runNumber << "," << logData->taskCondition << "," << eyequality << "," << GlanceLocation
                  //       "filtered_glance_location"        "BUFFER VAL KA"               "BUFFER VAL AV1"                 "BUFFER VAL CogV0"
                  << "," << FilteredGlanceLocation  << "," << KA_Buffer_Val_float << "," << AHEADV1_Buffer_Val_float << "," << CogV0_Buffer_Val_float
                  //       "Horizontal Eye Pos"    "blink events"
                  << "," << Horiz_Eye_Poz << "," << Blink << "\n";              //write line

           emit signalEyeLogging("true");                                       //Let QML know data is being logged
       }


      QFile file2(fileData->aheadFilename);                                    //Set filename

      if(file2.open(QIODevice::Append  | QIODevice::Text)){                    //Setup Filename and open

          QTextStream stream(&file2);                                           //Open up a write stream

          //     "Frame"                  "timestamp from start of run"        "glance_location"
          stream << frameNumber << "," << mSecsStart << "," << FilteredGlanceLocation << "\n";              //write line

          frameNumber++;                                                       //Increment Framenumber



          mSecsStart = mSecsStart + (posixTime - posixTimePrevious);
          //DOESN'T START FROM LOW VALUE(0-17) BECAUSE IT STORES POSIXTIMEAND KEEPS ADDING TO THAT.

          qDebug() << mSecsStart;
       }

    }

    //Send Data to Terminal Window
    //emit setTerminalWindowGlance(FilteredGlanceLocation, KA_Buffer_Val_float, lefteye, GlanceCounter, eyequality, Horiz_Eye_Poz, Blink);              //Display on terminal
    emit setTerminalWindowGlance(FilteredGlanceLocation, QString::number(posixTime - posixTimePrevious), QString::number(lefteye), QString::number(GlanceCounter), QString::number(eyequality), Horiz_Eye_Poz, Blink, "No Data");              //Display on terminal

    //Save last sample time
    posixTimePrevious = posixTime;                                              //Store previous time for next dat input



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

    QString GlanceLoc = "";                                                                         //Setup temporary glance location buffer
    QString PrevGlanceLoc = "";                                                                     //Setup temporary glance location buffer

    if(eyetrackerBrand == 0)                                                                        //--GLANCE LOCATIONS FOR SE--//
    {
        //"X" = No Valid Data sent
        //"B" = During Blink
        //" " = No Region or not tracking

        if(GlanceLoc2 == "0") GlanceLoc = "U";                                                      //"0" = Unknown
        if(GlanceLoc2 == "1") GlanceLoc = "CS";                                                     //"1" = Centre Stack
        if(GlanceLoc2 == "2") GlanceLoc = "IC";                                                     //"2" = Instrument Cluster
        if(GlanceLoc2 == "3") GlanceLoc = "LM";                                                     //"3" = Left (Mirror)
        if(GlanceLoc2 == "4") GlanceLoc = "LB";                                                     //"4" = Left Blind Spot
        if(GlanceLoc2 == "5") GlanceLoc = "RV";                                                     //"5" = Rear View Mirror
        if(GlanceLoc2 == "6") GlanceLoc = "RM";                                                     //"6" = Right (Mirror)
        if(GlanceLoc2 == "7") GlanceLoc = "RB";                                                     //"7" = Right Blind Spot
        if(GlanceLoc2 == "8") GlanceLoc = "RC";                                                     //"8" = Road Centre (ahead of driver)
        if(GlanceLoc2 == "9") GlanceLoc = "RO";                                                     //"9" = Road Other side

        if(PreviousGlanceLocation == "0") PrevGlanceLoc = "U";
        if(PreviousGlanceLocation == "1") PrevGlanceLoc = "CS";
        if(PreviousGlanceLocation == "2") PrevGlanceLoc = "IC";
        if(PreviousGlanceLocation == "3") PrevGlanceLoc = "LM";
        if(PreviousGlanceLocation == "4") PrevGlanceLoc = "LB";
        if(PreviousGlanceLocation == "5") PrevGlanceLoc = "RV";
        if(PreviousGlanceLocation == "6") PrevGlanceLoc = "RM";
        if(PreviousGlanceLocation == "7") PrevGlanceLoc = "RB";
        if(PreviousGlanceLocation == "8") PrevGlanceLoc = "RC";
        if(PreviousGlanceLocation == "9") PrevGlanceLoc = "RO";
    }

    if(eyetrackerBrand == 1)                                                                        //--GLANCE LOCATIONS FOR SM--//
    {
        if(GlanceLoc2 == "1")  GlanceLoc = "U";                                                     //"1" = Unknown
        if(GlanceLoc2 == "10") GlanceLoc = "CS";                                                    //"10" = Centre Stack
        if(GlanceLoc2 == "20") GlanceLoc = "IC";                                                    //"20" = Instrument Cluster
        if(GlanceLoc2 == "30") GlanceLoc = "LM";                                                    //"30" = Left (Mirror)
        if(GlanceLoc2 == "40") GlanceLoc = "LB";                                                    //"40" = Left Blind Spot
        if(GlanceLoc2 == "50") GlanceLoc = "RV";                                                    //"50" = Rear View Mirror
        if(GlanceLoc2 == "60") GlanceLoc = "RM";                                                    //"60" = Right (Mirror)
        if(GlanceLoc2 == "70") GlanceLoc = "RB";                                                    //"70" = Right Blind Spot
        if(GlanceLoc2 == "80") GlanceLoc = "RC";                                                    //"80" = Road Centre (ahead of driver)
        if(GlanceLoc2 == "81") GlanceLoc = "RC";                                                    //"81" = HUD (ahead of driver)
        if(GlanceLoc2 == "90") GlanceLoc = "RO";                                                    //"90" = Road Other side

        if(PreviousGlanceLocation == "1") PrevGlanceLoc = "U";
        if(PreviousGlanceLocation == "10") PrevGlanceLoc = "CS";
        if(PreviousGlanceLocation == "20") PrevGlanceLoc = "IC";
        if(PreviousGlanceLocation == "30") PrevGlanceLoc = "LM";
        if(PreviousGlanceLocation == "40") PrevGlanceLoc = "LB";
        if(PreviousGlanceLocation == "50") PrevGlanceLoc = "RV";
        if(PreviousGlanceLocation == "60") PrevGlanceLoc = "RM";
        if(PreviousGlanceLocation == "70") PrevGlanceLoc = "RB";
        if(PreviousGlanceLocation == "80") PrevGlanceLoc = "RC";
        if(PreviousGlanceLocation == "81") PrevGlanceLoc = "RC";
        if(PreviousGlanceLocation == "90") PrevGlanceLoc = "RO";
    }

    //KIRCHER ALSTROM BUFFER//////////////////////////////////////////////////////////////////////////

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
       else if(GlanceLoc == "IC" || GlanceLoc == "LM" || GlanceLoc == "LB" || GlanceLoc == "RM" || GlanceLoc == "RB" || GlanceLoc == "RV")  //Cluster & Mirror Behaviour
       {
           if((PrevGlanceLoc != "RC" && PrevGlanceLoc != "RO") || GlanceTimer > 1000)
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
       else if(GlanceLoc == "IC" || GlanceLoc == "LM" || GlanceLoc == "LB" || GlanceLoc == "RM" || GlanceLoc == "RB" || GlanceLoc == "RV")  //Cluster & Mirror Behaviour
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
}
