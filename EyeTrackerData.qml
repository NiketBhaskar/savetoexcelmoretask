import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtQuick.Controls 1.4 as OldControls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

Item {  
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DISPLAYS THE EYE TRACKER DATA/////////////////////////////
    id:eyetrackerDataDisplay


    //Exposed properties
    //NEED TO ADD BLINK EYE CLOSURE THRESHOLD

    property alias eyeParamVar: eyeParams
    property string glanceBuffer: ""

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////

    function reset_vals()
    {

    }

    function setTerminalWindowGlanceProperty(filterGlanceVal, glanceTimeVal, lefteyeVal, glanceCountVal, eyeQualityVal, horizEyeScanVal, blinkVal, noDataVal)
    {
        eyeParams.setProperty(0, "value", filterGlanceVal)
        eyeParams.setProperty(1, "value", glanceTimeVal)
        eyeParams.setProperty(2, "value", lefteyeVal)
        eyeParams.setProperty(3, "value", glanceCountVal)
        eyeParams.setProperty(4, "value", eyeQualityVal)
        eyeParams.setProperty(5, "value", horizEyeScanVal)
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR EYE TRACKER/////////////////////////////////////////////
    GroupBox {
        id: eyetrackerDataBox
        x: 5
        y: 10
        width: 295
        height:160
        //flat: true
        title: "  EYE TRACKER DATA"
        //checked: false
        z: -1

        ListModel
        {
            id: eyeParams

            ListElement {
                name: "Filtered Glance Location"
                value: "No Data Available"
            }
            ListElement {
                name: "Glance Time"
                value: "No Data Available"
            }
            ListElement {
                name: "Eye Closure"
                value: "No Data Available"
            }
            ListElement {
                name: "Glance Count"
                value: "No Data Available"
            }
            ListElement {
                name: "Eye Quality"
                value: "No Data Available"
            }
            ListElement {
                name: "Horizontal Eye Scan"
                value: "No Data Available"
            }
            ListElement {
                name: "Glance Log"
                value: "No Data Available"
            }
        }

        ListView {
            width: 290
            height: 200

            boundsBehavior: Flickable.StopAtBounds

            model: eyeParams

            delegate: Rectangle {
                implicitWidth: 260
                implicitHeight: 17
                border.color: "black"
                border.width: 0

                Text {
                    text: " " + name +"\t: " + value
                    font.pointSize: 8
                    font.family: "helvetica"
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        //Newer QT doesn't support the tableViewColumn
//        TableView
//        {
//            x: 0
//            y: 0
//            width: 284
//            height: 137


//            TableViewColumn {
//                role: "name"
//                title: "Variable"
//                width: 142
//            }
//            TableViewColumn {
//                role: "data"
//                title: "Data"
//                width: 142
//            }


//            model: eyeParams
//        }
    }
}
