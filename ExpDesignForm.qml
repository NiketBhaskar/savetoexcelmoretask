import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window
import QtMultimedia 5.15

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DISPLAYS THE EYE TRACKER DATA/////////////////////////////
    id:experimentalDesignDisplay

    //Exposed properties
    property alias runNumberBoxVal: runNumberBox.currentIndex
    property int timerlength: 10000
    property bool armed: false
    property int taskNum: 1


    ////////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR EXEPRIMENTAL DESIGN ////////////////////////////////////////
    GroupBox {
        id: experimentalDesignBox
        x: 5
        y: 10
        width: 295
        height:425
        title: "  EXPERIMENTAL DESIGN  "

        ////////////////////////////////////////////////////////////////////////
        //GROUP BOX FOR DATA////////////////////////////////////////////////////
        GroupBox {
            id: dataLabelBox
            x: 0
            y: 0
            width: 275
            height: 250
            title: "  DATA LABELLING  "

            Text {
                id:runNumberLabel
                x: 0
                y: 10
                text: qsTr("Participant Number")
            }

            Text {
                id: taskConditionLabel
                x: 0
                y: 41
                text: qsTr("Task Condition")
            }

            ComboBox {
                id: taskConditionBox
                x: 93
                y: 35
                width: 50
                height: 20
                enabled: !logging.logging
                model: ["0","1","0","2","0","3","0","4","0","5","0","6","0","7","0","8","0","9","0","10","0","11","0","12","0","13","0","14","0","15","0","16","0","17","0","18","0","19","0","20","0","21","0","22","0","23","0","24","0","25","0","26","0","27","0","28","0","29","0","30","0","31","0","32","0","33","0","34","0","35","0","36","0","37","0","38","0","39","0","40","0","41","0","42","0","43","0","44","0","45","0","46","0","47","0","48","0","49","0","50","0","51","0","52","0","53","0","54","0","55","0","56","0","57","0","58","0","59","0","60","0","61","0","62","0","63","0","64","0","65","0","66","0","67","0","68","0","69","0","70","0","71","0","72","0","73","0","74","0","75","0","76","0","77","0","78","0","79","0","80","0","81","0","82","0","83","0","84","0","85","0","86","0","87","0","88","0","89","0","90","0","91","0","92","0","93","0","94","0","95","0","96","0","97","0","98","0","99","0","100","0","101","0","102","0","103","0","104","0","105","0","106","0","107","0","108","0","109","0","110","0","111","0","112","0","113","0","114","0","115","0","116","0","117","0","118","0","119","0","120"]
                onCurrentIndexChanged:updateTaskConditionQML(taskConditionBox.currentText)
            }

            ComboBox {
                id: runNumberBox
                x: 93
                y: 9
                width: 50
                height: 20
                enabled: True
                model: [ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"]
                onCurrentIndexChanged:updateRunNumberQML(runNumberBox.currentText)
            }

            Button {
                id: simTrigger
                x: 0
                y: 70
                width: 120
                height: 23
                text: qsTr("SIM TRIGGER")
                enabled: logging.logging
                onClicked: sendSTISIMTriggerQML()
            }

            Button {
                id: taskStartLabel
                x: 0
                y: 100
                width: 120
                height: 23
                text: qsTr("TASK START")
                onClicked:{
                    writeDataLabelQML("TASK START")
                    taskConditionBox.currentIndex = taskConditionBox.currentIndex+1
                    //Start AHEAD Collection
                    var task = parseInt(taskConditionBox.currentText, 10);   // convert string → number
                    var tasknum = ((task - 1) % 12) + 1;   // 1–12 looping
                    var rep = Math.floor((task - 1) / 12) + 1; // repetition count
                    var filenameString = tasknum + "_R" + rep;
                    start_AHEAD_DataFile(filenameString);
                    start_DRA_DataFile(filenameString);
         //           startWriteTaskBuffFile(taskConditionBox.currentText)
                    armed = true
//                    switch(taskNum){
//                    case 1:
//                        accInstruction.play()
//                        break;
//                    case 2:
//                        eodInstruction.play()
//                        break;
//                    case 3:
//                        lcInstruction.play()
//                        break;
//                    case 4:
//                        lkaInstruction.play()
//                        break
//                    default:
//                        return;
//                    }
                }
                enabled: logging.logging
            }

            Button {
                id: taskEndLabel
                x: 135
                y: 100
                width: 120
                height: 23
                text: qsTr("TASK END")
                onClicked: {
                    writeDataLabelQML("TASK END")
                    taskConditionBox.currentIndex = taskConditionBox.currentIndex+1
                    //Stop AHEAD collection
                    stop_AHEAD_DataFile()
                    stop_DRA_DataFile("COMPLETE")
               //     stopWriteTaskBuffFile()
                    armed = false

                }
                enabled: logging.logging
            }
            Button {
                id: taskEndPartialLabel
                x: 0
                y: 130
                width: 120
                height: 23
                text: qsTr("TASK END PARTIAL")
                onClicked: {
                    writeDataLabelQML("TASK END PARTIAL")
                    taskConditionBox.currentIndex = taskConditionBox.currentIndex+1
                    //Stop AHEAD collection
                    stop_AHEAD_DataFile()
                    stop_DRA_DataFile("PARTIAL")
               //     stopWriteTaskBuffFile()
                    armed = false

                }
                enabled: logging.logging
            }
            Button {
                id: taskEndIncompleteLabel
                x: 135
                y: 130
                width: 120
                height: 23
                text: qsTr("TASK END INCOMPLETE")
                onClicked: {
                    writeDataLabelQML("TASK END INCOMPLETE")
                    taskConditionBox.currentIndex = taskConditionBox.currentIndex+1
                    //Stop AHEAD collection
                    stop_AHEAD_DataFile()
                    stop_DRA_DataFile("INCOMPLETE")
               //     stopWriteTaskBuffFile()
                    armed = false

                }
                enabled: logging.logging
            }
//            Button {
//                id: drivingOnlyLabel
//                x: 0
//                y: 130
//                width: 120
//                height: 23
//                text: qsTr("DRIVING ONLY")
//                onClicked: writeDataLabelQML("DRIVING ONLY")
//                enabled: logging.logging
//            }

//            Button {
//                id: multitaskingLabel
//                x: 135
//                y: 130
//                width: 120
//                height: 23
//                text: qsTr("MULTITASKING")
//                onClicked: writeDataLabelQML("MULTITASKING")
//                enabled: logging.logging
//            }

            TextField {
                id: manualTextLabel
                x: 0
                y: 160
                width: 255
                height: 23
                placeholderText: qsTr("  Please enter manual label here...  ")
                enabled: logging.logging
            }

            Button {
                id: manualEntryLabel
                x: 0
                y: 190
                width: 255
                height: 23
                text: qsTr("  MANUALLY ENTER LABEL INTO DATA TRACE  ")
                onClicked: writeDataLabelQML(manualTextLabel.text)
                enabled: logging.logging
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //GROUP BOX FOR INTERFACE TRIGGERS//////////////////////////////////////
        GroupBox {
            id: groupBox
            x: 0
            y: 260
            width: 275
            height:130
            title: qsTr("  INTERFACE TRIGGERS  ")

            Button {
                id: buttonSend
                x: 0
                y: 10
                width: 255
                height: 23
                text: qsTr("SEND TEST MESSAGE")
                onClicked: sendWarningMessage("1/True/A/Low Fuel Warning")
            }

            Button {
                id: buttonLoad
                x: 0
                y: 40
                width: 255
                height: 23
                text: qsTr("Load Script")
                onClicked: {
                    fileDialog.open()

                    //readTestScript("F://trial.csv")
                }

                //NEED TO ADD FILE DIALOG ONCE WORKING....
            }

            FileDialog {
                id: fileDialog

                title: "Please choose a test script"

                onAccepted: {
                    console.log(fileDialog.selectedFile)
                    readTestScript(fileDialog.selectedFile)
                }
                onRejected: {

                }

            }

            Button {
                id: buttonPlay
                x: 0
                y: 70
                width: 255
                height: 23
                text: qsTr("Play Script")
                enabled: logging.logging
                onClicked: {
                    deleteTestScriptItem("0")
                    timerlength = parseInt(cppListModel.myListModel_timing[0])
                    messageTimer.start()
                }
            }
//            Column {
//                x: 0
//                anchors.top:countertextt.bottom
//                anchors.topMargin: 5
//                visible: false
//                Repeater {
//                    model: 16
//                    delegate: CheckBox {
//                        id: cb
//                        text: "Task " + (index + 1)

//                        // Reflect Android → Windows updates
//                        checked: tcpServer.states[index]
//                        onCheckedChanged: if(index<12 && checked){
//                                              taskEndLabel.clicked()
//                                          }


//                    }
//                }


//                // Your other buttons can still call tcpServer.send(i, true/false)
//            }
            Timer {
                id: messageTimer
                interval: timerlength
                running: false
                repeat: true
                onTriggered: {
                    sendWarningMessage(cppListModel.myListModel_concept[0] + "/" + cppListModel.myListModel_audio[0] + "/" + cppListModel.myListModel_scenario[0] + "/" + cppListModel.myListModel_message[0])
                    //sendWarningMessage("")
                    writeDataLabelQML("Concept: " + cppListModel.myListModel_concept[0] + " / " + "Audio: " + cppListModel.myListModel_audio[0] + " / " + "Scenario: " + cppListModel.myListModel_scenario[0] + " / " + "Message: " + cppListModel.myListModel_message[0])
                    deleteTestScriptItem("0")
                    //cppListModel.myListModel_participant[0] - Do I Need to set these in the system?
                    //cppListModel.myListModel_number[0] - Do I need to set these in the system?
                    timerlength = parseInt(cppListModel.myListModel_timing[0])

                    if(timerlength <= 0){
                        messageTimer.stop()
                    }
                }
            }
        }
    }
    function triggerSave() {
        taskStartLabel.clicked()  // or just duplicate the logic directly here
    }
    function triggerEnd() {
        taskEndLabel.clicked()  // or just duplicate the logic directly here
    }
    function triggerEndPartial() {
        taskEndPartialLabel.clicked()  // or just duplicate the logic directly here
    }
    function triggerEndIncomplete() {
        taskEndIncompleteLabel.clicked()  // or just duplicate the logic directly here
    }
//    Audio{
//        id: accInstruction
//        source: "audio/ACC.wav"
//    }

//    Audio{
//        id: eodInstruction
//       source: "audio/EoD.wav"
//    }

//    Audio{
//        id: lcInstruction
//        source: "audio/LC.wav"
//    }

//    Audio{
//        id: lkaInstruction
//        source: "audio/LKA.wav"
//    }

//    Audio{
//        id: lcInstruction_new
//        source: "audio/LC_new.wav"
//    }

//    Audio{
//        id: lcInstruction_toCenter
//        source: "audio/LC_ToCenter.wav"
//        volume: 0.5
//    }


//    Audio{
//        id: lkaInstruction_ACCoff
//        source: "audio/ACCoff_LC_new.wav"
//    }
}
