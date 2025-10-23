#include "TaskWebSocketServer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

TaskWebSocketServer::TaskWebSocketServer(QObject *parent)
    : QObject(parent),
      m_server(new QWebSocketServer("TaskServer", QWebSocketServer::NonSecureMode, this)) {}

bool TaskWebSocketServer::startServer(quint16 port) {
    if (!m_server->listen(QHostAddress::Any, port)) {
        qWarning() << "WebSocket server failed to start:" << m_server->errorString();
        return false;
    }
    connect(m_server, &QWebSocketServer::newConnection, this, &TaskWebSocketServer::onNewConnection);
    qDebug() << "WebSocket server listening on port" << port;
    return true;
}

void TaskWebSocketServer::onNewConnection() {
    QWebSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QWebSocket::textMessageReceived, this, &TaskWebSocketServer::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected, this, &TaskWebSocketServer::onSocketDisconnected);
    m_clients << socket;
    qDebug() << "Client connected.";
}

void TaskWebSocketServer::onTextMessageReceived(QString message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();

    if (obj["event"] == "taskEnded") {
        int absTaskNum = obj["absTaskNum"].toInt();
        int repNum = obj["repNum"].toInt();
        emit taskEndedFromClient(absTaskNum, repNum);
        qDebug() << "Task end received from client: absTaskNum" << absTaskNum << "repNum" << repNum;
    }
}

void TaskWebSocketServer::onSocketDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket*>(sender());
    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();
    }
    qDebug() << "Client disconnected.";
}

void TaskWebSocketServer::onTaskStarted(TTSOTask *task) {
    if (!task) return;
    QJsonObject obj;
    obj["event"] = "taskStarted";
    obj["participantId"] = task->participantID();
    obj["absTaskNum"] = task->absoluteTaskNum();
    obj["taskOrder"] = task->taskOrder();
    obj["repNum"] = task->repNumber();
    obj["taskName"] = task->name();
    obj["x"] = 0; obj["y"] = 0; obj["z"] = 0; // placeholder for extra data

    QJsonDocument doc(obj);
    for (QWebSocket *client : m_clients) {
        client->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    }
}

void TaskWebSocketServer::onTaskEnded(TTSOTask *task) {
    if (!task) return;
    QJsonObject obj;
    obj["event"] = "taskEnded";
    obj["absTaskNum"] = task->absoluteTaskNum();
    obj["repNum"] = task->repNumber();

    QJsonDocument doc(obj);
    for (QWebSocket *client : m_clients) {
        client->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    }
}
