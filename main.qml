import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window
import QtMultimedia

ApplicationWindow {
    id: root
    visible: true
    width: 1240
    height: 1000
    title: qsTr("Data Logger for JLR Simulator Setup v4.4")


    ////////////////////////////////////////////////////////////////////////
    //PROPERTIES////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    property int temp:1

    ////////////////////////////////////////////////////////////////////////
    //SLOTS TO SEND DATA BACK TO C++////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    signal setNewFolderQML(string newfolder)                              //FILEMANAGEMENT.H
    signal updateFilenameQML(string currentfilename, string currentParticipantNum)                      //FILEMANAGEMENT.H
    signal makeDirQML()                                                   //FILEMANAGEMENT.H
    signal launchExecutableQML()                                          //FILEMANAGEMENT.H
    signal closeExecutableQML()                                           //FILEMANAGEMENT.H
    signal updateExeUrlQML(string name, string location)                  //FILEMANAGEMENT.H
    signal updateClusterOnOffQML(string name)                             //FILEMANAGEMENT.H
    signal updateHUDOnOffQML(string name)                                 //FILEMANAGEMENT.H
    signal updateTouchOnOffQML(string name)                               //FILEMANAGEMENT.H
    signal readTestScript(string csvfilename)                             //FILEMANAGEMENT.H
    signal deleteTestScriptItem(string locaton)                           //FILEMANAGEMENT.H
    ////////////////////////////////////////////////////////////////////////
    signal updateDrivingDataOnOffQML(string name)                         //DRIVINGDATAINPUT.H
    signal updatePortSettingsDraQML(string p1, string p2)                 //DRIVINGDATAINPUT.H
    signal sendSTISIMTriggerQML()                                         //DRIVINGDATAINPUT.H
    signal updateLogOnSimDataQML(string logonsimdata)                     //DRIVINGDATAINPUT.H
    signal start_DRA_DataFile(string filenum)                           //DRIVINGDATAINPUT.H
    signal stop_DRA_DataFile()                                          //DRIVINGDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////
    signal updateLoggingStatusQML(string logging)                         //LOGGINGDATATOFILE.H
    signal updateRunNumberQML(string runnumber)                           //LOGGINGDATATOFILE.H
    signal updateTaskConditionQML(string taskcondition)                   //LOGGINGDATATOFILE.H
    signal updateLoggingDataQML(string loggingdata)                       //LOGGINGDATATOFILE.H
    signal writeDataLabelQML(string message)                              //LOGGINGDATATOFILE.H
    ////////////////////////////////////////////////////////////////////////
    signal updateTaskDataOnOffQML(string name)                            //TASKDATAINPUT.H
    signal updatePortSettingsTaskQML(string p1, string p2)                //TASKDATAINPUT.H
    //signal startWriteTaskBuffFile(string filenum)                         //TASKDATAINPUT.H
    //signal stopWriteTaskBuffFile()                                        //TASKDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////
    signal updateEyeTrackingOnOffQML(string name)                         //EYEDATAINPUT.H
    signal updatePortSettingsEyeQML(string p1, string p2)                 //EYEDATAINPUT.H
    signal start_AHEAD_DataFile(string filenum)                           //EYEDATAINPUT.H
    signal stop_AHEAD_DataFile()                                          //EYEDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////
    signal launchUDPQML()                                                 //UDPMESSAGINGOUTPUT.H
    signal closeUDPQML()                                                  //UDPMESSAGINGOUTPUT.H
    signal updateUDPRateQML(string rate)                                  //UDPMESSAGINGOUTPUT.H
    signal sendWarningMessage(string message)                             //UDPMESSAGINGOUTPUT.H
    signal sendADAScontrolMessage(string message)                         //UDPMESSAGINGOUTPUT.H
    signal setADASControlFromSWS(string message)                          //UDPMESSAGINGOUTPUT.H
    ////////////////////////////////////////////////////////////////////////
    signal updateHRBROnOffQML(string name)                                //HRHDATAINPUT.H
    signal updatePortSettingsHRBRQML(string p1, string p2)                //HRHDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////
    signal changeADAState(string message1, string message2)               //ADASCONTROL.H
    //signal changeADAState(string message)

    signal startDemoLoggingQML()
    signal stopDemoLoggingQML()
    signal startExpLoggingQML()
    signal stopExpLoggingQML()
    //////////////////////////////////////////////////////////////////////////
    property bool baselineDone: false
    property bool experiencedDone: false
    property bool specialActive: false   // true while Baseline/Experienced is running

    function emitChangeADAState(mode, setSpeed = "0"){
        changeADAState(mode,setSpeed)
    }

    //function emitChangeADAState(mode){
            //changeADAState(mode)
    //}

    //////////////////////////////////////////////////////////////
    signal changeLEDstate(string pattern) ///LEDS

    function setLEDpattern(pattern){
        //console.log("sim main qml: "+pattern);//working
        changeLEDstate(pattern)
    }

    ////////////////////////////////////////////////////////////////////////
    //FUNCTIONS FROM C++ - SIGNALS//////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    //From FILEMANAGEMENT.H
    ////////////////////////////////////////////////////////////////////////

    //FUNCTION TO SET CURRENT FOLDER LOCATION///////////////////////////////
    function setFolderLocationQML(folderName)
    {   filename.currentfoldernameText = folderName
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO SET CURRENT EXECUTABLE LOCATIONS//////////////////////////
    function currentInterfaceExeLocationsQML(cluster, hud, touch)
    {   userinterfacecontrolpanel.clusterLocation = cluster
        userinterfacecontrolpanel.hudLocation = hud
        userinterfacecontrolpanel.touchLocation = touch
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO DECLARE TEST SCRIPT///////////////////////////////////////
    function sendTestScript(testScript)
    {
        console.log("message",testScript)
    }

    ////////////////////////////////////////////////////////////////////////
    //From DRIVINGDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO DECLARE WHETHER DATA INCOMING/////////////////////////////
    function signalDraRefreshQML(param1)
    {   if(param1 === "true")configuration.logDrivingSimIncoming = true
        else configuration.logDrivingSimIncoming = false
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO SET SYSTEM AS LOGGING/////////////////////////////////////
    function setLoggingOnSimDataQML(loggingStatus)
    {
        logging.logging = loggingStatus

        if(loggingStatus === true) {}
        if(loggingStatus === false)
        {
            expdesignform.runNumberBoxVal = expdesignform.runNumberBoxVal+1
        }
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO UPDATE DRIVING DATA///////////////////////////////////////
    function setTerminalWindowDrivingQML(timeVal, lanePosVal, strAngleDegVal, crashesVal, speedkmVal, throttleVal, brakeVal, distanceVal, closestCarVal, leftBSVal, rightBSVal, indicatorVal, sdlpVal, sdsVal, unixTime)
    {
       simdata.setTerminalWindowDrivingProperty(timeVal, lanePosVal, strAngleDegVal, crashesVal, speedkmVal, throttleVal, brakeVal, distanceVal, closestCarVal, leftBSVal, rightBSVal, indicatorVal, sdlpVal, sdsVal)

       userinterfacecontrolpanel.setUnixTime(unixTime)
    }

    ////////////////////////////////////////////////////////////////////////
    //From LOGGINGDATATOFILE.H
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO SET DRIVING DATA AS LOGGING///////////////////////////////
    function signalDraLoggingQML(param1)
    {   if(param1 === "true")configuration.logDrivingSimLogging = true
        else configuration.logDrivingSimLogging = false
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO SET TASK DATA AS LOGGING//////////////////////////////////
    function signalTaskLoggingQML(param1)
    {   if(param1 === "true")configuration.logTaskEventsLogging = true
        else configuration.logTaskEventsLogging = false
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO SET EYE DATA AS LOGGING///////////////////////////////////
    function signalEyeLoggingQML(param1)
    {   if(param1 === "true") configuration.logEyeTrackerLogging = true
        else configuration.logEyeTrackerLogging= false
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO SET BIO DATA AS LOGGING///////////////////////////////////
    function signalHRBRLoggingQML(param1)
    {   if(param1 === "true")configuration.logHRBRLogging = true
        else configuration.logHRBRLogging = false
    }

    ////////////////////////////////////////////////////////////////////////
    //From TASKDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO SET TASK DATA INCOMING////////////////////////////////////
    function signalTaskRefreshQML(param1)
    {   if(param1 === "true")configuration.logTaskEventsIncoming = true
        else configuration.logTaskEventsIncoming = false
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO UPDATE TASK DATA//////////////////////////////////////////
    function setTerminalWindowTaskQML(deviceVal, eventVal)
    {   taskdata.setTerminalWindowTaskProperty(deviceVal, eventVal)
        dataFromTestScript.setTerminalWindowTaskProperty(deviceVal, eventVal)
    }

    ////////////////////////////////////////////////////////////////////////
    //From EYEDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO UPDATE EYE DATA///////////////////////////////////////////
    function signalEyeRefreshQML(param1)
    {   if(param1 === "true") configuration.logEyeTrackerIncoming = true
        else configuration.logEyeTrackerIncoming = false
    }
    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO UPDATE GLANCE DATA////////////////////////////////////////
    //old tracker
//    function setTerminalWindowGlanceQML(filterGlanceVal, glanceTimeVal, lefteyeVal, glanceCountVal, eyeQualityVal, horizEyeScanVal, blinkVal, noDataVal)
//    {
//        eyetrackerdata.setTerminalWindowGlanceProperty(filterGlanceVal, glanceTimeVal, lefteyeVal, glanceCountVal, eyeQualityVal, horizEyeScanVal, blinkVal, noDataVal)

//    }
    //new tracker
    function setTerminalWindowGlanceQML(preciseRegion, estimatedRegion, preciseRegion, glanceTimeVal, glanceCountVal, liveAttenDVal, meanAttenDVal, driverState, blinkVal)
    {
        eyetrackerdata.setTerminalWindowGlanceProperty2(preciseRegion, estimatedRegion, preciseRegion, glanceTimeVal, glanceCountVal, liveAttenDVal, meanAttenDVal, driverState, blinkVal)
        meanAttenDgraph.setAttenDVal(preciseRegion, estimatedRegion, preciseRegion, glanceTimeVal, glanceCountVal, liveAttenDVal, meanAttenDVal, driverState, blinkVal)
    }

    ////////////////////////////////////////////////////////////////////////
    //From UDPMESSAGINGOUTPUT.H
    ////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////
    //From HRHDATAINPUT.H
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO UPDATE HR DATA////////////////////////////////////////////
    function signalHRBRRefreshQML(param1)
    {   if(param1 === "true") configuration.logHRBRIncoming = true
        else configuration.logHRBRIncoming = false
    }

    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO UPDATE HEART AND BREATHING RATE///////////////////////////
    function setTerminalWindowHRBRQML(heartRateVal,breathingRateVal)
    {
       hrbrdata.setTerminalWindowHRBRProperty(heartRateVal,breathingRateVal)
    }

    ////////////////////////////////////////////////////////////////////////
    //RESET FUNCTION////////////////////////////////////////////////////////
    function reset_vals()
    {
        filename.reset_vals()
        logging.reset_vals()
        configuration.reset_vals()
    }

    ////////////////////////////////////////////////////////////////////////
    //FUNCTION TO UPDATE COM PORT SETTINGS//////////////////////////////////
    function updatePortIDQML(param1,param2,param3,param4,param5,param6,param7,param8)
    {
        eyeIP = param1
        draIP = param2
        taskIP = param3
        hrbrIP = param4

        eyePort = param5
        draPort = param6
        taskPort = param7
        hrbrPort = param8
    }

    ////////////////////////////////////////////////////////////////////////
    //OBJECTS///////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    //MENU BAR OPTIONS//////////////////////////////////////////////////////
    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("&Reset")
                onTriggered: reset_vals()
            }
            MenuItem {                text: qsTr("&Port Settings")
                onTriggered: dialogPortSettings.open()

            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////
    //PARAMETERS FOR IP ADDRESS/////////////////////////////////////////////
    property string eyeIP: "192.168.101.22"
    property string draIP: "192.168.101.22"
    property string taskIP: "192.168.101.22"
    property string hrbrIP: "192.168.101.22"

    property string eyePort: "8080"
    property string draPort: "7070"
    property string taskPort: "6060"
    property string hrbrPort: "5050"

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR MANAGING FILENAME//////////////////////////////////////////
    FilenameWidget{
        id:filename
        x: 5
        y: 5
        width: 600
        height: 100
        enabled: !logging.logging
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR CONTROLLING LOGGING FUNCTION///////////////////////////////
    LoggingConsole{
        id:logging
        x: 610
        y: 5
        visible: (filename.currentfilenameText != "")
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR CONTROLLING LOGGING FUNCTION///////////////////////////////
    MeasureSelector{
        id:configuration
        x: 5
        y: 120
        enabled: !logging.logging
        visible: (filename.currentfilenameText != "")
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING SIMULATOR///////////////////////////////////////
    SimulatorData{
        id:simdata
        x: 317
        y: 120
        visible: (filename.currentfilenameText != "" && configuration.logDrivingSim)
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING USER INTERFACE CONTROL PANEL////////////////////
    UserInterfacePanel{
        id:userinterfacecontrolpanel
        x: 5
        y: 410
        enabled: !logging.logging
        visible: (filename.currentfilenameText != "")
    }
    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING TASK DATA///////////////////////////////////////
    TaskData{
        id:taskdata
        x: 610
        y: 120
        visible: (filename.currentfilenameText != "" && configuration.logTaskEvents)

    }


    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING TASK DATA///////////////////////////////////////
    //old file is without _v1
    EyeTrackerData_v1{
        id:eyetrackerdata
        x: 610
        y: 210
        visible: (filename.currentfilenameText != ""&& configuration.logEyeTracker)
        //visible: true
    }
    AttenDgraph{
            id:meanAttenDgraph
            x:915
            y: 5
            width:300
            height:300
            visible: (filename.currentfilenameText != ""&& configuration.logEyeTracker)
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING EXP DESIGN FORM/////////////////////////////////
    ExpDesignForm{
        id:expdesignform
        x: 610
        y: 450
        visible: (filename.currentfilenameText != "")
    }

    // Timer display next to the button
    Text {
        id: timerDisplay
        x: demoExpButton.x + demoExpButton.width + 10
        y: demoExpButton.y + 2
        font.pixelSize: 18
        font.bold: true
        color: elapsedTimer.running ? "#00AA00" : "#666666"
        text: {
            var hours = Math.floor(elapsedSeconds / 3600)
            var minutes = Math.floor((elapsedSeconds % 3600) / 60)
            var seconds = elapsedSeconds % 60

            if (hours > 0) {
                return String(hours).padStart(2, '0') + ":" +
                       String(minutes).padStart(2, '0') + ":" +
                       String(seconds).padStart(2, '0')
            } else {
                return String(minutes).padStart(2, '0') + ":" +
                       String(seconds).padStart(2, '0')
            }
        }
    }

    // Timer logic
    Timer {
        id: elapsedTimer
        interval: 1000 // 1 second
        repeat: true
        running: false
        onTriggered: {
            elapsedSeconds++
        }
    }

    property int elapsedSeconds: 0

    Button {
        id: demoExpButton
        x: 610
        y: 900
        width: 255
        height: 28
        property int phase: 0
        text: phase === 0 ? "Start Demo Drive"
             : phase === 1 ? "End Demo Drive"
             : phase === 2 ? "Start Exp Drive"
             :               "End Exp Drive"
        onClicked: {
            switch (phase) {
            case 0: // Start Demo
                startDemoLoggingQML()
                elapsedSeconds = 0
                elapsedTimer.start()
                phase = 1
                break
            case 1: // End Demo
                stopDemoLoggingQML()
                elapsedTimer.stop()
                phase = 2
                break
            case 2: // Start Exp
                startExpLoggingQML()
                elapsedSeconds = 0
                elapsedTimer.start()
                phase = 3
                break
            case 3: // End Exp
                stopExpLoggingQML()
                elapsedTimer.stop()
                phase = 0
                break
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING TEST SCRIPT/////////////////////////////////////
    TestScript{
        id: dataFromTestScript
        x: 5
        y: 635
        enabled: true
        visible: (filename.currentfilenameText != "")
    }

    Connections {
        target: dataFromTestScript
        onRequestCheckTask: ledControls.setTaskChecked(taskNum, state)
    }
    Connections {
        target: ledControls
        onTaskUnchecked: dataFromTestScript.endTaskBtn.clicked()
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET LED CONTROLS///////////////////////////////////
    TTSO{
        id: ledControls
        x: 1050
        y: 330
        enabled: true
        visible: (filename.currentfilenameText != "")
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING ADAS CONTROLS///////////////////////////////////
    ADAS{
        id: adasControls

        //SIM
        x: 920
        y: 315
       // y: 330

        //HOME
        //x: 200
        //y: 200

        enabled: true //disabled by default, more testing needed
        visible: (filename.currentfilenameText != "")
    }

    ////////////////////////////////////////////////////////////////////////
    //WIDGET FOR DISPLAYING HEART RATE BREATING RATE////////////////////////
    /*
    HRBRData{
        id:hrbrdata
        x: 7
        y: 845
        visible: (filename.currentfilenameText != "" && configuration.logHRBR)
        //visible: true
    }
    */



    ////////////////////////////////////////////////////////////////////////
    //POPUP FOR SETTINGS CONTENT////////////////////////////////////////////
    Dialog {
        id: dialogPortSettings
        title: "Input Port Settings"
        width: 300
        height: 300
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: "Use this dialog to change the COM or UDP Port settings"
            y: 0
        }

        Label {
            text: "IP Address"
            x: 100
            y: 20
        }

        Label {
            text: "Port"
            x: 200
            y: 20
        }

        Label {
            text: "Eye Tracker"
            y: 40
        }

        TextInput {
            id: eyeIPInput
            x: 100
            y: 40
            width: 80
            text: eyeIP
        }

        TextInput {
            id: eyePortInput
            x: 200
            y: 40
            width: 80
            text: eyePort
        }

        Label {
            text: "Driving Data"
            y: 80
        }

        TextInput {
            id: draIPInput
            x: 100
            y: 80
            width: 80
            text: draIP
        }

        TextInput {
            id: draPortInput
            x: 200
            y: 80
            width: 80
            text: draPort
        }

        Label {
            text: "HR-BR Data"
            y: 120
        }

        TextInput {
            id: hrbrIPInput
            x: 100
            y: 120
            width: 80
            text: hrbrIP
        }

        TextInput {
            id: hrbrPortInput
            x: 200
            y: 120
            width: 80
            text: hrbrPort
        }

        Label {
            text: "Task Data"
            y: 160
        }

        TextInput {
            id: taskIPInput
            x: 100
            y: 160
            width: 80
            text: taskIP
        }

        TextInput {
            id: taskPortInput
            x: 200
            y: 160
            width: 80
            text: taskPort
        }



        onAccepted: {
            updatePortSettingsEyeQML(eyeIPInput.text, eyePortInput.text)
            updatePortSettingsDraQML(draIPInput.text, draPortInput.text)
            updatePortSettingsHRBRQML(hrbrIPInput.text, hrbrPortInput.text)
            updatePortSettingsTaskQML(taskIPInput.text, taskPortInput.text)
        }

        onRejected: {} //Do nothing
    }

}
