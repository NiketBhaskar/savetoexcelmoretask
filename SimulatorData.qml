import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window
import Qt.labs.qmlmodels 1.0

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DISPLAYS THE SIMULATOR DATA///////////////////////////////
    id:simulatorDataDisplay

    //Exposed properties
    property alias simParamsVar: simParams

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////
    function reset_vals()
    {

    }

    function setTerminalWindowDrivingProperty(timeVal, lanePosVal, strAngleDegVal, crashesVal, speedkmVal, throttleVal, brakeVal, distanceVal, closestCarVal, leftBSVal, rightBSVal, indicatorVal, sdlpVal, sdsVal){
        simParams.setProperty(0, "info", timeVal)
        simParams.setProperty(1, "info", lanePosVal)
        simParams.setProperty(2, "info", strAngleDegVal)
        simParams.setProperty(3, "info", crashesVal)
        simParams.setProperty(4, "info", speedkmVal)
        simParams.setProperty(5, "info", throttleVal)
        simParams.setProperty(6, "info", brakeVal)
        simParams.setProperty(7, "info", distanceVal)
        simParams.setProperty(8, "info", closestCarVal)
        simParams.setProperty(9, "info", leftBSVal)
        simParams.setProperty(10, "info", rightBSVal)
        simParams.setProperty(11, "info", indicatorVal)
        simParams.setProperty(12, "info", sdlpVal.toString())
        simParams.setProperty(13, "info", sdsVal.toString())
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR QUESTIONNAIRES//////////////////////////////////////////
    GroupBox {
        id: simulatorDataBox
        x: 5
        y: 10
        width: 280
        height:275
        title: "  DRIVING SIMULATOR DATA  "

        ListModel {
            id: simParams

            ListElement {
                name: "Run Time"
                info: "No Data Available"
            }
            ListElement {
                name: "Lane Position"
                info: "No Data Available"
            }
            ListElement {
                name: "Steering angle"
                info: "No Data Available"
            }
            ListElement {
                name: "Crashes"
                info: "No Data Available"
            }
            ListElement {
                name: "Speed (KMPH)"
                info: "No Data Available"
            }
            ListElement {
                name: "Throttle"
                info: "No Data Available"
            }
            ListElement {
                name: "Brake"
                info: "No Data Available"
            }
            ListElement {
                name: "Distance"
                info: "No Data Available"
            }
            ListElement {
                name: "Closest Car"
                info: "No Data Available"
            }
            ListElement {
                name: "LH Blind Spot"
                info: "No Data Available"
            }
            ListElement {
                name: "RH Blind Spot"
                info: "No Data Available"
            }
            ListElement {
                name: "Indicator"
                info: "No Data Available"
            }
            ListElement {
                name: "SD - Lane Pos"
                info: "No Data Available"
            }
            ListElement {
                name: "SD - Speed"
                info: "No Data Available"
            }
        }

        ListView {
            width: 270
            height: 275
            boundsBehavior: Flickable.StopAtBounds

            model: simParams

            delegate: Rectangle {
                implicitWidth: 260
                implicitHeight: 17
                border.color: "black"
                border.width: 0

                Text {
                    text: " " + name + "\t\t: " + info
                    font.pointSize: 8
                    font.family: "helvetica"
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
