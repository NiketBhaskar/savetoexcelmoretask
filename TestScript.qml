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

    // State management
    enum TaskState {
        Idle,           // Nothing active
        BaselineActive, // Baseline drive running
        TasksActive,    // Regular tasks running
        ExperiencedActive // Experienced drive running
    }
    property int currentState: TestScript.TaskState.Idle
    property int savedRepFilter: 1  // Save user's rep selection when doing baseline/experienced

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
                var absNum = taskModel.currentActiveTask.absoluteTaskNum
                if (absNum === 16) {
                    endTaskBtn.clicked()
                }
            }
        }
    }

    // Audio players
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
            if (!taskModel.hasActiveTask && !taskModel.isPaused &&
                currentState === TestScript.TaskState.TasksActive) {
                nextTaskBtn.clicked()
            }
        }
    }

    Column {
        spacing: 4

        // Top control row - BASELINE AND EXPERIENCED
        Row {
            spacing: 10
            Label {
                text: "Special Drives:"
                font.bold: true
            }

            Button {
                id: startBaselineBtn
                text: "Start Baseline"
                enabled: currentState === TestScript.TaskState.Idle && !taskModel.isPaused
                onClicked: {
                    currentState = TestScript.TaskState.BaselineActive

                    // Save current rep filter
                    savedRepFilter = taskModel.repFilter

                    // ✅ Set filter to -1 to show only baseline
                    taskModel.repFilter = -1
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {
                        baselineSound.play()
                        expdesignform.triggerSave()
                    }
                }
            }

            Button {
                id: endBaselineBtn
                text: "End Baseline"
                enabled: currentState === TestScript.TaskState.BaselineActive && taskModel.hasActiveTask
                onClicked: {
                    expdesignform.triggerEnd()
                    taskModel.clearActiveTask()

                    // ✅ Restore previous rep filter
                    taskModel.repFilter = savedRepFilter

                    currentState = TestScript.TaskState.Idle
                }
            }

            Button {
                id: startExperiencedBtn
                text: "Start Experienced"
                enabled: currentState === TestScript.TaskState.Idle && !taskModel.isPaused
                onClicked: {
                    currentState = TestScript.TaskState.ExperiencedActive

                    // Save current rep filter
                    savedRepFilter = taskModel.repFilter

                    // ✅ Set filter to -2 to show only experienced
                    taskModel.repFilter = -2
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {
                        experiencedSound.play()
                        expdesignform.triggerSave()
                    }
                }
            }

            Button {
                id: endExperiencedBtn
                text: "End Experienced"
                enabled: currentState === TestScript.TaskState.ExperiencedActive && taskModel.hasActiveTask
                onClicked: {
                    expdesignform.triggerEnd()
                    taskModel.clearActiveTask()

                    // ✅ Restore previous rep filter
                    taskModel.repFilter = savedRepFilter

                    currentState = TestScript.TaskState.Idle
                }
            }

            // Visual separator
            Rectangle {
                width: 2
                height: parent.height
                color: "gray"
            }

            Button {
                id: pauseResumeBtn
                text: taskModel.isPaused ? "Resume" : "Pause"
                enabled: currentState === TestScript.TaskState.TasksActive && !taskModel.hasActiveTask
                onClicked: {
                    taskModel.setPaused(!taskModel.isPaused)
                }
            }
        }

        // Separator
        Rectangle {
            width: parent.width
            height: 2
            color: "lightgray"
        }

        // REGULAR TASKS (1-120) Section
        Row {
            spacing: 20
            Label {
                text: "Regular Tasks (1-120):"
                font.bold: true
            }

            ComboBox {
                id: repSelector
                width: 200
                model: ["Rep 1 (Tasks 1-12)", "Rep 2 (Tasks 13-24)", "Rep 3 (Tasks 25-36)",
                        "Rep 4 (Tasks 37-48)", "Rep 5 (Tasks 49-60)", "Rep 6 (Tasks 61-72)",
                        "Rep 7 (Tasks 73-84)", "Rep 8 (Tasks 85-96)", "Rep 9 (Tasks 97-108)",
                        "Rep 10 (Tasks 109-120)"]
                currentIndex: 0
                enabled: currentState === TestScript.TaskState.Idle
                onCurrentIndexChanged: {
                    if (currentState === TestScript.TaskState.Idle) {
                        taskModel.repFilter = currentIndex + 1  // Rep 1-10
                        savedRepFilter = taskModel.repFilter  // Save it
                        taskView.currentIndex = 0
                        taskModel.clearActiveTask()
                    }
                }
            }

            // Task control buttons
            Button {
                id: startTaskBtn
                text: "Start Tasks"
                enabled: currentState === TestScript.TaskState.Idle && !taskModel.isPaused
                onClicked: {
                    currentState = TestScript.TaskState.TasksActive

                    // ✅ Make sure we're showing the selected rep
                    taskModel.repFilter = repSelector.currentIndex + 1
                    savedRepFilter = taskModel.repFilter

                    taskView.currentIndex = 0
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {
                        checkAndSetTask(taskModel.currentActiveTask.absoluteTaskNum)
                        playAudioForCurrentTask()
                        expdesignform.triggerSave()
                    }
                }
            }

            Button {
                id: nextTaskBtn
                text: "Next Task"
                enabled: currentState === TestScript.TaskState.TasksActive &&
                         !taskModel.hasActiveTask && !taskModel.isPaused
                onClicked: {
                    var next = taskView.currentIndex + 1
                    var currentRep = repSelector.currentIndex + 1

                    if (next < taskView.count) {
                        // Continue in current rep
                        taskView.currentIndex = next
                        taskModel.setActiveTaskByIndex(next)

                        if (taskModel.currentActiveTask) {
                            checkAndSetTask(taskModel.currentActiveTask.absoluteTaskNum)
                            playAudioForCurrentTask()
                            expdesignform.triggerSave()
                        }
                    } else {
                        // Check if we need to move to next rep
                        if (currentRep < 10) {
                            repSelector.currentIndex = currentRep
                            taskModel.repFilter = currentRep + 1
                            savedRepFilter = taskModel.repFilter
                            taskView.currentIndex = 0
                            taskModel.setActiveTaskByIndex(0)

                            if (taskModel.currentActiveTask) {
                                checkAndSetTask(taskModel.currentActiveTask.absoluteTaskNum)
                                playAudioForCurrentTask()
                                expdesignform.triggerSave()
                            }
                        } else {
                            // All 120 tasks complete
                            console.log("All 120 tasks completed for participant " + filename.selectedParticipant)
                            currentState = TestScript.TaskState.Idle
                            taskModel.repFilter = savedRepFilter  // Restore filter
                        }
                    }
                }
            }

            Button {
                id: endTaskBtn
                text: "End Task"
                enabled: currentState === TestScript.TaskState.TasksActive && taskModel.hasActiveTask
                onClicked: {
                    expdesignform.triggerEnd()

                    if (taskModel.currentActiveTask) {
                        var absNum = taskModel.currentActiveTask.absoluteTaskNum
                        checkAndSetTask(absNum, false)
                    }

                    taskModel.clearActiveTask()

                    if (!taskModel.isPaused) {
                        autoNextTimer.restart()
                    }
                }
            }

            // Stop All Tasks button (emergency stop)
            Button {
                id: stopAllTasksBtn
                text: "Stop All Tasks"
                enabled: currentState === TestScript.TaskState.TasksActive
                onClicked: {
                    if (taskModel.hasActiveTask) {
                        expdesignform.triggerEnd()
                        taskModel.clearActiveTask()
                    }
                    currentState = TestScript.TaskState.Idle
                    taskModel.setPaused(false)
                    autoNextTimer.stop()

                    // Restore rep filter
                    taskModel.repFilter = savedRepFilter
                }
            }
        }

        // Status indicator
        Row {
            spacing: 10
            Label {
                text: "Status:"
                font.bold: true
            }
            Label {
                text: {
                    switch(currentState) {
                        case TestScript.TaskState.Idle: return "Idle"
                        case TestScript.TaskState.BaselineActive: return "BASELINE DRIVE ACTIVE"
                        case TestScript.TaskState.TasksActive: return "TASKS ACTIVE"
                        case TestScript.TaskState.ExperiencedActive: return "EXPERIENCED DRIVE ACTIVE"
                        default: return "Unknown"
                    }
                }
                font.bold: true
                color: currentState === TestScript.TaskState.Idle ? "gray" : "green"
            }
            Label {
                text: taskModel.isPaused ? "(PAUSED)" : ""
                font.bold: true
                color: "orange"
                visible: taskModel.isPaused
            }
            Label {
                text: "Rep: " + taskModel.repFilter + " | Tasks in list: " + taskView.count
                color: "blue"
                visible: currentState !== TestScript.TaskState.Idle
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

    // Helper function to check and set task on Android device
    function checkAndSetTask(absNum, state = true) {
        var taskMap = {
            11: 1, 1: 2, 5: 3, 18: 4, 14: 5, 12: 6,
            9: 7, 23: 8, 21: 9, 16: 10, 22: 11, 19: 12, 0: 0
        }

        if (taskMap.hasOwnProperty(absNum)) {
            requestCheckTask(taskMap[absNum], state)
        }
    }

    // Function to play audio based on current active task
    function playAudioForCurrentTask() {
        if (!taskModel.currentActiveTask) return

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
        savedRepFilter = 1
        taskModel.clearActiveTask()
        currentState = TestScript.TaskState.Idle
    }

    Connections {
        target: filename
        onSelectedParticipantChanged: {
            taskModel.participantFilter = filename.selectedParticipant
            repSelector.currentIndex = 0
            taskModel.repFilter = 1
            savedRepFilter = 1
            taskView.currentIndex = 0
            taskModel.clearActiveTask()
            currentState = TestScript.TaskState.Idle
            taskModel.setPaused(false)
        }
    }

    signal requestCheckTask(int taskNum, bool state)
}
