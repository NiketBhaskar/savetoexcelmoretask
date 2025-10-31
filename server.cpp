#include "server.h"
#include <QDebug>

Server::Server(quint16 port, QObject* parent) : QObject(parent) {
    connect(&server_, &QTcpServer::newConnection, this, &Server::onNewConnection);
    server_.listen(QHostAddress::Any, port);
}

QVariantList Server::states() const {
    QVariantList out;
    out.reserve(stateVec_.size());
    for (bool b : stateVec_) out << b;
    return out;
}

void Server::onNewConnection() {
    while (server_.hasPendingConnections()) {
        auto* s = server_.nextPendingConnection();
        clients_ << s;
        connect(s, &QTcpSocket::readyRead,    this, &Server::onReadyRead);
        connect(s, &QTcpSocket::disconnected, this, &Server::onDisconnected);
        qDebug() << "Client connected from" << s->peerAddress().toString();
    }
}
void Server::onReadyRead() {
    auto* s = qobject_cast<QTcpSocket*>(sender());
    while (s && s->canReadLine()) {
        const QByteArray line = s->readLine().trimmed();
        const QList<QByteArray> parts = line.split(' ');
        if (parts.size() >= 2) {
            bool ok1=false, ok2=false;
            int idx  = parts[0].toInt(&ok1);
            int flag = parts[1].toInt(&ok2);
            if (ok1 && ok2 && idx >= 0 && idx < stateVec_.size()) {
                bool checked = (flag == 1);

                // ✅ Special logging for Android screen touches (index 16 unchecked)
                if (idx == 16 && !checked) {
                    qDebug() << "Android SCREEN TOUCH detected (idx 16 unchecked)";
                } else {
                    qDebug() << "Android idx" << idx << "->"
                             << (checked ? "checked" : "unchecked");
                }

                if (stateVec_[idx] != checked) {
                    stateVec_[idx] = checked;
                    emit statesChanged();
                    emit stateUpdated(idx, checked);
                }
            }
        }
    }
}
//void Server::onReadyRead() {
//    auto* s = qobject_cast<QTcpSocket*>(sender());
//    while (s && s->canReadLine()) {
//        const QByteArray line = s->readLine().trimmed();   // e.g. "3 1"
//        const QList<QByteArray> parts = line.split(' ');
//        if (parts.size() >= 2) {
//            bool ok1=false, ok2=false;
//            int idx  = parts[0].toInt(&ok1);
//            int flag = parts[1].toInt(&ok2);
//            if (ok1 && ok2 && idx >= 0 && idx < stateVec_.size()) {
//                bool checked = (flag == 1);
//                // Log ALL incoming messages (moved outside state-change check)
//                                qDebug() << "📱 Android says idx" << idx << "->"
//                                         << (checked ? "CHECKED ✓" : "UNCHECKED ✗")
//                                         << "(raw flag:" << flag << ")";
//                if (stateVec_[idx] != checked) {
//                    stateVec_[idx] = checked;
//                    emit statesChanged();                // updates QML bindings
//                    emit stateUpdated(idx, checked);     // (optional) per-index hook
//                    qDebug() << "   └─ State CHANGED for index" << idx;
//                }
//                else {
//                                    qDebug() << "   └─ State unchanged (duplicate message)";
//                                }
////                qDebug() << "Android says idx" << idx << "->" << checked;
//            }
//        }
//    }
//}

void Server::onDisconnected() {
    auto* s = qobject_cast<QTcpSocket*>(sender());
    clients_.removeAll(s);
    s->deleteLater();
}

void Server::send(int index, bool checked) {
    QByteArray msg = QByteArray::number(index) + " " + (checked ? "1" : "0") + "\n";
    for (auto* s : clients_) {
        if (s->state() == QAbstractSocket::ConnectedState) {
            s->write(msg);
            s->flush();
        }
    }
    // also update local state so UI mirrors what we sent
    if (index >= 0 && index < stateVec_.size()) {
        if (stateVec_[index] != checked) {
            stateVec_[index] = checked;
            emit statesChanged();
            emit stateUpdated(index, checked);
        }
    }
}
