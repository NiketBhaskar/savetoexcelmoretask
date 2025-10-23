import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DISPLAYS THE EYE TRACKER DATA/////////////////////////////
    id:adasControlsDisplay

    //Exposed properties


    ////////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR EXEPRIMENTAL DESIGN ////////////////////////////////////////
    GroupBox {
        id: adasControlBox
        x: 5
        y: 10
        width:118
        height:375
        title: "  ADAS PANEL  "

        ////////////////////////////////////////////////////////////////////////
        //ADAS MODE BUTTONS/////////////////////////////////////////////////////

        Text {
            id: adasModesLabel
            x: 0
            y: 0
            width: 100
            height: 15
            text: qsTr("ADAS Modes")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
            font.bold: true
            wrapMode: Text.WordWrap
        }

        Button {
            id: modeManual
            anchors.topMargin: 5
            anchors.top: adasModesLabel.bottom
            width: 100
            height: 28
            text: qsTr("Manual Mode")
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                emitChangeADAState("0")
            }
        }

        Button {
            id: modeSPDLim
            anchors.topMargin: 5
            anchors.top: modeManual.bottom
            width: 100
            height: 28
            text: qsTr("Speed Limiter")
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //Limiter
                emitChangeADAState("1","40") //Set Speed to 70 mph
            }
        }

        Button {
            id: modeCC
            anchors.topMargin: 5
            anchors.top: modeSPDLim.bottom
            width: 100
            height: 28
            text: qsTr("Cruise Control")
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //Cruise Control
                emitChangeADAState("2","50")
            }
        }

        Button {
            id: turnACC
            anchors.topMargin: 5
            anchors.top: modeCC.bottom
            width: 100
            height: 28
            text: qsTr("ACC")
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //ACC Set Speed = 70
                emitChangeADAState("3","70")
            }
        }

        Button {
            id: turnACCLKON
            anchors.topMargin: 5
            anchors.top: turnACC.bottom
            width: 100
            height: 28
            text: qsTr("L2 ON")
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //ACC + SA SetSpeed = 50
                emitChangeADAState("4","50")
            }
        }

        Text {
            id: adasControlsLabel
            anchors.topMargin: 10
            anchors.top: turnACCLKON.bottom
            width: 100
            height: 15
            text: qsTr("ADAS Controls")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
            font.bold: true
            wrapMode: Text.WordWrap
        }

        Button {
            id: setPlus
            anchors.topMargin: 5
            anchors.top: adasControlsLabel.bottom
            width: 47
            height: 28
            text: qsTr("SET +")
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //Set +
                emitChangeADAState("5")
            }
        }

        Button {
            id: setMin
            x: 51
            anchors.topMargin: 5
            anchors.top: adasControlsLabel.bottom
            width: 47
            height: 28
            text: qsTr("SET -")
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //Set -
                emitChangeADAState("6")
            }
        }

        Button {
            id: laneChangeRight
            anchors.topMargin: 5
            anchors.top: setPlus.bottom
            width: 100
            height: 28
            text: qsTr("Lane Change Right")
            font.pointSize: 8
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //Lane Change Right
                emitChangeADAState("7")
            }
        }

        Button {
            id: laneChangeLeft
            anchors.topMargin: 5
            anchors.top: laneChangeRight.bottom
            width: 100
            height: 28
            text: qsTr("Lane Change Left")
            font.pointSize: 8
            enabled: logging.logging && !logOnSWSButtons.checkState

            //SEND PARAMETER TO C++
            onClicked: {
                //Lane Change Left
                emitChangeADAState("8")
            }
        }

        CheckBox{
            id: logOnSWSButtons
            anchors.topMargin: 10
            anchors.top: laneChangeLeft.bottom
            text: qsTr("Control from"+"\n"+"Set and Cancel")
            checked: false
            enabled: false
            onCheckStateChanged: {
                if(logOnSWSButtons.checkState) setADASControlFromSWS("On")
                else setADASControlFromSWS("Off")
            }
        }
    }
}

