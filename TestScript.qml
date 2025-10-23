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

        // Auto-end task 16 when indicator turns on
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

    // Audio players for baseline and experienced
    MediaPlayer {
        id: baselineSound
        source: "qrc:/audio/audio/baseline.mp3"
        audioOutput: AudioOutput { id: baselineSoundOut }
    }

    MediaPlayer {
        id: experiencedSound
        source: "qrc:/audio/audio/experienced.mp3"
        audioOutput: AudioOutput { id: experiencedSoundOut }
    }

    // Audio players for each unique task type (T1-T23)
    MediaPlayer {
        id: t1Sound
        source: "qrc:/audio/audio/T1.mp3"
        audioOutput: AudioOutput { id: t1SoundOut }
    }

    MediaPlayer {
        id: t5Sound
        source: "qrc:/audio/audio/T5- 24.mp3"
        audioOutput: AudioOutput { id: t5SoundOut }
    }

    MediaPlayer {
        id: t9Sound
        source: "qrc:/audio/audio/T9.mp3"
        audioOutput: AudioOutput { id: t9SoundOut }
    }

    MediaPlayer {
        id: t11Sound
        source: "qrc:/audio/audio/T11- Bangor.mp3"
        audioOutput: AudioOutput { id: t11SoundOut }
    }

    MediaPlayer {
        id: t12Sound
        source: "qrc:/audio/audio/T12.mp3"
        audioOutput: AudioOutput { id: t12SoundOut }
    }

    MediaPlayer {
        id: t14Sound
        source: "qrc:/audio/audio/T14.mp3"
        audioOutput: AudioOutput { id: t14SoundOut }
    }

    MediaPlayer {
        id: t16Sound
        source: "qrc:/audio/audio/T16-left.mp3"
        audioOutput: AudioOutput { id: t16SoundOut }
    }

    MediaPlayer {
        id: t18Sound
        source: "qrc:/audio/audio/T18-split.mp3"
        audioOutput: AudioOutput { id: t18SoundOut }
    }

    MediaPlayer {
        id: t19Sound
        source: "qrc:/audio/audio/T19-on.mp3"
        audioOutput: AudioOutput { id: t19SoundOut }
    }

    MediaPlayer {
        id: t21Sound
        source: "qrc:/audio/audio/T21- Virgin Radio UK.mp3"
        audioOutput: AudioOutput { id: t21SoundOut }
    }

    MediaPlayer {
        id: t22Sound
        source: "qrc:/audio/audio/T22- Comfort.mp3"
        audioOutput: AudioOutput { id: t22SoundOut }
    }

    MediaPlayer {
        id: t23Sound
        source: "qrc:/audio/audio/T23.mp3"
        audioOutput: AudioOutput { id: t23SoundOut }
    }

    Timer {
        id: autoNextTimer
        interval: 5000
        repeat: false
        onTriggered: {
            if (!taskModel.hasActiveTask && !taskModel.isPaused && nextTaskBtn.enabled) {
                nextTaskBtn.clicked()
            }
        }
    }

    Column {
        spacing: 4

        // Top control row with Baseline, Experienced, and Pause buttons
        Row {
            spacing: 10
            Label { text: "Special Tasks:" }

            Button {
                id: startBaselineBtn
                text: "Start Baseline (Abs#1)"
                enabled: !taskModel.hasActiveTask && !taskModel.isPaused
                onClicked: {
                    taskModel.repFilter = 0  // Baseline rep
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {
                        switch(taskModel.currentActiveTask.absoluteTaskNum){
                        case 11 :
                            requestCheckTask(1, true)
                            break;
                        case 1 :
                            requestCheckTask(2, true)
                            break;
                        case 5 :
                            requestCheckTask(3, true)
                            break;
                        case 18 :
                            requestCheckTask(4, true)
                            break;
                        case 14 :
                            requestCheckTask(5, true)
                            break;
                        case 12 :
                            requestCheckTask(6, true)
                            break;
                        case 9 :
                            requestCheckTask(7, true)
                            break;
                        case 23 :
                            requestCheckTask(8, true)
                            break;
                        case 21 :
                            requestCheckTask(9, true)
                            break;
                        case 16 :
                            requestCheckTask(10, true)
                            break;
                        case 22 :
                            requestCheckTask(11, true)
                            break;
                        case 19 :
                            requestCheckTask(12, true)
                            break;
                        default:
                            break;
                            }
                        playAudioForCurrentTask()
                    }

                    expdesignform.triggerSave()
                }
            }

            Button {
                id: startExperiencedBtn
                text: "Start Experienced (Abs#122)"
                enabled: !taskModel.hasActiveTask && !taskModel.isPaused
                onClicked: {
                    taskModel.repFilter = 11  // Experienced rep
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {
                        switch(taskModel.currentActiveTask.absoluteTaskNum){
                        case 11 :
                            requestCheckTask(1, true)
                            break;
                        case 1 :
                            requestCheckTask(2, true)
                            break;
                        case 5 :
                            requestCheckTask(3, true)
                            break;
                        case 18 :
                            requestCheckTask(4, true)
                            break;
                        case 14 :
                            requestCheckTask(5, true)
                            break;
                        case 12 :
                            requestCheckTask(6, true)
                            break;
                        case 9 :
                            requestCheckTask(7, true)
                            break;
                        case 23 :
                            requestCheckTask(8, true)
                            break;
                        case 21 :
                            requestCheckTask(9, true)
                            break;
                        case 16 :
                            requestCheckTask(10, true)
                            break;
                        case 22 :
                            requestCheckTask(11, true)
                            break;
                        case 19 :
                            requestCheckTask(12, true)
                            break;
                        default:
                            break;
                            }
                        playAudioForCurrentTask()
                    }

                    expdesignform.triggerSave()
                }
            }

            Button {
                id: pauseResumeBtn
                text: taskModel.isPaused ? "Resume" : "Pause"
                enabled: !taskModel.hasActiveTask
                onClicked: {
                    taskModel.setPaused(!taskModel.isPaused)
                }
            }
        }

        // Rep selector for tasks 2-121 (Reps 1-10)
        Row {
            spacing: 20
            Label { text: "Rep:" }
            ComboBox {
                id: repSelector
                width: 150
                model: ["Rep 1 (order#1-12)", "Rep 2 (order#13-24)", "Rep 3 (order#25-36)",
                        "Rep 4 (order#37-48)", "Rep 5 (order#49-60)", "Rep 6 (order#61-72)",
                        "Rep 7 (order#73-84)", "Rep 8 (order#85-96)", "Rep 9 (order#97-110)",
                        "Rep 10 (order#111-122)"]
                currentIndex: 0
                onCurrentIndexChanged: {
                    taskModel.repFilter = currentIndex + 1  // Rep 1-10
                    taskView.currentIndex = 0
                    taskModel.clearActiveTask()
                    startTaskBtn.enabled = true
                    nextTaskBtn.enabled = false
                    endTaskBtn.enabled = false
                }
            }

            // Task control buttons
            Button {
                id: startTaskBtn
                text: "Start Task"
                enabled: !taskModel.hasActiveTask && !taskModel.isPaused
                onClicked: {
                    taskView.currentIndex = 0
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {

                       // requestCheckTask(taskModel.currentActiveTask, true)
                        switch(taskModel.currentActiveTask.absoluteTaskNum){
                        case 11 :
                            requestCheckTask(1, true)
                            break;
                        case 1 :
                            requestCheckTask(2, true)
                            break;
                        case 5 :
                            requestCheckTask(3, true)
                            break;
                        case 18 :
                            requestCheckTask(4, true)
                            break;
                        case 14 :
                            requestCheckTask(5, true)
                            break;
                        case 12 :
                            requestCheckTask(6, true)
                            break;
                        case 9 :
                            requestCheckTask(7, true)
                            break;
                        case 23 :
                            requestCheckTask(8, true)
                            break;
                        case 21 :
                            requestCheckTask(9, true)
                            break;
                        case 16 :
                            requestCheckTask(10, true)
                            break;
                        case 22 :
                            requestCheckTask(11, true)
                            break;
                        case 19 :
                            requestCheckTask(12, true)
                            break;
                        default:
                            break;
                            }
                        playAudioForCurrentTask()
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
                    var currentRep = repSelector.currentIndex + 1

                    if (next < taskView.count) {
                        // Continue in current rep
                        taskView.currentIndex = next
                        taskModel.setActiveTaskByIndex(next)

                        if (taskModel.currentActiveTask) {
                            switch(taskModel.currentActiveTask.absoluteTaskNum){
                            case 11 :
                                requestCheckTask(1, true)
                                break;
                            case 1 :
                                requestCheckTask(2, true)
                                break;
                            case 5 :
                                requestCheckTask(3, true)
                                break;
                            case 18 :
                                requestCheckTask(4, true)
                                break;
                            case 14 :
                                requestCheckTask(5, true)
                                break;
                            case 12 :
                                requestCheckTask(6, true)
                                break;
                            case 9 :
                                requestCheckTask(7, true)
                                break;
                            case 23 :
                                requestCheckTask(8, true)
                                break;
                            case 21 :
                                requestCheckTask(9, true)
                                break;
                            case 16 :
                                requestCheckTask(10, true)
                                break;
                            case 22 :
                                requestCheckTask(11, true)
                                break;
                            case 19 :
                                requestCheckTask(12, true)
                                break;
                            default:
                                break;
                                }
                            playAudioForCurrentTask()
                        }

                        nextTaskBtn.enabled = false
                        endTaskBtn.enabled = true
                    } else {
                        // Move to next rep
                        if (currentRep < 10) {
                            repSelector.currentIndex = currentRep
                            taskView.currentIndex = 0
                            taskModel.setActiveTaskByIndex(0)

                            if (taskModel.currentActiveTask) {
                                switch(taskModel.currentActiveTask.absoluteTaskNum){
                                case 11 :
                                    requestCheckTask(1, true)
                                    break;
                                case 1 :
                                    requestCheckTask(2, true)
                                    break;
                                case 5 :
                                    requestCheckTask(3, true)
                                    break;
                                case 18 :
                                    requestCheckTask(4, true)
                                    break;
                                case 14 :
                                    requestCheckTask(5, true)
                                    break;
                                case 12 :
                                    requestCheckTask(6, true)
                                    break;
                                case 9 :
                                    requestCheckTask(7, true)
                                    break;
                                case 23 :
                                    requestCheckTask(8, true)
                                    break;
                                case 21 :
                                    requestCheckTask(9, true)
                                    break;
                                case 16 :
                                    requestCheckTask(10, true)
                                    break;
                                case 22 :
                                    requestCheckTask(11, true)
                                    break;
                                case 19 :
                                    requestCheckTask(12, true)
                                    break;
                                default:
                                    break;
                                    }
                                playAudioForCurrentTask()
                            }

                            nextTaskBtn.enabled = false
                            endTaskBtn.enabled = true
                        } else {
                            console.log("All 120 rep tasks complete for participant " + filename.selectedParticipant)
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

                    if (taskModel.currentActiveTask) {
                        switch(taskModel.currentActiveTask.absoluteTaskNum){
                        case 11 :
                            requestCheckTask(1, false)
                            break;
                        case 1 :
                            requestCheckTask(2, false)
                            break;
                        case 5 :
                            requestCheckTask(3, false)
                            break;
                        case 18 :
                            requestCheckTask(4, false)
                            break;
                        case 14 :
                            requestCheckTask(5, false)
                            break;
                        case 12 :
                            requestCheckTask(6, false)
                            break;
                        case 9 :
                            requestCheckTask(7, false)
                            break;
                        case 23 :
                            requestCheckTask(8, false)
                            break;
                        case 21 :
                            requestCheckTask(9, false)
                            break;
                        case 16 :
                            requestCheckTask(10, false)
                            break;
                        case 22 :
                            requestCheckTask(11, false)
                            break;
                        case 19 :
                            requestCheckTask(12, false)
                            break;
                        default:
                            break;
                            }
                    }

                    taskModel.clearActiveTask()

                    startTaskBtn.enabled = false
                    nextTaskBtn.enabled = !taskModel.isPaused
                    endTaskBtn.enabled = false

                    if (!taskModel.isPaused) {
                        autoNextTimer.restart()
                    }
                }
            }
        }

        // Column headers
        Row {
            spacing: 40
            width: parent.width
            Rectangle { width: 60; height: 20; color: "lightgray"; Text { anchors.centerIn: parent; text: "Task Order" } }
            Rectangle { width: 40; height: 20; color: "lightgray"; Text { anchors.centerIn: parent; text: "Abs#" } }
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
                    width: 40; height: 20
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

        // Baseline (Abs#1)
        if (absNum === 121) {
            baselineSound.play()
            return
        }

        // Experienced (Abs#122)
        if (absNum === 122) {
            experiencedSound.play()
            return
        }

        // For tasks 2-121, map to the underlying task type
        // We need to get the task name to determine which audio to play
        var taskName = taskModel.currentActiveTask.name

        if (taskName === "Navigation") t11Sound.play()
        else if (taskName === "External Temp") t1Sound.play()
        else if (taskName === "Temp adjust") t5Sound.play()
        else if (taskName === "Split Airflow") t18Sound.play()
        else if (taskName === "wash/wipe") t14Sound.play()
        else if (taskName === "flick wipe") t12Sound.play()
        else if (taskName === "pause media") t9Sound.play()
        else if (taskName === "Fuel check") t23Sound.play()
        else if (taskName === "Radio selection") t21Sound.play()
        else if (taskName === "give indicator") t16Sound.play()
        else if (taskName === "Driver mode") t22Sound.play()
        else if (taskName === "Country Road") t19Sound.play()
        else console.log("No audio mapping for task: " + taskName)
    }

    // Initialize filters
    Component.onCompleted: {
        taskModel.participantFilter = filename.selectedParticipant
        taskModel.repFilter = 1  // Start with Rep 1
        taskModel.clearActiveTask()
    }

    Connections {
        target: filename
        onSelectedParticipantChanged: {
            taskModel.participantFilter = filename.selectedParticipant
            repSelector.currentIndex = 0
            taskModel.repFilter = 1
            taskView.currentIndex = 0
            taskModel.clearActiveTask()
            startTaskBtn.enabled = true
            nextTaskBtn.enabled = false
            endTaskBtn.enabled = false
        }
    }

    signal requestCheckTask(int taskNum, bool state)
}
