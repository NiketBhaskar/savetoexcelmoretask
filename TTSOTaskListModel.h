#ifndef TTSOTASKLISTMODEL_H
#define TTSOTASKLISTMODEL_H

#include <QAbstractListModel>
#include "TTSOTask.h"

class TTSOTaskListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int participantFilter READ participantFilter WRITE setParticipantFilter NOTIFY filterChanged)
    Q_PROPERTY(int repFilter READ repFilter WRITE setRepFilter NOTIFY filterChanged)
    Q_PROPERTY(bool hasActiveTask READ hasActiveTask NOTIFY hasActiveTaskChanged)
    Q_PROPERTY(TTSOTask* currentActiveTask READ currentActiveTask NOTIFY activeTaskChanged)
    Q_PROPERTY(bool isPaused READ isPaused WRITE setPaused NOTIFY pauseStateChanged)

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

    explicit TTSOTaskListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setTasks(const QList<TTSOTask*> &tasks);

    // Properties
    TTSOTask* currentActiveTask() const;
    bool hasActiveTask() const;
    int participantFilter() const { return m_participantFilter; }
    int repFilter() const { return m_repFilter; }
    bool isPaused() const;

    // Invokable methods
    Q_INVOKABLE void setActiveTask(int participantID, int repNumber, int taskOrder);
    Q_INVOKABLE void setActiveTaskByIndex(int filteredIndex);
    Q_INVOKABLE void setActiveTaskByAbsNum(int absNum);
    Q_INVOKABLE void clearActiveTask();
    Q_INVOKABLE void setPaused(bool paused);

public slots:
    void setParticipantFilter(int participant);
    void setRepFilter(int rep);

signals:
    void filterChanged();
    void activeTaskChanged(TTSOTask *task);
    void hasActiveTaskChanged();
    void pauseStateChanged(bool paused);

private:
    QList<TTSOTask*> m_allTasks;      // all tasks
    QList<TTSOTask*> m_filteredTasks; // filtered view
    TTSOTask *m_currentActiveTask;
    int m_activeParticipant;
    int m_activeRep;
    int m_activeTaskOrder;
    int m_participantFilter;
    int m_repFilter;
    bool m_isPaused;

    void applyFilter();
};

#endif // TTSOTASKLISTMODEL_H
