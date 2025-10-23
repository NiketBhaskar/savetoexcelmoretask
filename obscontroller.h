#ifndef OBSCONTROLLER_H
#define OBSCONTROLLER_H

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>

class OBSController : public QObject
{
    Q_OBJECT

public:
    explicit OBSController(QObject *parent = nullptr);

    void connectToOBS(const QUrl &url = QUrl("ws://127.0.0.1:4444"));
    void startRecording(const QString &filename = QString());
    void stopRecordingDelayed(int msecDelay = 0);

signals:
    void connected();
    void disconnected();
    void recordingStarted();
    void recordingStopped();

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);

private:
    void sendRequest(const QString &requestType, const QString &messageId, const QJsonObject &extra = QJsonObject());

    QWebSocket m_socket;
};

#endif // OBSCONTROLLER_H
