import QtQuick 2.7
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window
import Qt.labs.qmlmodels 1.0

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DISPLAYS THE TASK DATA////////////////////////////////////
    id:taskDataDisplay

    //Exposed properties
    property alias taskParamsVar: taskParams

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////
    function reset_vals()
    {

    }

    function setTerminalWindowTaskProperty(deviceVal, eventVal)
    {   taskParams.setProperty(0, "info", deviceVal)
        taskParams.setProperty(1, "info", eventVal)
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR TASK DATA///////////////////////////////////////////////
    GroupBox {
        id: taskDataBox
        x: 5
        y: 10
        width: 295
        height:75

        title: "  UI EVENT DATA"

        ListModel
        {
            id: taskParams

            ListElement {
                name: "Task Component"
                info: "No Data Available"
            }
            ListElement {
                name: "Task Events"
                info: "No Data Available"
            }
        }

        ListView {
            width: 290
            height: 200
            boundsBehavior: Flickable.StopAtBounds

            model: taskParams

            delegate: Rectangle {
                implicitWidth: 260
                implicitHeight: 17
                border.color: "black"
                border.width: 0

                Text {
                    text: "  " + name + "  : " + info
                    font.pointSize: 8
                    font.family: "helvetica"
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
