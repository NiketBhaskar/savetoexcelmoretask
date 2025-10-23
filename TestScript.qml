import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Item {
    id: testScript
    property alias endTaskBtn: endTaskBtn
    property string indicatorSignal: "none"
    property string lastDeviceVal: "No Data Available"
    property string lastEventVal:  "No Data Available"

    function setTerminalWindowTaskProperty(deviceVal, eventVal) {
        lastDeviceVal = deviceVal
        lastEventVal  = eventVal

        if (eventVal === "LeftIndicatorOn" || eventVal === "RightIndicatorOn") {
            indicatorSignal = (eventVal === "LeftIndicatorOn") ? "Left" : "Right"
        }

        // ✅ If abs task 16 is active AND indicator turns on → press End
        if (eventVal === "LeftIndicatorOn" || eventVal === "RightIndicatorOn") {
            var hasTask = taskModel.hasActiveTask && taskModel.currentActiveTask
            if (hasTask) {
                var absNum = parseInt(taskModel.currentActiveTask.absoluteTaskNum, 10)
                if (absNum === 16) {
                    endTaskBtn.clicked()
                }
            }
        }
    }

    // Audio players for each task type
    MediaPlayer {
        id: reminderSound
        source: "qrc:/audio/audio/TTSOdummy.mp3"
        audioOutput: AudioOutput { id: reminderOut }
    }

    MediaPlayer {
        id: baselineSound
        source: "qrc:/audio/audio/baseline.mp3"  // Add your baseline audio
        audioOutput: AudioOutput { id: baselineSoundOut }
    }

    MediaPlayer {
        id: experiencedSound
        source: "qrc:/audio/audio/experienced.mp3"  // Add your experienced audio
        audioOutput: AudioOutput { id: experiencedSoundOut }
    }

    MediaPlayer {
        id: t11Sound
        source: "qrc:/audio/audio/T11- Bangor.mp3"  // Use first variant for all
        audioOutput: AudioOutput { id: t11SoundOut }
    }

    MediaPlayer {
        id: t1Sound
        source: "qrc:/audio/audio/T1.mp3"
        audioOutput: AudioOutput { id: t1SoundOut }
    }

    MediaPlayer {
        id: t5Sound
        source: "qrc:/audio/audio/T5- 24.mp3"  // Use first variant for all
        audioOutput: AudioOutput { id: t5SoundOut }
    }

    MediaPlayer {
        id: t18Sound
        source: "qrc:/audio/audio/T18-split.mp3"
        audioOutput: AudioOutput { id: t18SoundOut }
    }

    MediaPlayer {
        id: t14Sound
        source: "qrc:/audio/audio/T14.mp3"
        audioOutput: AudioOutput { id: t14SoundOut }
    }

    MediaPlayer {
        id: t12Sound
        source: "qrc:/audio/audio/T12.mp3"
        audioOutput: AudioOutput { id: t12SoundOut }
    }

    MediaPlayer {
        id: t9Sound
        source: "qrc:/audio/audio/T9.mp3"
        audioOutput: AudioOutput { id: t9SoundOut }
    }

    MediaPlayer {
        id: t23Sound
        source: "qrc:/audio/audio/T23.mp3"
        audioOutput: AudioOutput { id: t23SoundOut }
    }

    MediaPlayer {
        id: t21Sound
        source: "qrc:/audio/audio/T21- Virgin Radio UK.mp3"  // Use first variant for all
        audioOutput: AudioOutput { id: t21SoundOut }
    }

    MediaPlayer {
        id: t16Sound
        source: "qrc:/audio/audio/T16-left.mp3"  // Use left variant
        audioOutput: AudioOutput { id: t16SoundOut }
    }

    MediaPlayer {
        id: t22Sound
        source: "qrc:/audio/audio/T22- Comfort.mp3"  // Use comfort variant
        audioOutput: AudioOutput { id: t22SoundOut }
    }

    MediaPlayer {
        id: t19Sound
        source: "qrc:/audio/audio/T19-on.mp3"  // Use on variant
        audioOutput: AudioOutput { id: t19SoundOut }
    }

    Timer {
        id: autoNextTimer
        interval: 5000      // 5 seconds
        repeat: false
        onTriggered: {
            console.log("5s passed after End Task")
            if (!taskModel.hasActiveTask && nextTaskBtn.enabled) {
                nextTaskBtn.clicked()
            }
        }
    }

    Column {
        spacing: 4

        // Rep selector showing: Baseline, Rep 1-10, Experienced
        Row {
            spacing: 20
            Label { text: "Section:" }
            ComboBox {
                id: repSelector
                width: 150
                model: ["Baseline", "Rep 1", "Rep 2", "Rep 3", "Rep 4", "Rep 5",
                        "Rep 6", "Rep 7", "Rep 8", "Rep 9", "Rep 10", "Experienced"]
                currentIndex: 0
                onCurrentIndexChanged: {
                    // Map UI index to actual rep filter
                    // 0 = Baseline (rep 0)
                    // 1-10 = Rep 1-10 (rep 1-10)
                    // 11 = Experienced (rep 11)
                    taskModel.repFilter = currentIndex
                    taskView.currentIndex = 0
                    taskModel.clearActiveTask()
                    startTaskBtn.enabled = true
                    nextTaskBtn.enabled = false
                    endTaskBtn.enabled = false
                }
            }

            // Control buttons
            Button {
                id: startTaskBtn
                text: "Start Task"
                enabled: true
                onClicked: {
                    taskView.currentIndex = 0
                    taskModel.setActiveTaskByIndex(0)  // Set first task in filtered list

                    // Check task in TTSO (use absolute task order)
                    if (taskModel.currentActiveTask) {
                        requestCheckTask(taskModel.currentActiveTask.taskOrder, true)
                        playAudioForCurrentTask()  // Play after task is set
                    }

                    startTaskBtn.enabled = false
                    nextTaskBtn.enabled = false
                    endTaskBtn.enabled = true
                    expdesignform.triggerSave()
                }
            }

            Button {
                id: nextTaskBtn
                text: "Next Task"
                enabled: false
                onClicked: {
                    var next = taskView.currentIndex + 1
                    var currentRep = repSelector.currentIndex

                    if (next < taskView.count) {
                        taskView.currentIndex = next
                        taskModel.setActiveTaskByIndex(next)  // Use filtered index

                        // Check next task in TTSO and play audio
                        if (taskModel.currentActiveTask) {
                            requestCheckTask(taskModel.currentActiveTask.taskOrder, true)
                            playAudioForCurrentTask()  // Play after task is set
                        }

                        nextTaskBtn.enabled = false
                        endTaskBtn.enabled = true
                    } else {
                        // Move to next section
                        if (currentRep < 11) {  // 11 is Experienced
                            repSelector.currentIndex = currentRep + 1
                            taskView.currentIndex = 0
                            taskModel.setActiveTaskByIndex(0)  // First task in new section

                            if (taskModel.currentActiveTask) {
                                requestCheckTask(taskModel.currentActiveTask.taskOrder, true)
                                playAudioForCurrentTask()  // Play after task is set
                            }

                            nextTaskBtn.enabled = false
                            endTaskBtn.enabled = true
                        } else {
                            console.log("All 122 tasks complete for participant " + filename.selectedParticipant)
                        }
                    }
                    expdesignform.triggerSave()
                }
            }

            Button {
                id: endTaskBtn
                text: "End Task"
                enabled: false
                onClicked: {
                    expdesignform.triggerEnd()

                    // Uncheck current task using absolute task order
                    if (taskModel.currentActiveTask) {
                        requestCheckTask(taskModel.currentActiveTask.taskOrder, false)
                    }

                    taskModel.clearActiveTask()

                    startTaskBtn.enabled = false
                    nextTaskBtn.enabled = true
                    endTaskBtn.enabled = false
                    autoNextTimer.restart()
                }
            }
        }

        // Column headers
        Row {
            spacing: 40
            width: parent.width
            Rectangle { width: 60; height: 20; color: "lightgray"; Text { anchors.centerIn: parent; text: "Task Order" } }
            Rectangle { width: 30; height: 20; color: "lightgray"; Text { anchors.centerIn: parent; text: "AbsT" } }
            Rectangle { width: 120; height: 20; color: "lightgray"; Text { anchors.centerIn: parent; text: "Name" } }
            Rectangle { width: 40; height: 20; color: "lightgray"; Text { anchors.centerIn: parent; text: "Comp" } }
            Rectangle { width: 90; height: 20; color: "lightgray"; Text { anchors.centerIn: parent; text: "Control" } }
        }

        // Task list
        ListView {
            id: taskView
            width: parent.width
            height: 400
            model: taskModel
            clip: true

            delegate: Row {
                height: 20
                spacing: 40

                Rectangle {
                    width: 60; height: 20
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.taskOrder }
                }
                Rectangle {
                    width: 30; height: 20
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.absoluteTaskNum }
                }
                Rectangle {
                    width: 120; height: 20
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.name; wrapMode: Text.WordWrap }
                }
                Rectangle {
                    width: 40; height: 20
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.complexity }
                }
                Rectangle {
                    width: 90; height: 20
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.controlTypes }
                }
            }
        }
    }

    // Function to play audio based on current active task
    function playAudioForCurrentTask() {
        if (!taskModel.currentActiveTask) return

        var absNum = taskModel.currentActiveTask.absoluteTaskNum

        // Play audio based on absolute task number
        switch(absNum) {
        case 0:  baselineSound.play(); break;
        case -1: experiencedSound.play(); break;
        case 1:  t1Sound.play(); break;
        case 5:  t5Sound.play(); break;
        case 9:  t9Sound.play(); break;
        case 11: t11Sound.play(); break;
        case 12: t12Sound.play(); break;
        case 14: t14Sound.play(); break;
        case 16: t16Sound.play(); break;
        case 18: t18Sound.play(); break;
        case 19: t19Sound.play(); break;
        case 21: t21Sound.play(); break;
        case 22: t22Sound.play(); break;
        case 23: t23Sound.play(); break;
        default:
            console.log("No audio for task " + absNum)
            break;
        }
    }

    // Keep filters synced
    Component.onCompleted: {
        taskModel.participantFilter = filename.selectedParticipant
        taskModel.repFilter = 0  // Start with Baseline
        taskModel.clearActiveTask()
    }

    Connections {
        target: filename
        onSelectedParticipantChanged: {
            taskModel.participantFilter = filename.selectedParticipant
            repSelector.currentIndex = 0
            taskModel.repFilter = 0
            taskView.currentIndex = 0
            taskModel.clearActiveTask()
            startTaskBtn.enabled = true
            nextTaskBtn.enabled = false
            endTaskBtn.enabled = false
        }
    }

    signal requestCheckTask(int taskNum, bool state)
}
