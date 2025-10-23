import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: ledwindow
    visible: true
    width:118
    height: 300
    x:5
    y:10
   property int counter: 0
    GroupBox {
        id: ledControl
        anchors.centerIn: parent
        x: 5
        y: 10
        width:118
        height: 310
        title: "  TTSO CONTROL  "
        Button {
            id: pattern0Button
            x: 0
           // y: 160
            width: 100
            height: 30
            text: "Play Music"
            anchors.topMargin: 5
            onClicked: {
               // console.log("OFF")
               tcpServer.send(12, true)
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
            text: "Switch To Radio"
            anchors.top:pattern0Button.bottom
            anchors.topMargin: 5
            onClicked: {
                //console.log("GREEN")
                 tcpServer.send(13, true)
                //ledcontrol.onPattern1BtnClicked()

            }
        }

        Button {

           id: pattern2Button
           x: 0
          // y: 40
           width: 100
           height: 30
           text: "Switch To Spotify"
           anchors.top:pattern1Button.bottom
           anchors.topMargin: 5
           onClicked: {
              // console.log("AMBER")
               tcpServer.send(14, true)
               //ledcontrol.onPattern2BtnClicked()

           }

        }

        Button {

            id: pattern3Button
            x: 0
            //y: 80
            width: 100
            height: 30
            text: "Change Temperature"
            anchors.top:pattern2Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("RED")
                 tcpServer.send(15, true)
               // ledcontrol.onPattern3BtnClicked()
            }

        }

        Button {

            id: pattern4Button
            x: 0
           // y: 120
            width: 100
            height: 30
            text: "Reset Heated Screen"
            anchors.top:pattern3Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("BLUE")
                 tcpServer.send(16, true)
                //ledcontrol.onPattern4BtnClicked()
            }

        }
        Button {

            id: pattern5Button
            x: 0
           // y: 120
            width: 100
            height: 30
            text: "Next Task"
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
            text: "Reset All Task"
            anchors.top:pattern5Button.bottom
            anchors.topMargin: 5
            onClicked: {
               // console.log("PURPLE")
                for(var i=0; i<18; i++){

                 tcpServer.send(i, false)
                }
            }

        }

                Button {

                    id: pattern07Button
                    x: 0
                   // y: 160
                    width: 100
                    height: 30
                    text: "Start Task Sequence"
                    anchors.top:pattern6Button.bottom
                    anchors.topMargin: 5
                    onClicked: {
                       // console.log("CHASE")
                        counter++
                       // ledcontrol.onPattern0BtnClicked()
                    }
                }
                Text {
                    id: countertextt
                    text: "Count: " + counter
                    anchors.top:pattern07Button.bottom
                }
        Column {
            x: 0
            anchors.top:countertextt.bottom
            anchors.topMargin: 5
            Repeater {
                id: taskRepeater
                model: 16
                delegate: CheckBox {
                    id: cb
                    property int taskIndex: index + 1   // Task 1..16
                    text: "Task " + (index + 1)

                    // Reflect Android → Windows updates
                    checked: tcpServer.states[index]

                    // Send Windows → Android when user clicks
                    onClicked: {
                                tcpServer.send(index, checked)
                                if (!checked) {
                                    taskUnchecked(taskIndex)   // user unchecked → trigger End Task
                                }
                            }
                    // Server/Android update → react without causing feedback loop
                            Connections {
                                target: tcpServer
                                onStateUpdated: function(index, checkedState) {
                                            if (index === cb.taskIndex - 1 && !checkedState) {
                                                taskUnchecked(cb.taskIndex)
                                            }
                                        }
                            }
                }
            }


            // Your other buttons can still call tcpServer.send(i, true/false)
        }

    }
    function setTaskChecked(taskNum, state) {
        if (taskNum > 0 && taskNum <= taskRepeater.count) {
                    tcpServer.send(taskNum - 1, state)

                }
            }
    signal taskUnchecked(int taskNum)

}

