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

    //property int attenDVal: 0

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////

    function reset_vals()
    {

    }

    function setTerminalWindowGlanceProperty2(preciseRegion, estimatedRegion, filteredRegion, glanceTimeVal, glanceCountVal, liveAttenDVal, meanAttenDVal, driverState, blinkVal)
    {
        eyeParams.setProperty(0, "value", preciseRegion)
        eyeParams.setProperty(1, "value", estimatedRegion)
        eyeParams.setProperty(2, "value", filteredRegion)
        eyeParams.setProperty(3, "value", glanceTimeVal)
        eyeParams.setProperty(4, "value", glanceCountVal)
        eyeParams.setProperty(5, "value", liveAttenDVal)
        eyeParams.setProperty(6, "value", meanAttenDVal)
        eyeParams.setProperty(7, "value", driverState)
        eyeParams.setProperty(8, "value", blinkVal)

       // attenDVal=meanAttenDVal;
       // if(attenDVal>0) console.log("v1",attenDVal);
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR EYE TRACKER/////////////////////////////////////////////
    GroupBox {
        id: eyetrackerDataBox
        x: 5
        y: 10
        width: 295
        height: 225
        //flat: true
        title: "  EYE TRACKER DATA"
        //checked: false
        z: -1

        ListModel
        {
            id: eyeParams

            ListElement {
                name: "Precise Region   "
                value: "No Data Available"
            }
            ListElement {
                name: "Estimated Region "
                value: "No Data Available"
            }
            ListElement {
                name: "Filtered Region      "
                value: "No Data Available"
            }
            ListElement {
                name: "Current glance time"
                value: "No Data Available"
            }
            ListElement {
                name: "Number of glances"
                value: "No Data Available"
            }
            ListElement {
                name: "Live AttenD value"
                value: "No Data Available"
            }
            ListElement {
                name: "Mean AttenD value"
                value: "No Data Available"
            }
            ListElement {
                name: "Driver State           "
                value: "No Data Available"
            }
            ListElement {
                name: "Blink                  "
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

//Loader{
//    x: -12
//    y: 163
//    width:300
//    height:300
//    source: "AttenDgraph.qml";
//}

         //BUTTON TO OPEN GRAPH WINDOW
//        Button{
//            id:btn
//            x: 0
//            y: 163
//            height: 25
//            width: 275
//            text:"OPEN GRAPH"
//            onClicked: {
//                var component = Qt.createComponent("qrc:/AttenDgraph.qml");
//                if(component.status === component.Ready) {
//                  var window = component.createObject("root");
//                  if(window !== null){
//                      window.show();
//                  }else{
//                      console.error("Error creating object from component")
//                  }
//                 }
//                else{
//                    console.error("Component not ready");
//                }
//            }
//        }
        //current QT version doesn't support the tableViewColumn
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
