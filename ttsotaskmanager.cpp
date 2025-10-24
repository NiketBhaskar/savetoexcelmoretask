#include "TTSOTaskManager.h"
#include <QMap>
#include <QStringList>
#include <QHash>

TTSOTaskManager::TTSOTaskManager(QObject *parent) : QObject(parent) {}

void TTSOTaskManager::loadHardcodedTasks()
{
    // Metadata from Sheet1 (absolute_task_number -> [name, complexity, controlTypes])
    struct TaskMeta {
        QString name;
        double complexity;
        QString controlType;
        int absoluteNum;
    };

    QHash<QString, TaskMeta> meta;

    meta["T11"] = { "Navigation", 21.0, "Centre Screen", 11 };
    meta["T1"]  = { "External Temp", 14.33, "Cluster", 1 };
    meta["T5"]  = { "Temp adjust", 17.83, "Centre Screen", 5 };
    meta["T18"] = { "Split Airflow", 27.67, "Centre Screen", 18 };
    meta["T14"] = { "wash/wipe", 12.0, "Stalks" , 14};
    meta["T12"] = { "flick wipe", 12.0, "Stalks", 12 };
    meta["T9"]  = { "pause media", 11.0, "Centre Screen", 9 };
    meta["T23"] = { "Fuel check", 10.0, "Centre Screen", 23 };
    meta["T21"] = { "Radio selection", 18.0, "Centre Screen", 21 };
    meta["T16"] = { "give indicator", 15.0, "Stalks", 16 };
    meta["T22"] = { "Driver mode", 18.0, "Centre Screen", 22 };
    meta["T19"] = { "Country Road", 12.0, "Centre Console", 19 };

    // Define 10 rep sequences (10 reps × 12 tasks = 120 tasks)
    QList<QStringList> repSequences;
    repSequences.append({ "T11","T1","T5","T18","T14","T12","T9","T23","T21","T16","T22","T19" });  // Rep 1
    repSequences.append({ "T1","T18","T11","T12","T5","T23","T14","T16","T9","T19","T21","T22" });  // Rep 2
    repSequences.append({ "T18","T12","T1","T23","T11","T16","T5","T19","T14","T22","T9","T21" });  // Rep 3
    repSequences.append({ "T12","T23","T18","T16","T1","T19","T11","T22","T5","T21","T14","T9" });  // Rep 4
    repSequences.append({ "T23","T16","T12","T19","T18","T22","T1","T21","T11","T9","T5","T14" });  // Rep 5
    repSequences.append({ "T16","T19","T23","T22","T12","T21","T18","T9","T1","T14","T11","T5" });  // Rep 6
    repSequences.append({ "T19","T22","T16","T21","T23","T9","T12","T14","T18","T5","T1","T11" });  // Rep 7
    repSequences.append({ "T22","T21","T19","T9","T16","T14","T23","T5","T12","T11","T18","T1" });  // Rep 8
    repSequences.append({ "T21","T9","T22","T14","T19","T5","T16","T11","T23","T1","T12","T18" });  // Rep 9
    repSequences.append({ "T9","T14","T21","T5","T22","T11","T19","T1","T16","T18","T23","T12" });  // Rep 10

    // Build tasks for each participant (1-24)
    for (int p = 1; p <= 24; ++p) {
        int taskOrder = 1;

        // Task 1: Baseline Drive (Absolute #1)
        TTSOTask *baselineTask = new TTSOTask(this);
        baselineTask->setName("Baseline Drive");
        baselineTask->setParticipantID(p);
        baselineTask->setAbsoluteTaskNum(-1);
        baselineTask->setTaskOrder(taskOrder++);
        baselineTask->setRepNumber(-1);
        baselineTask->setTaskComplexity("baseline");
        baselineTask->setControlTypes("baseline");
        baselineTask->setActualTouchCount(-1);
        baselineTask->setMadeTouchCount(-1);
        baselineTask->setErrorMade(false);
        m_tasks.append(baselineTask);

        // Tasks 2-121: The 120 TTSO tasks (10 reps × 12 tasks each)
        for (int rep = 0; rep < 10; ++rep) {
            QStringList sequence = repSequences[rep];
            for (int i = 0; i < sequence.size(); ++i) {
                QString taskID = sequence[i];
                TTSOTask *task = new TTSOTask(this);

                task->setName(meta.contains(taskID) ? meta[taskID].name : "Not defined");
                task->setParticipantID(p);
                task->setAbsoluteTaskNum(meta.contains(taskID) ? meta[taskID].absoluteNum : 0 );  // Sequential 2-121
                task->setTaskOrder(taskOrder++);
                task->setRepNumber(rep + 1);  // Rep 1-10
                task->setTaskComplexity(meta.contains(taskID) ? QString::number(meta[taskID].complexity) : "Not defined");
                task->setControlTypes(meta.contains(taskID) ? meta[taskID].controlType : "Not defined");
                task->setActualTouchCount(-1);
                task->setMadeTouchCount(-1);
                task->setErrorMade(false);

                m_tasks.append(task);
            }
        }

        // Task 122: Experienced Drive (Absolute #122)
        TTSOTask *expTask = new TTSOTask(this);
        expTask->setName("Experienced Drive");
        expTask->setParticipantID(p);
        expTask->setAbsoluteTaskNum(-2);
        expTask->setTaskOrder(taskOrder);
        expTask->setRepNumber(-2);
        expTask->setTaskComplexity("experienced");
        expTask->setControlTypes("experienced");
        expTask->setActualTouchCount(-1);
        expTask->setMadeTouchCount(-1);
        expTask->setErrorMade(false);
        m_tasks.append(expTask);
    }
}
