import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DEFINES THE LOGGING CONTROL///////////////////////////////
    id:loggingControlWidget

    //Exposed properties
    property alias startbutton: startLogging
    property bool logOnSimData: false
    property bool logging: false

    onLogOnSimDataChanged: { updateLogOnSimDataQML(logOnSimData)        }
    onLoggingChanged: { updateLoggingStatusQML(logging)                 }

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////
    function reset_vals()
    {
        logOnSimData = false
        logging = false
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR FILENAME WIDGET/////////////////////////////////////////
    GroupBox {
        id: loggingBox
        x: 5
        y: 10
        width: 295
        height: 100

        title: "  DATA LOGGING CONTROL PANEL "

        ButtonGroup { id: logOnSimGroup }

        ////////////////////////////////////////////////////////////////////////
        //GROUP BOX FOR AUTO LOGGING ON SIMULATOR///////////////////////////////
        GroupBox {
            id: autoLog
            x: 0
            y: -5
            width: 125
            height: 68

            title: "SimData Auto Log"

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Trigger the data logging automatically when simulator is started")

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO TURN OFF LOG ON SIM DATA/////////////////////////////
            RadioButton {
                id: logOnSimOff
                x: 5
                y: 10
                ButtonGroup.group: logOnSimGroup
                checked: !logOnSimData
                text: qsTr("OFF")
                enabled: !logging
                z: 3
                onClicked: { logOnSimData = false           }
            }

            ////////////////////////////////////////////////////////////////////////
            //OPTION BUTTON TO TURN ON LOG ON SIM DATA//////////////////////////////
            RadioButton {
                id: logOnSimOn
                x: 60
                y: 10
                ButtonGroup.group: logOnSimGroup
                text: qsTr("ON")
                enabled: !logging
                z: 4
                onClicked: {    logOnSimData = true         }
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //START LOGGING BUTTON//////////////////////////////////////////////////
        Button {
            id: startLogging
            x: 148
            y: 0
            width: 130
            height: 28
            text: qsTr("Start")
            enabled: (!logging && !logOnSimData)

            //SEND PARAMETER TO C++
            onClicked: {
                logging = true
                writeDataLabelQML("Start Of Run")
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //STOP LOGGING BUTTON///////////////////////////////////////////////////
        Button {
            id: stopLogging
            x: 148
            y: 35
            width: 130
            height: 28
            text: qsTr("Stop")
            enabled: (logging && !logOnSimData)

            //SEND PARAMETER TO C++//
            onClicked: {
                logging = false
                writeDataLabelQML("End Of Run")
                expdesignform.runNumberBoxVal = expdesignform.runNumberBoxVal+1
            }
        }
    }
}
