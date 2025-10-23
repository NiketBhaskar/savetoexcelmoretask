#include "TTSOTask.h"
#include <QtGlobal> // for qFuzzyCompare

TTSOTask::TTSOTask(QObject *parent)
    : QObject(parent),
      m_name(""),
      m_participantID(-1),
      m_absoluteTaskNum(0),
      m_taskOrder(0),
      m_repNumber(0),
      m_totalElapsedTime(0.0),
      m_taskComplexity(""),
      m_controlTypes(""),
      m_actualTouchCount(0),
      m_madeTouchCount(0),
      m_errorMade(false)
{}

// Getters
QString TTSOTask::name() const { return m_name; }
int TTSOTask::participantID() const { return m_participantID; }
int TTSOTask::absoluteTaskNum() const { return m_absoluteTaskNum; }
int TTSOTask::taskOrder() const { return m_taskOrder; }
int TTSOTask::repNumber() const { return m_repNumber; }
double TTSOTask::totalElapsedTime() const { return m_totalElapsedTime; }
QString TTSOTask::taskComplexity() const { return m_taskComplexity; }
QString TTSOTask::controlTypes() const { return m_controlTypes; }

int TTSOTask::actualTouchCount() const { return m_actualTouchCount; }
int TTSOTask::madeTouchCount() const { return m_madeTouchCount; }
bool TTSOTask::errorMade() const { return m_errorMade; }

// Setters
void TTSOTask::setName(const QString &name) {
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void TTSOTask::setParticipantID(int id) {
    if (m_participantID != id) {
        m_participantID = id;
        emit participantIDChanged();
    }
}

void TTSOTask::setAbsoluteTaskNum(int num) {
    if (m_absoluteTaskNum != num) {
        m_absoluteTaskNum = num;
        emit absoluteTaskNumChanged();
    }
}

void TTSOTask::setTaskOrder(int order) {
    if (m_taskOrder != order) {
        m_taskOrder = order;
        emit taskOrderChanged();
    }
}

void TTSOTask::setRepNumber(int rep) {
    if (m_repNumber != rep) {
        m_repNumber = rep;
        emit repNumberChanged();
    }
}

void TTSOTask::setTotalElapsedTime(double time) {
    if (!qFuzzyCompare(m_totalElapsedTime, time)) {
        m_totalElapsedTime = time;
        emit totalElapsedTimeChanged();
    }
}

void TTSOTask::setTaskComplexity(const QString &complexity) {
    if (m_taskComplexity != complexity) {
        m_taskComplexity = complexity;
        emit taskComplexityChanged();
    }
}

void TTSOTask::setControlTypes(const QString &types) {
    if (m_controlTypes != types) {
        m_controlTypes = types;
        emit controlTypesChanged();
    }
}

void TTSOTask::setActualTouchCount(int count) {
    if (m_actualTouchCount != count) {
        m_actualTouchCount = count;
        emit actualTouchCountChanged();
        checkForErrors(); // auto-check
    }
}

void TTSOTask::setMadeTouchCount(int count) {
    if (m_madeTouchCount != count) {
        m_madeTouchCount = count;
        emit madeTouchCountChanged();
        checkForErrors(); // auto-check
    }
}

void TTSOTask::setErrorMade(bool error) {
    if (m_errorMade != error) {
        m_errorMade = error;
        emit errorMadeChanged();
    }
}

// New method
void TTSOTask::checkForErrors()
{
    setErrorMade(m_madeTouchCount > m_actualTouchCount);
}
