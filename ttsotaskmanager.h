#ifndef TTSOTASKMANAGER_H
#define TTSOTASKMANAGER_H

#include <QObject>
#include <QList>
#include "TTSOTask.h"

class TTSOTaskManager : public QObject
{
    Q_OBJECT
public:
    explicit TTSOTaskManager(QObject *parent = nullptr);

    Q_INVOKABLE void loadHardcodedTasks();
    QList<TTSOTask*> tasks() const { return m_tasks; }

private:
    QList<TTSOTask*> m_tasks;
};

#endif // TTSOTASKMANAGER_H
