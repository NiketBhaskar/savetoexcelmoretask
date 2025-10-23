#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include <QUdpSocket>

class QUdpSocket;

class UDPSocket : public QObject
{
    Q_OBJECT

        public:
            explicit UDPSocket(QObject *parent = 0);
            ~UDPSocket();
            QUdpSocket *udpSocket;
            void ConnectMultiCast (int port);
            void ConnectSingle (int port);
            void ConnectSingleAltAddress (int port, QString address);
            void sendCommand(QString message,QString ipaddress, int port);
            void SetAsMultiCast();

        signals:
            void messageReceived (QByteArray message);

        public slots:
            void readyRead();

        private:



};

#endif // UDPSOCKET_H
