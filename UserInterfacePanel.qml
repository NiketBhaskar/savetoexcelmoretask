import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DISPLAYS CONTROLS TO TURN ON AND OFF UI///////////////////

    id:uicontrolpanelDisplay

    //Exposed properties
    property var menu_items_name: []
    property var menu_items_lists: []
    property bool questionActiveFlag: false
    property bool udpoutputFlag: false
    property bool interfacesFlag: false
    property bool hudFlag: false
    property bool clusterFlag: false
    property bool touchscreenFlag: false
    property var clusterLocation: ""
    property var hudLocation: ""
    property var touchLocation: ""
    property var udpTimerRate: 10

    onUdpTimerRateChanged: { updateUDPRateQML(udpTimerRate)     }

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////
    function reset_vals()
    {
        questionActiveFlag = false
        udpoutputFlag = false
        interfacesFlag = false
        hudFlag = false
        clusterFlag = false
        touchscreenFlag = false
        clusterLocation = ""
        hudLocation = ""
        touchLocation = ""
    }

    function setUnixTime(unixTime)
    {
        unixStamp.text = unixTime
    }

    ////////////////////////////////////////////////////////////////////////
    //UPDATING FUNCTIONS////////////////////////////////////////////////////
    onQuestionActiveFlagChanged: {}

    onUdpoutputFlagChanged:{
        if(udpoutputFlag)launchUDPQML()
        if(!udpoutputFlag)closeUDPQML()
    }

    onInterfacesFlagChanged:{
        if(interfacesFlag)launchExecutableQML()
        if(!interfacesFlag)closeExecutableQML()
    }

    //onHudFlagChanged: { updateHUDOnOffQML(hudFlag)                      } ORIGINAL VERSION

    //onClusterFlagChanged: { updateClusterOnOffQML(clusterFlag)          } ORIGINAL VERSION

    onHudFlagChanged: {

        if(hudFlag === true && clusterFlag === true){
            clusterFlag = false
        }

        updateHUDOnOffQML(hudFlag)
    }

    onClusterFlagChanged: {

        if(hudFlag === true && clusterFlag === true){
            hudFlag = false
        }

        updateClusterOnOffQML(clusterFlag)
    }

    onTouchscreenFlagChanged: { updateTouchOnOffQML(touchscreenFlag)    }

    onClusterLocationChanged: { updateExeUrlQML("clu", clusterLocation) }

    onHudLocationChanged: { updateExeUrlQML("hud", hudLocation)         }

    onTouchLocationChanged: { updateExeUrlQML("tou", touchLocation)     }

    ////////////////////////////////////////////////////////////////////////
    //FUNCTIONS/////////////////////////////////////////////////////////////

    //Update Questionnaire List/////////////////////////////////////////////
    function updateQuestionList(qList){

        questionListModel.clear()

        var newlist = qList.split(",")

        for(var i = 0; i < newlist.length; i++)
        {
            questionListModel.append({"name": newlist[i]})
        }
    }

    ////////////////////////////////////////////////////////////////////////
    //Update Completed Questionnaire////////////////////////////////////////
    function questionnaireCompleted()
    {
        questionListModel.setProperty(listView.currentIndex,"name",(questionListModel.get(listView.currentIndex).name + "  ✓ - Complete"))
        questionActiveFlag = false
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR USER INTERFACE CONTROLS/////////////////////////////////
    GroupBox {
        id: uicontrolpanelDataBox
        x: 5
        y: 10
        width: 595
        height: 210
        title: qsTr("  USER INTERFACE CONTROL PANEL  ")
        enabled: !questionActiveFlag

        ButtonGroup { id:interfacesOnOffGroup    }
        ButtonGroup { id:clusterOnOffGroup       }
        ButtonGroup { id:touchscreenOnOffGroup   }
        ButtonGroup { id:hudOnOffGroup           }
        ButtonGroup { id:udpOnOffGroup           }

    ////////////////////////////////////////////////////////////////////////
    //THE INTERFACE DATA OUTPUT ON OFF SWITCH///////////////////////////////
        Text {
            id: interfaceOnOffLabel
            x: 6
            y: 89
            width: 130
            height: 15
            text: qsTr("Turn on Interfaces")
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }

        RadioButton {
            id: interfacesOn
            x: 179
            y: 90
            width: 49
            height: 15
            ButtonGroup.group: interfacesOnOffGroup
            text: qsTr("On")
            checked: interfacesFlag
            //enabled: !udpoutputFlag
            enabled: clusterFlag || hudFlag
            onClicked: {
                udpoutputFlag = true
                interfacesFlag = true
            }
        }

        RadioButton {
            id: interfacesOff
            x: 124
            y: 90
            width: 49
            height: 15
            text: qsTr("Off")
            ButtonGroup.group: interfacesOnOffGroup
            checked: !interfacesFlag
            //enabled: !udpoutputFlag
            enabled: clusterFlag || hudFlag
            onClicked: {
                interfacesFlag = false
                udpoutputFlag = false
            }
        }


        RadioButton {
            id: udpOn
            x: 179
            y: 120
            width: 49
            height: 15
            ButtonGroup.group: udpOnOffGroup
            text: qsTr("On")
            checked: udpFlag
            visible:false
            //enabled: !udpoutputFlag
            enabled: interfacesFlag
            onClicked: {
                udpoutputFlag = true
            }
        }

        RadioButton {
            id: udpOff
            x: 124
            y: 120
            width: 49
            height: 15
            text: qsTr("Off")
            ButtonGroup.group: udpOnOffGroup
            checked: !udpFlag
            visible:false
            //enabled: !udpoutputFlag
            enabled: interfacesFlag
            onClicked: {
                udpoutputFlag = false
            }
        }

    ////////////////////////////////////////////////////////////////////////
    //THE CLUSTER INTERFACE ON OFF SWITCH///////////////////////////////////
        Text {
            id: clusterOnOffLabel
            x: 6
            y: 5
            width: 64
            height: 14
            text: qsTr("Concept 1")
            font.pixelSize: 12
        }

        RadioButton {
            id: clusteron
            x: 179
            y: 6
            width: 49
            height: 15
            text: qsTr("On")
            ButtonGroup.group: clusterOnOffGroup
            checked: clusterFlag
            enabled: !interfacesFlag && !(currentClusterEXE.text === "")
            onClicked: {clusterFlag = true}
        }

        RadioButton {
            id: clusteroff
            x: 124
            y: 6
            width: 49
            height: 15
            text: qsTr("Off")
            ButtonGroup.group: clusterOnOffGroup
            checked: !clusterFlag
            enabled: !interfacesFlag && !(currentClusterEXE.text === "")
            onClicked: {clusterFlag = false}
        }

        TextField {
            id: currentClusterEXE
            x: 224
            y: 4
            width: 350
            height: 20
            text: clusterLocation
            placeholderText: qsTr("Cluster Exe Location")
            font.pixelSize: 10
            verticalAlignment: Text.AlignVCenter
            readOnly: true
            clip: false
            z: 1

            MouseArea{
                width: parent.width
                height: parent.height
                enabled: !interfacesFlag
                onClicked: fileDialogCluster.visible = true
            }
        }

    ////////////////////////////////////////////////////////////////////////
    //THE HUD INTERFACE ON OFF SWITCH///////////////////////////////////////
        Text {
            id: hudOnOffLabel
            x: 6
            y:29
            width: 64
            height: 14
            text: qsTr("Concept 2")
            font.pixelSize: 12
        }

        RadioButton {
            id: hudon
            x: 179
            y: 30
            width: 49
            height: 15
            text: qsTr("On")
            ButtonGroup.group: hudOnOffGroup
            checked: hudFlag
            enabled: !interfacesFlag  && !(currentHUDEXE.text === "")
            onClicked: {hudFlag = true}
        }

        RadioButton {
            id: hudoff
            x: 124
            y: 30
            width: 49
            height: 15
            text: qsTr("Off")
            ButtonGroup.group: hudOnOffGroup
            checked: !hudFlag
            enabled: !interfacesFlag  && !(currentHUDEXE.text === "")
            onClicked: {hudFlag = false}
        }

        TextField {
            id: currentHUDEXE
            x: 224
            y: 28
            width: 350
            height: 20
            text: hudLocation
            placeholderText: qsTr("HUD Exe Location")
            font.pixelSize: 10
            verticalAlignment: Text.AlignVCenter
            readOnly: true
            clip: false
            z: 1

            MouseArea{
                width: parent.width
                height: parent.height
                enabled: !interfacesFlag
                onClicked: fileDialogHUD.visible = true
            }
        }

    ////////////////////////////////////////////////////////////////////////
    //THE TOUCHSCREEN INTERFACE ON OFF SWITCH///////////////////////////////
        Text {
            id: touchscreenOnOffLabel
            x: 6
            y: 53
            width: 64
            height: 14
            visible: !(currentTouchscreenEXE.text="")
            text: qsTr("Concept 3")
            font.pixelSize: 12
        }

        RadioButton {
            id: touchscreenon
            x: 179
            y: 54
            width: 49
            height: 15
            text: qsTr("On")
            visible: !(currentTouchscreenEXE.text="")
            ButtonGroup.group: touchscreenOnOffGroup
            checked: touchscreenFlag
            enabled: !interfacesFlag && !(currentTouchscreenEXE.text === "")
            onClicked: {touchscreenFlag = true}
        }

        RadioButton {
            id: touchscreenoff
            x: 124
            y: 54
            width: 49
            height: 15
            text: qsTr("Off")
            visible: !(currentTouchscreenEXE.text="")
            ButtonGroup.group: touchscreenOnOffGroup
            checked: !touchscreenFlag
            enabled: !interfacesFlag && !(currentTouchscreenEXE.text === "")
            onClicked: {touchscreenFlag = false}
        }

        TextField {
            id: currentTouchscreenEXE
            x: 224
            y: 52
            width: 350
            height: 20
            text: touchLocation
            visible: true
            placeholderText: qsTr("Touchscreen Exe Location")
            font.pixelSize: 10
            verticalAlignment: Text.AlignVCenter
            readOnly: true
            clip: false
            z: 1

            MouseArea{
                width: parent.width
                height: parent.height
                enabled: !interfacesFlag
                onClicked: fileDialogTouchscreen.visible = true
            }
        }

    ////////////////////////////////////////////////////////////////////////
    //CHANGE EXECUTABLE FILE DIALOG/////////////////////////////////////////
        FileDialog {

            id: fileDialogCluster
            title: "Please choose the exe"
            nameFilters: ["Executable Files (*.exe)"]

            onAccepted:
            {   clusterLocation = fileDialogCluster.fileUrl
            }
            onRejected:
            {}
        }

        FileDialog {

            id: fileDialogHUD
            title: "Please choose the exe"
            nameFilters: ["Executable Files (*.exe)"]

            onAccepted:
            {   hudLocation = fileDialogHUD.fileUrl
            }
            onRejected:
            { }
        }

        FileDialog {

            id: fileDialogTouchscreen
            title: "Please choose the exe"
            visible: false
            nameFilters: ["Executable Files (*.exe)"]

            onAccepted:
            {  touchLocation = fileDialogTouchscreen.fileUrl
            }
            onRejected:
            {}
        }
    }

    ////////////////////////////////////////////////////////////////////////
    //DISPLAY FOR UNIX TIMESTAMP////////////////////////////////////////////
    Text {
        id: unixStamp
        x: 15
        y: 140
        width: 572
        height: 57
        text: qsTr("Unix time will appear here")
        font.pixelSize: 50
    }
}
