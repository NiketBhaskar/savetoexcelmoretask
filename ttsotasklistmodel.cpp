#include "TTSOTaskListModel.h"

TTSOTaskListModel::TTSOTaskListModel(QObject *parent)
    : QAbstractListModel(parent),
      m_currentActiveTask(nullptr),
      m_activeParticipant(-1),
      m_activeRep(-1),
      m_activeTaskOrder(-1),
      m_participantFilter(-1),
      m_repFilter(-1)
{}

int TTSOTaskListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_filteredTasks.size();
}

QVariant TTSOTaskListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_filteredTasks.size())
        return QVariant();

    TTSOTask *task = m_filteredTasks.at(index.row());
    switch (role) {
    case ParticipantRole:     return task->participantID();
    case RepRole:             return task->repNumber();
    case TaskOrderRole:       return task->taskOrder();
    case AbsoluteTaskNumRole: return task->absoluteTaskNum();
    case NameRole:            return task->name();
    case ComplexityRole:      return task->taskComplexity();
    case ControlTypesRole:    return task->controlTypes();
    case IsActiveRole:        return (task == m_currentActiveTask);
    default:                  return QVariant();
    }
}

QHash<int, QByteArray> TTSOTaskListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ParticipantRole]     = "participantID";
    roles[RepRole]             = "repNumber";
    roles[TaskOrderRole]       = "taskOrder";
    roles[AbsoluteTaskNumRole] = "absoluteTaskNum";
    roles[NameRole]            = "name";
    roles[ComplexityRole]      = "complexity";
    roles[ControlTypesRole]    = "controlTypes";
    roles[IsActiveRole]        = "isActive";
    return roles;
}

void TTSOTaskListModel::setTasks(const QList<TTSOTask*> &tasks) {
    beginResetModel();
    m_allTasks = tasks;
    applyFilter();
    endResetModel();
}

void TTSOTaskListModel::setActiveTask(int participantID, int repNumber, int taskOrder) {
    m_activeParticipant = participantID;
    m_activeRep = repNumber;
    m_activeTaskOrder = taskOrder;

    m_currentActiveTask = nullptr;
    for (TTSOTask *task : m_filteredTasks) {
        if (task->participantID() == participantID &&
            task->repNumber() == repNumber &&
            task->taskOrder() == taskOrder) {
            m_currentActiveTask = task;
            emit activeTaskChanged(task);   // 🔴 let listeners know
            break;
        }
    }

    if (!m_filteredTasks.isEmpty()) {
        emit dataChanged(index(0,0), index(m_filteredTasks.size()-1,0), {IsActiveRole});
    }
}

void TTSOTaskListModel::setParticipantFilter(int participant) {
    if (m_participantFilter == participant) return;
    m_participantFilter = participant;
    applyFilter();
    emit filterChanged();
}

void TTSOTaskListModel::setRepFilter(int rep) {
    if (m_repFilter == rep) return;
    m_repFilter = rep;
    applyFilter();
    emit filterChanged();
}

void TTSOTaskListModel::applyFilter() {
    beginResetModel();
    m_filteredTasks.clear();

    for (TTSOTask *task : m_allTasks) {
        bool matchParticipant = (m_participantFilter == -1 || task->participantID() == m_participantFilter);
        bool matchRep         = (m_repFilter == -1 || task->repNumber() == m_repFilter);

        if (matchParticipant && matchRep) {
            m_filteredTasks.append(task);
        }
    }

    endResetModel();
}

TTSOTask* TTSOTaskListModel::currentActiveTask() const {
    return m_currentActiveTask;
}
void TTSOTaskListModel::clearActiveTask()
{
    if (m_currentActiveTask) {
        m_currentActiveTask = nullptr;
        m_activeParticipant = -1;
        m_activeRep = -1;
        m_activeTaskOrder = -1;
        emit dataChanged(index(0,0), index(m_filteredTasks.size()-1,0), {IsActiveRole});
        emit activeTaskChanged(nullptr);   // ✅ tell eyeDataInput "no task"
    }
}
