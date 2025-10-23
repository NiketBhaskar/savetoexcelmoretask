#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include <QObject>
#include <QDebug>
#include "windows.h"
#include <QVariant>

class filemanagement : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList myListModel_participant READ myListModel_participant WRITE setMyListModel_participant NOTIFY myListModel_participantChanged)
    Q_PROPERTY(QStringList myListModel_number READ myListModel_number WRITE setMyListModel_number NOTIFY myListModel_numberChanged)
    Q_PROPERTY(QStringList myListModel_timing READ myListModel_timing WRITE setMyListModel_timing NOTIFY myListModel_timingChanged)
    Q_PROPERTY(QStringList myListModel_concept READ myListModel_concept WRITE setMyListModel_concept NOTIFY myListModel_conceptChanged)
    Q_PROPERTY(QStringList myListModel_audio READ myListModel_audio WRITE setMyListModel_audio NOTIFY myListModel_audioChanged)
    Q_PROPERTY(QStringList myListModel_scenario READ myListModel_scenario WRITE setMyListModel_scenario NOTIFY myListModel_scenarioChanged)
    Q_PROPERTY(QStringList myListModel_message READ myListModel_message WRITE setMyListModel_message NOTIFY myListModel_messageChanged)

    public:
        explicit filemanagement(QObject *parent = 0);

        ULONGLONG makeTime();                                                   //Function for creating a UNIX timestamp

        QString foldername = "";                                                //Current stored folder path
        QString fullfolderpath = "";                                            //Current path including folder and file
        QString filename = "";                                                  //Current specified filename
        QString participantNum = "";
        QString currentFilePath = "";                                           //Current file path for questionnaires
        QString clusterUrl = "";                                                //Current executable file path for cluster
        QString hudUrl = "";                                                    //Current executable file path for HUD
        QString touchUrl = "";                                                  //Current executable file path for Touchscreen
        QString ClusterSetOnOff = "false";                                      //Cluster activated or not?
        QString HUDSetOnOff= "false";                                           //HUD activated or not?
        QString TouchSetOnOff = "false";                                        //Touchscreen activated or not?
        //QString ConfigFileLoc = "F:\\Assets\\ConfigFiles\\SimEyeLogConfig.txt"; //Config file location
        QString ConfigFileLoc = "C:\\Assets\\ConfigFiles\\SimEyeLogConfig.txt"; //Config file location
        //QString ConfigFileLoc = "D:\\Assets\\ConfigFiles\\SimEyeLogConfig.txt"; //Config file location
        QString drivingFilename;                                                //Driving Data filename
        QString taskFilename;                                                   //Task Data filename
        QString glanceFilename;                                                 //Glance Data filename
        QString aheadFilename;                                                  //AHEAD buffer data file
        QString hrbrFilename;                                                   //Bio Data filename
        //TASK SCRIPT DATA BUFFERS
        QStringList pList;                                                      //Participant
        QStringList nList;                                                      //Number
        QStringList tList;                                                      //Time
        QStringList cList;                                                      //Concept
        QStringList aList;                                                      //Audio
        QStringList sList;                                                      //Scenario
        QStringList mList;                                                      //Message

        qint64 posixTimeStart;                                                  //Start time for file

        void initialisation();                                                  //Determines initial path for the file structure

        QStringList myListModel_participant() const
        {  return m_myListModel_participant;        }

        QStringList myListModel_number() const
        {  return m_myListModel_number;             }

        QStringList myListModel_timing() const
        {  return m_myListModel_timing;             }

        QStringList myListModel_concept() const
        {  return m_myListModel_concept;            }

        QStringList myListModel_audio() const
        {  return m_myListModel_audio;              }

        QStringList myListModel_scenario() const
        {  return m_myListModel_scenario;           }

        QStringList myListModel_message() const
        {  return m_myListModel_message;            }

    signals:
        void setFolderLocation(QVariant foldername);                                //Send current file location to Qt
        void currentInterfaceExeLocations (QVariant cluster, QVariant hud, QVariant touch); //Send current file locations to Qt
        void sendTestScript(QVariant filename);                                     //Send test script filename to Qt
        void myListModel_participantChanged(QStringList myListModel_participant);   //Call function when model changes
        void myListModel_numberChanged(QStringList myListModel_number);             //Call function when model changes
        void myListModel_timingChanged(QStringList myListModel_timing);             //Call function when model changes
        void myListModel_conceptChanged(QStringList myListModel_concept);           //Call function when model changes
        void myListModel_audioChanged(QStringList myListModel_audio);               //Call function when model changes
        void myListModel_scenarioChanged(QStringList myListModel_scenario);         //Call function when model changes
        void myListModel_messageChanged(QStringList myListModel_message);           //Call function when model changes

    public slots:
        void setNewFolder(QString newfolder);                                       //Function to set new Folder location
        void updateFilename(QString currentfilename, QString currentParticipantNum);                               //Function to set new filename
        void makeDir();
        void launchInterface();
        void closeInterface();
        void updateExeUrl(QString exename, QString exelocation);
        void updateClusterOnOff(QString ClusterOnOff);
        void updateHUDOnOff(QString HUDOnOff);
        void updateTouchOnOff (QString TouchOnOff);
        void readTestScript(QString filename);
        void deleteTestScriptItem(QString location);

        void setMyListModel_participant(QStringList myListModel_participant)
        {
            if (m_myListModel_participant == myListModel_participant)
                return;
            m_myListModel_participant = myListModel_participant;
            emit myListModel_participantChanged(m_myListModel_participant);
        }

        void setMyListModel_number(QStringList myListModel_number)
        {
            if (m_myListModel_number == myListModel_number)
                return;
            m_myListModel_number = myListModel_number;
            emit myListModel_numberChanged(m_myListModel_number);
        }

        void setMyListModel_timing(QStringList myListModel_timing)
        {
            if (m_myListModel_timing == myListModel_timing)
                return;
            m_myListModel_timing = myListModel_timing;
            emit myListModel_timingChanged(m_myListModel_timing);
        }

        void setMyListModel_concept(QStringList myListModel_concept)
        {
            if (m_myListModel_concept == myListModel_concept)
                return;
            m_myListModel_concept = myListModel_concept;
            emit myListModel_conceptChanged(m_myListModel_concept);
        }

        void setMyListModel_audio(QStringList myListModel_audio)
        {
            if (m_myListModel_audio == myListModel_audio)
                return;
            m_myListModel_audio = myListModel_audio;
            emit myListModel_audioChanged(m_myListModel_audio);
        }

        void setMyListModel_scenario(QStringList myListModel_scenario)
        {
            if (m_myListModel_scenario == myListModel_scenario)
                return;
            m_myListModel_scenario = myListModel_scenario;
            emit myListModel_scenarioChanged(m_myListModel_scenario);
        }

        void setMyListModel_message(QStringList myListModel_message)
        {
            if (m_myListModel_message == myListModel_message)
                return;
            m_myListModel_message = myListModel_message;
            emit myListModel_messageChanged(m_myListModel_message);
        }

    private:
        QStringList m_myListModel_participant;
        QStringList m_myListModel_number;
        QStringList m_myListModel_timing;
        QStringList m_myListModel_concept;
        QStringList m_myListModel_audio;
        QStringList m_myListModel_scenario;
        QStringList m_myListModel_message;
};

#endif // FILEMANAGEMENT_H
