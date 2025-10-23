import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: ledwindow
    visible: true
    width:118
    height: 300
    x:5
    y:10
    GroupBox {
        id: ledControl
        anchors.centerIn: parent
        x: 5
        y: 10
        width:118
        height: 310
        title: "  LED CONTROL  "
        Button {
            id: pattern0Button
            x: 0
           // y: 160
            width: 100
            height: 30
            text: "OFF"
            anchors.topMargin: 5
            onClicked: {
               // console.log("OFF")
                setLEDpattern("0")
               // writeDataLabelQML("off")
               // ledcontrol.onPattern0BtnClicked()
            }
        }
        Button {

            id: pattern1Button
            x: 0
            y: 0
            width: 100
            height: 30
            text: "GREEN"
            anchors.top:pattern0Button.bottom
            anchors.topMargin: 5
            onClicked: {
                //console.log("GREEN")
                setLEDpattern("1")
                //ledcontrol.onPattern1BtnClicked()

            }
        }

        Button {

           id: pattern2Button
           x: 0
          // y: 40
           width: 100
           height: 30
           text: "AMBER"
           anchors.top:pattern1Button.bottom
           anchors.topMargin: 5
           onClicked: {
              // console.log("AMBER")
               setLEDpattern("2")
               //ledcontrol.onPattern2BtnClicked()

           }

        }

        Button {

            id: pattern3Button
            x: 0
            //y: 80
            width: 100
            height: 30
            text: "RED"
            anchors.top:pattern2Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("RED")
                setLEDpattern("3")
               // ledcontrol.onPattern3BtnClicked()
            }

        }

        Button {

            id: pattern4Button
            x: 0
           // y: 120
            width: 100
            height: 30
            text: "BLUE"
            anchors.top:pattern3Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("BLUE")
                setLEDpattern("4")
                //ledcontrol.onPattern4BtnClicked()
            }

        }
        Button {

            id: pattern5Button
            x: 0
           // y: 120
            width: 100
            height: 30
            text: "WHITE"
            anchors.top:pattern4Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("WHITE")
                setLEDpattern("5")
                //ledcontrol.onPattern4BtnClicked()
            }

        }
        Button {

            id: pattern6Button
            x: 0
           // y: 120
            width: 100
            height: 30
            text: "PURPLE"
            anchors.top:pattern5Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("PURPLE")
                setLEDpattern("6")
                //ledcontrol.onPattern4BtnClicked()
            }

        }

        Button {

            id: pattern07Button
            x: 0
           // y: 160
            width: 100
            height: 30
            text: "CHASE"
            anchors.top:pattern6Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("CHASE")
                setLEDpattern("7")
               // ledcontrol.onPattern0BtnClicked()
            }
        }
    }
}

