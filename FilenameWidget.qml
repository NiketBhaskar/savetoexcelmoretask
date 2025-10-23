import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

Item {
    ////////////////////////////////////////////////////////////////////////
    //THIS WIDGET DEFINES THE FILENAME SETTINGS/////////////////////////////
    id: filenameWidget
    x: 0
    y: 0
    width: 600
    height: 130

    //Exposed properties
    property string defaultFileLocation: "D:/"
    property string currentfilenameText: ""
    property string currentfoldernameText: ""
    property int selectedParticipant: -1   // -1 means not selected

    ////////////////////////////////////////////////////////////////////////
    //RESET DEFAULT VALUES FUNCTION/////////////////////////////////////////
    function reset_vals() {
        defaultFileLocation = "D:/"
        selectedParticipant = -1
    }

    ////////////////////////////////////////////////////////////////////////
    //GROUP BOX FOR FILENAME WIDGET/////////////////////////////////////////
    GroupBox {
        id: filenameBox
        x: 5
        y: 10
        width: 590
        height: 120
        title: "  FILE NAME SETTINGS  "

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 8

            ////////////////////////////////////////////////////////////////////////
            // PARTICIPANT DROPDOWN ///////////////////////////////////////////////
            RowLayout {
                spacing: 10

                Label {
                    text: "Participant Number:"
                    Layout.alignment: Qt.AlignVCenter
                }

                ComboBox {
                    id: participantDropdown
                    Layout.fillWidth: true
                    model: ListModel {
                        Component.onCompleted: {
                            for (var i = 1; i <= 30; i++) {
                                append({ "name": "Participant " + i })
                            }
                        }
                    }
                    textRole: "name"

                    onCurrentIndexChanged: {
                        if (currentIndex >= 0) {
                            filenameWidget.selectedParticipant = currentIndex + 1
                        } else {
                            filenameWidget.selectedParticipant = -1
                        }
                    }
                }

            }

            ////////////////////////////////////////////////////////////////////////
            //TEXT FIELD FOR CURRENT TARGET FOLDER//////////////////////////////////
            TextField {
                id: currentfoldername
                Layout.fillWidth: true
                height: 24
                readOnly: true
                text: currentfoldernameText
                placeholderText: qsTr("   Current Folder Location")
                verticalAlignment: Text.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        newFileLocationDialog.visible = true
                    }
                }
            }

            ////////////////////////////////////////////////////////////////////////
            //TEXT FIELD TO ENTER FILENAME//////////////////////////////////////////
            TextField {
                id: currentfilename
                Layout.fillWidth: true
                height: 24
                readOnly: true
                enabled: filenameWidget.selectedParticipant > 0
                text: currentfilenameText
                placeholderText: qsTr("   Enter File Name (enabled after participant selected)")
                verticalAlignment: Text.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    enabled: filenameWidget.selectedParticipant > 0
                    onClicked: {
                        filenameEntry.visible = true
                        currentfilenameDialog.focus = true
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //FOLDER DIALOG TO SELECT NEW FOLDER FOR USE IN THE PROJECT/////////////
        FolderDialog {
            id: newFileLocationDialog
            title: "Please choose a file save location"

            onAccepted: {
                setNewFolderQML(newFileLocationDialog.currentFolder)
                currentfoldernameText = newFileLocationDialog.currentFolder
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //FILENAME DIALOG TO SELECT NEW FOLDER FOR USE IN THE PROJECT///////////
        Dialog {
            id: filenameEntry
            width: 200
            height: 100
            visible: false
            title: "ENTER NEW FILE NAME"
            standardButtons: Dialog.Save | Dialog.Cancel

            onAccepted: {
                currentfilenameText = currentfilenameDialog.text
                updateFilenameQML(currentfilename.text, filenameWidget.selectedParticipant.toString())                     //Create filename
                if (currentfilename.text !== "") makeDirQML()               //Create folder and file structure
            }

            TextEdit {
                id: currentfilenameDialog
                x: 5
                y: 5
                width: 150
                height: 20
                color: "White"
                text: ""
                readOnly: false
                clip: false
                focus: true
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 12
            }

            Rectangle {
                color: "#262221"
                x: 3
                y: 5
                z: 1
                implicitWidth: 156
                implicitHeight: 20
            }
        }
    }
}
