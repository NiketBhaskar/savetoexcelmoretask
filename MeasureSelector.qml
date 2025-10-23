import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

Item {

    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DEFINES THE LOGGING CONTROL///////////////////////////////
    id:configurationWidget

    //Exposed properties
    property bool logDrivingSim: true
    property bool logTaskEvents: true
    property bool logEyeTracker: true
    property bool logHRBR: false

    property bool logDrivingSimIncoming: false
    property bool logTaskEventsIncoming: false
    property bool logEyeTrackerIncoming: false
    property bool logHRBRIncoming: false

    property bool logDrivingSimLogging: false
    property bool logTaskEventsLogging: false
    property bool logEyeTrackerLogging: false
    property bool logHRBRLogging: false

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////
    function reset_vals()
    {
        logDrivingSim = true
        logTaskEvents = true
        logEyeTracker = true
        logHRBR = false

        logDrivingSimIncoming = false
        logTaskEventsIncoming = false
        logEyeTrackerIncoming = false
        logHRBRIncoming = false

        logDrivingSimLogging = false
        logTaskEventsLogging = false
        logEyeTrackerLogging = false
        logHRBRLogging = false
    }

    ////////////////////////////////////////////////////////////////////////
    //UPDATING FUNCTIONS////////////////////////////////////////////////////

    onLogDrivingSimChanged:{ updateDrivingDataOnOffQML(logDrivingSim)   }
    onLogTaskEventsChanged:{ updateTaskDataOnOffQML(logTaskEvents)      }
    onLogEyeTrackerChanged:{ updateEyeTrackingOnOffQML(logEyeTracker)   }
    onLogHRBRChanged:{ updateHRBROnOffQML(logHRBR)                      }

    onLogDrivingSimIncomingChanged: {
        if(logDrivingSimIncoming)logSimulatorDataIncoming.color = "#00ff00"
        else logSimulatorDataIncoming.color = "#ff0000"
    }
    onLogTaskEventsIncomingChanged: {
        if(logTaskEventsIncoming)logTaskEventsDataIncoming.color = "#00ff00"
        else logTaskEventsDataIncoming.color = "#ff0000"
    }
    onLogEyeTrackerIncomingChanged: {
        if(logEyeTrackerIncoming)logEyeTrackerDataIncoming.color = "#00ff00"
        else logEyeTrackerDataIncoming.color = "#ff0000"
    }
    onLogHRBRIncomingChanged: {
        if(logHRBRIncoming)logHRBRDataIncoming.color = "#00ff00"
        else logHRBRDataIncoming.color = "#ff0000"
    }

    onLogDrivingSimLoggingChanged: {
        if(logDrivingSimLogging)logSimulatorDataLogging.color = "#00ff00"
        else logSimulatorDataLogging.color = "#ff0000"
    }
    onLogTaskEventsLoggingChanged: {
        if(logTaskEventsLogging)logTaskEventsDataLogging.color = "#00ff00"
        else logTaskEventsDataLogging.color = "#ff0000"
    }
    onLogEyeTrackerLoggingChanged: {
        if(logEyeTrackerLogging)logEyeTrackerDataLogging.color = "#00ff00"
        else logEyeTrackerDataLogging.color = "#ff0000"
    }
    onLogHRBRLoggingChanged: {
        if(logHRBRLogging)logHRBRDataLogging.color = "#00ff00"
        else logHRBRDataLogging.color = "#ff0000"
    }

   ////////////////////////////////////////////////////////////////////////
   //GROUP BOX FOR FILENAME WIDGET/////////////////////////////////////////

    GroupBox {
        id: loggingBox
        title: "  CONFIGURATION PANEL  "
        x: 5
        y: 10
        width: 305
        height:275

        ButtonGroup { id:logDrivingSimGroup  }
        ButtonGroup { id:logTaskEventsGroup  }
        ButtonGroup { id:logEyeTrackerGroup  }
        ButtonGroup { id:logHRBRGroup        }

        ////////////////////////////////////////////////////////////////////////
        //BACKGROUND OBJECT/////////////////////////////////////////////////////
        Rectangle {
            id: logSimulatorBackground
            x: 0
            y: 5
            width: 285
            height: 32
            border.color: "#999999"
            z: 1

            ////////////////////////////////////////////////////////////////////////
            //TEXT FIELD FOR LOGGING SIM DATA OPTION////////////////////////////////
            Text {
                id: logSimulatorLabel
                x: 8
                y: 7
                width: 120
                height: 18
                text: qsTr("Log Driving Sim Data")
                verticalAlignment: Text.AlignVCenter
                z: 2
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG SIMULATOR DATA ON////////////////////////////////
            RadioButton {
                id: logSimulatorOn
                x: 177
                y: 8
                text: qsTr("ON")
                ButtonGroup.group: logDrivingSimGroup
                checked: logDrivingSim
                z: 4

                //SEND PARAMETER TO C++
                onClicked: {
                    logDrivingSim = true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG SIMULATOR DATA OFF///////////////////////////////
            RadioButton {
                id: logSimulatorOff
                x: 130
                y: 8
                text: qsTr("OFF")
                ButtonGroup.group: logDrivingSimGroup
                checked: !logDrivingSim
                z: 3

                //SEND PARAMETER TO C++
                onClicked: {
                    logDrivingSim = false
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA INCOMING - EYE TRACKER///////////////////////////////////////////
            Rectangle {
                id: logSimulatorDataIncoming
                x: 229
                y: 7
                width: 19
                height: 18
                color: "#000000"
                z: 1
                border.width: 3

                ToolTip {
                    id: toolTipSimInc
                    text: qsTr("Is there any data coming from the simulator?, R=No, G=Yes")
                    visible: toolTipSlotSimInc.containsMouse
                }

                MouseArea {
                    id: toolTipSlotSimInc
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA LOGGING - EYE TRACKER////////////////////////////////////////////

            Rectangle {
                id: logSimulatorDataLogging
                x: 256
                y: 7
                width: 19
                height: 18
                color: "#000000"
                z: 1
                border.width: 3

                ToolTip {
                    id: toolTipSimLog
                    text: qsTr("Is there any data being logged for the simulator?, R=No, G=Yes")
                    visible: toolTipSlotSimLog.containsMouse
                }

                MouseArea {
                    id: toolTipSlotSimLog
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //BACKGROUND OBJECT/////////////////////////////////////////////////////
        Rectangle {
            id: logTaskBackground
            x: 0
            y: 45
            width: 285
            height: 32
            border.color: "#999999"
            z: 1

            ////////////////////////////////////////////////////////////////////////
            //TEXT FIELD FOR LOG TASK DATA OPTION///////////////////////////////////
            Text {
                id: logTaskDataLabel
                x: 8
                y: 7
                width: 120
                height: 18
                text: qsTr("Log Task Data")
                verticalAlignment: Text.AlignVCenter
                z: 2
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG HRBR DATA ON/////////////////////////////////////
            RadioButton {
                id: logTaskDataOn
                x: 178
                y: 8
                text: qsTr("ON")
                ButtonGroup.group: logTaskEventsGroup
                checked: logTaskEvents
                z: 4

                //SEND PARAMETER TO C++
                onClicked: {
                    logTaskEvents = true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG TASK DATA OFF////////////////////////////////////
            RadioButton {
                id: logTaskDataOff
                x: 130
                y: 8
                text: qsTr("OFF")
                ButtonGroup.group: logTaskEventsGroup
                checked: !logTaskEvents
                z: 3

                //SEND PARAMETER TO C++
                onClicked: {
                    logTaskEvents = false
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA INCOMING - TASK EVENTS///////////////////////////////////////////
            Rectangle {
                id: logTaskEventsDataIncoming
                x: 229
                y: 7
                width: 19
                height: 18
                color: "#000000"
                z: 1
                border.width: 3

                ToolTip {
                    id: toolTipTaskInc
                    text: qsTr("Is there any data coming for task events?, R=No, G=Yes")
                    visible: toolTipSlotTaskInc.containsMouse
                }

                MouseArea {
                    id: toolTipSlotTaskInc
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA LOGGING - TASK EVENTS////////////////////////////////////////////
            Rectangle {
                id: logTaskEventsDataLogging
                x: 256
                y: 7
                width: 19
                height: 18
                color: "#000000"
                z: 1
                border.width: 3

                ToolTip {
                    id: toolTipTaskLog
                    text: qsTr("Is there any data being logged for task events?, R=No, G=Yes")
                    visible: toolTipSlotTaskLog.containsMouse
                }

                MouseArea {
                    id: toolTipSlotTaskLog
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //BACKGROUND OBJECT/////////////////////////////////////////////////////
        Rectangle
        {
            id: logEyetrackerBackground
            x: 0
            y: 85
            width: 285
            height: 32
            border.color: "#999999"
            z: 1

            ////////////////////////////////////////////////////////////////////////
            //TEXT FIELD FOR LOG EYE TRACKER DATA OPTION////////////////////////////
            Text {
                id: logEyeTrackerLabel
                x: 8
                y: 7
                width: 120
                height: 18
                text: qsTr("Log Eye Tracker Data")
                //color: "#999999"//14/11/23
                z: 2
                verticalAlignment: Text.AlignVCenter
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG EYE DATA ON//////////////////////////////////////
            RadioButton {
                id: logEyeTrackerOn
                x: 177
                y: 8
                ButtonGroup.group: logEyeTrackerGroup
                checked: logEyeTracker
                //enabled: false//14/11/23
                text: qsTr("ON")
                z: 4

                //SEND PARAMETER TO C++
                onClicked: {
                    logEyeTracker = true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG EYE DATA OFF/////////////////////////////////////
            RadioButton {
                id: logEyeTrackerOff
                x: 130
                y: 8
                ButtonGroup.group: logEyeTrackerGroup
                checked: !logEyeTracker
                //enabled: false//14/11/23
                text: qsTr("OFF")
                z: 3

                //SEND PARAMETER TO C++
                onClicked: {
                    logEyeTracker = false
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA INCOMING - EYE TRACKER///////////////////////////////////////////
            Rectangle {
                id: logEyeTrackerDataIncoming
                x: 229
                y: 7
                width: 19
                height: 18
                color: "#999999"
                border.width: 3
                z: 1

                ToolTip {
                    id: toolTipEyeInc
                    text: qsTr("Is there any data coming from the eye tracker?, R=No, G=Yes")
                    visible: toolTipSlotEyeInc.containsMouse
                }

                MouseArea {
                    id: toolTipSlotEyeInc
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA LOGGING - EYE TRACKER////////////////////////////////////////////
            Rectangle {
                id: logEyeTrackerDataLogging
                x: 256
                y: 7
                width: 19
                height: 18
                color: "#999999"
                border.width: 3
                z: 1

                ToolTip {
                    id: toolTipEyeLog
                    text: qsTr("Is there any data being logged for the eye tracker?, R=No, G=Yes")
                    visible: toolTipSlotEyeLog.containsMouse
                }

                MouseArea {
                    id: toolTipSlotEyeLog
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //BACKGROUND OBJECT/////////////////////////////////////////////////////
        Rectangle {
            id: logHRBRBackground
            x: 0
            y: 125
            width: 285
            height: 32
            border.color: "#999999"
            z: 1

            ////////////////////////////////////////////////////////////////////////
            //TEXT FIELD FOR LOG HRBR DATA OPTION///////////////////////////////////

            Text {
                id: logHRBRLabel
                x: 8
                y: 7
                width: 120
                height: 18
                text: qsTr("Log HR/BR Data")
                color: "#999999"
                verticalAlignment: Text.AlignVCenter
                z: 2
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG HRBR DATA ON/////////////////////////////////////

            RadioButton {
                id: logHRBROn
                x: 177
                y: 8
                text: qsTr("ON")
                ButtonGroup.group: logHRBRGroup
                enabled: false
                z: 4

                //SEND PARAMETER TO C++
                onClicked: {
                    logHRBR = true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO LOG HRBR DATA OFF////////////////////////////////////

            RadioButton {
                id: logHRBROff
                x: 130
                y: 8
                text: qsTr("OFF")
                ButtonGroup.group: logHRBRGroup
                enabled: false
                checked: !logHRBR
                z: 3

                //SEND PARAMETER TO C++
                onClicked: {
                    logHRBR = false
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA INCOMING - HRBR//////////////////////////////////////////////////

            Rectangle {
                id: logHRBRDataIncoming
                x: 229
                y: 7
                width: 19
                height: 18
                color: "#999999"
                z: 1
                border.width: 3

                ToolTip {
                    id: toolTipHRBRInc
                    text: qsTr("Is there any data coming from the HR/BR sensor?, R=No, G=Yes")
                    visible: toolTipSlotHRBRInc.containsMouse
                }

                MouseArea {
                    id: toolTipSlotHRBRInc
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //DATA LOGGING - HRBR///////////////////////////////////////////////////

            Rectangle {
                id: logHRBRDataLogging
                x: 256
                y: 7
                width: 19
                height: 18
                color: "#999999"
                z: 1
                border.width: 3

                ToolTip {
                    id: toolTipHRBRLog
                    text: qsTr("Is there any data being logged for HR/BR?, R=No, G=Yes")
                    visible: toolTipSlotHRBRLog.containsMouse
                }

                MouseArea {
                    id: toolTipSlotHRBRLog
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }
    }
}
