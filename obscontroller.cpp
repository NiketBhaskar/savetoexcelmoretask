#include "OBSController.h"
#include <QDebug>

OBSController::OBSController(QObject *parent) : QObject(parent)
{
    connect(&m_socket, &QWebSocket::connected, this, &OBSController::onConnected);
    connect(&m_socket, &QWebSocket::disconnected, this, &OBSController::onDisconnected);
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &OBSController::onTextMessageReceived);
}

void OBSController::connectToOBS(const QUrl &url)
{
    qDebug() << "[OBS] Connecting to" << url.toString();
    m_socket.open(url);
}

void OBSController::onConnected()
{
    qDebug() << "[OBS] Connected";
    emit connected();
}

void OBSController::onDisconnected()
{
    qDebug() << "[OBS] Disconnected";
    emit disconnected();
}

void OBSController::onTextMessageReceived(const QString &message)
{
    qDebug() << "[OBS] Response:" << message;
    if (message.contains("StartRecording")) {
        emit recordingStarted();
    } else if (message.contains("StopRecording")) {
        emit recordingStopped();
    }
}

void OBSController::sendRequest(const QString &requestType, const QString &messageId, const QJsonObject &extra)
{
    QJsonObject obj = extra;
    obj["request-type"] = requestType;
    obj["message-id"] = messageId;

    QString msg = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    qDebug() << "[OBS] Sending:" << msg;
    m_socket.sendTextMessage(msg);
}

void OBSController::startRecording(const QString &filename)
{
    if (!m_socket.isValid()) {
        qWarning() << "[OBS] Not connected";
        return;
    }

    QJsonObject extra;
    if (!filename.isEmpty()) {
        extra["filename"] = filename;
    }
    sendRequest("StartRecording", "startRec", extra);
}

void OBSController::stopRecordingDelayed(int msecDelay)
{
    if (!m_socket.isValid()) {
        qWarning() << "[OBS] Not connected";
        return;
    }

    if (msecDelay <= 0) {
        sendRequest("StopRecording", "stopRec");
    } else {
        QTimer::singleShot(msecDelay, [this]() {
            sendRequest("StopRecording", "stopRec");
        });
    }
}
