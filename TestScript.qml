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
    property MediaPlayer nextAfterBeep: null

    function playBeepThen(player) {
        // defensively stop both to handle rapid clicks
        if (player) player.stop()
        beepSound.stop()
        nextAfterBeep = player
        beepSound.play()
    }
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
    // --- Stuck/resume detection state ---
    property real lastRoundedTime: -1.0
    property int sameRunCount: 0
    property bool autoPauseActive: false
    property int requiredSameCount: 3     // trigger after 3 identical values
    property int decimals: 5              // precision: 5 decimal places

    function roundN(x, n) {
        var f = Math.pow(10, n)
        return Math.round(x * f) / f
    }

    // --- Main function ---
    function datafromSim(timeVal) {
        // Monitor only when tasks are active
        if (currentState !== TestScript.TaskState.TasksActive)
            return

        var r = roundN(timeVal, decimals)

        // Initialize on first data
        if (lastRoundedTime < 0) {
            lastRoundedTime = r
            sameRunCount = 1
            return
        }

        // --- Case 1: Time increasing normally ---
        if (r > lastRoundedTime) {
            sameRunCount = 1

            // Resume if previously auto-paused and button is enabled
            if (autoPauseActive && taskModel.isPaused && pauseResumeBtn.enabled) {
                pauseResumeBtn.clicked()
                autoPauseActive = false
            }

            lastRoundedTime = r
            return
        }

        // --- Case 2: Time same (possible stall) ---
        if (r === lastRoundedTime) {
            sameRunCount += 1

            // If same value repeated enough times → pause
            if (sameRunCount >= requiredSameCount) {
                if (!autoPauseActive && !taskModel.isPaused && pauseResumeBtn.enabled) {
                    pauseResumeBtn.clicked()
                    autoPauseActive = true
                }
                sameRunCount = 0 // reset counter after triggering
            }
            return
        }

        // --- Case 3: Time went backwards (reset or loop) ---
        if (r < lastRoundedTime) {
            sameRunCount = 1
            lastRoundedTime = r
            return
        }
    }

    // --- Unified pause/resume trigger (uses your button when possible) ---
    function clickPauseResumeSafely(targetPaused) {
        const canUseButton =
            pauseResumeBtn.enabled &&
            currentState === TestScript.TaskState.TasksActive &&
            !taskModel.hasActiveTask

        if (taskModel.isPaused !== targetPaused) {
            if (canUseButton) {
                pauseResumeBtn.clicked()      // simulate real press
            } else {
                taskModel.setPaused(targetPaused) // fallback to model
            }
        }
    }

    // Audio players

    MediaPlayer {
        id: t1Sound
        source: "qrc:/audio/audio/T1.mp3"
        audioOutput: AudioOutput { id: t1SoundOut }
    }

    MediaPlayer {
            id: t5Sound
            source: taskModel.repFilter === 1? "qrc:/audio/audio/T5_24.mp3"
                  : taskModel.repFilter === 2? "qrc:/audio/audio/T5_28.mp3"
                  : taskModel.repFilter === 3? "qrc:/audio/audio/T5_24.mp3"
                  : taskModel.repFilter === 4? "qrc:/audio/audio/T5_20.mp3"
                  : taskModel.repFilter === 5? "qrc:/audio/audio/T5_16.mp3"
                  : taskModel.repFilter === 6? "qrc:/audio/audio/T5_20.mp3"
                  : taskModel.repFilter === 7? "qrc:/audio/audio/T5_24.mp3"
                  : taskModel.repFilter === 8? "qrc:/audio/audio/T5_28.mp3"
                  : taskModel.repFilter === 9? "qrc:/audio/audio/T5_24.mp3" : "qrc:/audio/audio/T5_20.mp3"
            audioOutput: AudioOutput { id: t5SoundOut }
        }

    MediaPlayer {
        id: t9Sound
        source: "qrc:/audio/audio/T9.mp3"
        audioOutput: AudioOutput { id: t9SoundOut }
    }

    MediaPlayer {
        id: beepSound
        source: "qrc:/audio/audio/Beep.mp3"
        audioOutput: AudioOutput { id: beepSoundOut }
    }

    MediaPlayer {
            id: t11Sound
            source: taskModel.repFilter === 1? "qrc:/audio/audio/T11_Bangor.mp3"
                  : taskModel.repFilter === 2? "qrc:/audio/audio/T11_Bolton.mp3"
                  : taskModel.repFilter === 3? "qrc:/audio/audio/T11_Boston.mp3"
                  : taskModel.repFilter === 4? "qrc:/audio/audio/T11_Durham.mp3"
                  : taskModel.repFilter === 5? "qrc:/audio/audio/T11_Exeter.mp3"
                  : taskModel.repFilter === 6? "qrc:/audio/audio/T11-Gaydon.mp3"
                  : taskModel.repFilter === 7? "qrc:/audio/audio/T11_Woking.mp3"
                  : taskModel.repFilter === 8? "qrc:/audio/audio/T11_Oxford.mp3"
                  : taskModel.repFilter === 9? "qrc:/audio/audio/T11_Marlow.mp3" : "qrc:/audio/audio/T11_Oldham.mp3"
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
        id: t18Sound
        source: "qrc:/audio/audio/T18_Split.mp3"
        audioOutput: AudioOutput { id: t18SoundOut }
    }

    MediaPlayer {
            id: t21Sound
            source: taskModel.repFilter === 1? "qrc:/audio/audio/T21_Virgin Radio UK.mp3"
                  : taskModel.repFilter === 2? "qrc:/audio/audio/T21_Times Radio.mp3"
                  : taskModel.repFilter === 3? "qrc:/audio/audio/T21_Classic FM.mp3"
                  : taskModel.repFilter === 4? "qrc:/audio/audio/T21_Talksports.mp3"
                  : taskModel.repFilter === 5? "qrc:/audio/audio/T21_LBC.mp3"
                  : taskModel.repFilter === 6? "qrc:/audio/audio/T21_Radio X.mp3"
                  : taskModel.repFilter === 7? "qrc:/audio/audio/T21_Virgin Radio UK.mp3"
                  : taskModel.repFilter === 8? "qrc:/audio/audio/T21_Classic FM.mp3"
                  : taskModel.repFilter === 9? "qrc:/audio/audio/T21_Times Radio.mp3" : "qrc:/audio/audio/T21_Talksports.mp3"
            audioOutput: AudioOutput { id: t21SoundOut }
        }

    MediaPlayer {
            id: t16Sound
            source: taskModel.repFilter%2 === 0? "qrc:/audio/audio/T16_Left.mp3"
                  : "qrc:/audio/audio/T16_Right.mp3"
            audioOutput: AudioOutput { id: t16SoundOut }
        }
    MediaPlayer {
            id: t22Sound
            source: taskModel.repFilter%2 === 0? "qrc:/audio/audio/T22_Comfort.mp3"
                  : "qrc:/audio/audio/T22_Dynamic.mp3"
            audioOutput: AudioOutput { id: t22SoundOut }
        }
    MediaPlayer {
            id: t19Sound
            source: taskModel.repFilter%2 === 0? "qrc:/audio/audio/T19_On.mp3"
                  : "qrc:/audio/audio/T19_Off.mp3"
            audioOutput: AudioOutput { id: t19SoundOut }
        }

    MediaPlayer {
        id: t23Sound
        source: "qrc:/audio/audio/T23.mp3"
        audioOutput: AudioOutput { id: t23SoundOut }
    }
    // Timer logic _for baseline and end drive timer
    property int elapsedSeconds: 0
    Timer {
        id: elapsedTimer
        interval: 1000 // 1 second
        repeat: true
        running: false
        onTriggered: {
            elapsedSeconds++
        }
    }

    // Timer logic - for activating next tsak
    Timer {
        id: autoNextTimer
    //    interval: 10000
        interval: 3000
        repeat: false
        onTriggered: {
            if (!taskModel.hasActiveTask && !taskModel.isPaused &&
                currentState === TestScript.TaskState.TasksActive) {
                nextTaskBtn.clicked()
            }
        }
    }


    Column {
        spacing: 1

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
                    elapsedSeconds = 0
                    elapsedTimer.start()
                    // Save current rep filter
                    savedRepFilter = taskModel.repFilter

                    // ✅ Set filter to -1 to show only baseline
                    taskModel.repFilter = -1
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {
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
                    elapsedTimer.stop()
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
                    elapsedSeconds = 0
                    elapsedTimer.start()
                    // Save current rep filter
                    savedRepFilter = taskModel.repFilter

                    // ✅ Set filter to -2 to show only experienced
                    taskModel.repFilter = -2
                    taskModel.setActiveTaskByIndex(0)

                    if (taskModel.currentActiveTask) {
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
                    elapsedTimer.stop()
                    // ✅ Restore previous rep filter
                    taskModel.repFilter = savedRepFilter

                    currentState = TestScript.TaskState.Idle
                }
            }

            // Visual separator
            Rectangle {
                width: 1
                height: parent.height
                color: "gray"
            }
            // Timer display next to the button
            Text {
                id: timerDisplay
                font.pixelSize: 16
                font.bold: true
                color: elapsedTimer.running ? "#00AA00" : "#666666"
                text: {
                    var hours = Math.floor(elapsedSeconds / 3600)
                    var minutes = Math.floor((elapsedSeconds % 3600) / 60)
                    var seconds = elapsedSeconds % 60

                    if (hours > 0) {
                        return String(hours).padStart(2, '0') + ":" +
                               String(minutes).padStart(2, '0') + ":" +
                               String(seconds).padStart(2, '0')
                    } else {
                        return String(minutes).padStart(2, '0') + ":" +
                               String(seconds).padStart(2, '0')
                    }
                }
            }


            // Visual separator
            Rectangle {
                width: 1
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
            spacing: 10


            ComboBox {
                id: repSelector
                width: 100
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
                text: "Completed Task"
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
            Button {
                id: partialEndTaskBtn
                text: "Partial Completed Task"
                enabled: currentState === TestScript.TaskState.TasksActive && taskModel.hasActiveTask
                onClicked: {
                    expdesignform.triggerEndPartial()

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

            Button {
                id: incorrectEndTaskBtn
                text: "Incomplete Task"
                enabled: currentState === TestScript.TaskState.TasksActive && taskModel.hasActiveTask
                onClicked: {
                    expdesignform.triggerEndIncomplete()

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
//            Button {
//                id: stopAllTasksBtn
//                text: "Stop All Tasks"
//                enabled: currentState === TestScript.TaskState.TasksActive
//                onClicked: {
//                    if (taskModel.hasActiveTask) {
//                        expdesignform.triggerEnd()
//                        taskModel.clearActiveTask()
//                    }
//                    currentState = TestScript.TaskState.Idle
//                    taskModel.setPaused(false)
//                    autoNextTimer.stop()

//                    // Restore rep filter
//                    taskModel.repFilter = savedRepFilter
//                }
//            }
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
            Rectangle { width: 60; height: 18; color: "lightgray"; Text { anchors.centerIn: parent; text: "Task Order" } }
            Rectangle { width: 40; height: 18; color: "lightgray"; Text { anchors.centerIn: parent; text: "Abs#" } }
            Rectangle { width: 120; height: 18; color: "lightgray"; Text { anchors.centerIn: parent; text: "Name" } }
            Rectangle { width: 40; height: 18; color: "lightgray"; Text { anchors.centerIn: parent; text: "Comp" } }
            Rectangle { width: 90; height: 18; color: "lightgray"; Text { anchors.centerIn: parent; text: "Control" } }
        }

        // Task list
        ListView {
            id: taskView
            width: parent.width
            height: 400
            model: taskModel
            clip: true

            delegate: Row {
                height: 17
                spacing: 40

                Rectangle {
                    width: 60; height: 17
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.taskOrder }
                }
                Rectangle {
                    width: 40; height: 17
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.absoluteTaskNum }
                }
                Rectangle {
                    width: 120; height: 17
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.name; wrapMode: Text.WordWrap }
                }
                Rectangle {
                    width: 40; height: 17
                    color: model.isActive ? "lightgreen" : "transparent"
                    border.width: 1
                    Text { anchors.centerIn: parent; text: model.complexity }
                }
                Rectangle {
                    width: 90; height: 17
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

        if (taskName === "Navigation")            playBeepThen(t11Sound)
        else if (taskName === "External Temp")    playBeepThen(t1Sound)
        else if (taskName === "Temp adjust")      playBeepThen(t5Sound)
        else if (taskName === "Split Airflow")    playBeepThen(t18Sound)
        else if (taskName === "wash/wipe")        playBeepThen(t14Sound)
        else if (taskName === "flick wipe")       playBeepThen(t12Sound)
        else if (taskName === "pause media") {
            tcpServer.send(14, true)
            playBeepThen(t9Sound)
            // If those tcpServer calls must be AFTER the instruction starts,
            // move them into the beep completion handler instead.
            tcpServer.send(12, true)
        }
        else if (taskName === "Fuel check")       playBeepThen(t23Sound)
        else if (taskName === "Radio selection") {
            tcpServer.send(13, true)
            playBeepThen(t21Sound)
        }
        else if (taskName === "give indicator")   playBeepThen(t16Sound)
        else if (taskName === "Driver mode")      playBeepThen(t22Sound)
        else if (taskName === "Country Road")     playBeepThen(t19Sound)
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
    Connections {
            target: beepSound
            onPlaybackStateChanged: {
                // In Qt 5.15: MediaPlayer.StoppedState when it finishes
                if (beepSound.playbackState === MediaPlayer.StoppedState && nextAfterBeep) {
                    nextAfterBeep.play()
                    nextAfterBeep = null
                }
            }
            // (Optional) handle errors
            onErrorOccurred: {
                console.warn("Beep error:", beepSound.errorString)
                if (nextAfterBeep) { nextAfterBeep.play(); nextAfterBeep = null }
            }
        }
    signal requestCheckTask(int taskNum, bool state)
}
