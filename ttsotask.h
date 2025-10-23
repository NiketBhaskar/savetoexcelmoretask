#ifndef TTSOTASK_H
#define TTSOTASK_H

#include <QObject>

class TTSOTask : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int participantID READ participantID WRITE setParticipantID NOTIFY participantIDChanged)
    Q_PROPERTY(int absoluteTaskNum READ absoluteTaskNum WRITE setAbsoluteTaskNum NOTIFY absoluteTaskNumChanged)
    Q_PROPERTY(int taskOrder READ taskOrder WRITE setTaskOrder NOTIFY taskOrderChanged)
    Q_PROPERTY(int repNumber READ repNumber WRITE setRepNumber NOTIFY repNumberChanged)
    Q_PROPERTY(double totalElapsedTime READ totalElapsedTime WRITE setTotalElapsedTime NOTIFY totalElapsedTimeChanged)
    Q_PROPERTY(QString taskComplexity READ taskComplexity WRITE setTaskComplexity NOTIFY taskComplexityChanged)
    Q_PROPERTY(QString controlTypes READ controlTypes WRITE setControlTypes NOTIFY controlTypesChanged)

    Q_PROPERTY(int actualTouchCount READ actualTouchCount WRITE setActualTouchCount NOTIFY actualTouchCountChanged)
    Q_PROPERTY(int madeTouchCount READ madeTouchCount WRITE setMadeTouchCount NOTIFY madeTouchCountChanged)
    Q_PROPERTY(bool errorMade READ errorMade WRITE setErrorMade NOTIFY errorMadeChanged)

public:
    explicit TTSOTask(QObject *parent = nullptr);

    // Getters
    QString name() const;
    int participantID() const;
    int absoluteTaskNum() const;
    int taskOrder() const;
    int repNumber() const;
    double totalElapsedTime() const;
    QString taskComplexity() const;
    QString controlTypes() const;

    int actualTouchCount() const;
    int madeTouchCount() const;
    bool errorMade() const;

    // Setters
    void setName(const QString &name);
    void setParticipantID(int id);
    void setAbsoluteTaskNum(int num);
    void setTaskOrder(int order);
    void setRepNumber(int rep);
    void setTotalElapsedTime(double time);
    void setTaskComplexity(const QString &complexity);
    void setControlTypes(const QString &types);

    void setActualTouchCount(int count);
    void setMadeTouchCount(int count);
    void setErrorMade(bool error);

public slots:
    Q_INVOKABLE void checkForErrors();  // check if madeTouchCount > actualTouchCount

signals:
    void nameChanged();
    void participantIDChanged();
    void absoluteTaskNumChanged();
    void taskOrderChanged();
    void repNumberChanged();
    void totalElapsedTimeChanged();
    void taskComplexityChanged();
    void controlTypesChanged();

    void actualTouchCountChanged();
    void madeTouchCountChanged();
    void errorMadeChanged();

private:
    QString m_name;
    int m_participantID;
    int m_absoluteTaskNum;
    int m_taskOrder;
    int m_repNumber;
    double m_totalElapsedTime;
    QString m_taskComplexity;
    QString m_controlTypes;

    int m_actualTouchCount;
    int m_madeTouchCount;
    bool m_errorMade;
};

#endif // TTSOTASK_H
