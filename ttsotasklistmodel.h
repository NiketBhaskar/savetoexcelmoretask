#ifndef TTSOTASKLISTMODEL_H
#define TTSOTASKLISTMODEL_H

#include <QAbstractListModel>
#include "TTSOTask.h"

class TTSOTaskListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int participantFilter READ participantFilter WRITE setParticipantFilter NOTIFY filterChanged)
    Q_PROPERTY(int repFilter READ repFilter WRITE setRepFilter NOTIFY filterChanged)

public:
    enum TaskRoles {
        ParticipantRole = Qt::UserRole + 1,
        RepRole,
        TaskOrderRole,
        AbsoluteTaskNumRole,
        NameRole,
        ComplexityRole,
        ControlTypesRole,
        IsActiveRole
    };
    Q_ENUM(TaskRoles)
    TTSOTask* currentActiveTask() const;
    explicit TTSOTaskListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setTasks(const QList<TTSOTask*> &tasks);

    Q_INVOKABLE void setActiveTask(int participantID, int repNumber, int taskOrder);
    Q_INVOKABLE void clearActiveTask();

    int participantFilter() const { return m_participantFilter; }
    int repFilter() const { return m_repFilter; }

public slots:
    void setParticipantFilter(int participant);
    void setRepFilter(int rep);

signals:
    void filterChanged();
    void activeTaskChanged(TTSOTask *task);

private:
    QList<TTSOTask*> m_allTasks;      // all tasks
    QList<TTSOTask*> m_filteredTasks; // filtered view
    TTSOTask *m_currentActiveTask = nullptr;
    int m_activeParticipant;
    int m_activeRep;
    int m_activeTaskOrder;
    int m_participantFilter;
    int m_repFilter;

    void applyFilter();
};

#endif // TTSOTASKLISTMODEL_H
