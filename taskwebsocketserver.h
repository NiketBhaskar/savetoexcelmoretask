#ifndef TASKWEBSOCKETSERVER_H
#define TASKWEBSOCKETSERVER_H
#pragma once
#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include "TTSOTask.h"

class TaskWebSocketServer : public QObject {
    Q_OBJECT
public:
    explicit TaskWebSocketServer(QObject *parent = nullptr);
    bool startServer(quint16 port = 12345);

signals:
    void taskEndedFromClient(int absTaskNum, int repNum);

public slots:
    void onTaskStarted(TTSOTask *task);   // called when Qt starts task
    void onTaskEnded(TTSOTask *task);     // optional, if you also want to notify Android

private slots:
    void onNewConnection();
    void onTextMessageReceived(QString message);
    void onSocketDisconnected();

private:
    QWebSocketServer *m_server;
    QList<QWebSocket*> m_clients;
};

#endif // TASKWEBSOCKETSERVER_H
