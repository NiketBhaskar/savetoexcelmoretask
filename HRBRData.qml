import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DISPLAYS THE HR/BR DATA///////////////////////////////////
    id:hrbrDataDisplay
    x:0
    y:0
    width: 300
    height: 80

    //Exposed properties

    property alias hrbrParamsVar: hrbrParams

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////

    function reset_vals()
    {

    }

    function setTerminalWindowHRBRProperty(heartRateVal,breathingRateVal)
    {
        hrbrParams.setProperty(0, "data", heartRateVal)
        hrbrParams.setProperty(1, "data", breathingRateVal)
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR HR/BR///////////////////////////////////////////////////
    GroupBox {
        id: hrbrDataBox
        anchors.fill: parent
        title: "  HEART RATE & BREATHING RATE DATA"
        //checked: false
        //flat: true
        z: -1

        ListModel
        {
            id: hrbrParams

            ListElement {
                name: "Heart Rate"
                data: "No Data Available"
            }
            ListElement {
                name: "Breathing Rate"
                data: "No Data Available"
            }
        }

        TableView
        {
            y: 0
            width: 284
            height: 56

            /*
            TableViewColumn {
                role: "name"
                title: "Variable"
                width: 142
            }
            TableViewColumn {
                role: "data"
                title: "Data"
                width: 142
            }
            */

            model: hrbrParams
        }
    }
}
