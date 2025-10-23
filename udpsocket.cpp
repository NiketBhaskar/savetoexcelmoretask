#include "udpsocket.h"
#include <QDebug>
#include <QVariant>


UDPSocket::UDPSocket(QObject *parent) :
    QObject(parent)
{
    if (udpSocket != NULL)
        udpSocket = NULL;
}

UDPSocket::~UDPSocket()
{
    if (udpSocket != NULL)
    {
        delete udpSocket;
        udpSocket = NULL;
    }
}

void UDPSocket::ConnectMultiCast (int port)
{
    udpSocket = new QUdpSocket(this);

    QHostAddress groupAddress = QHostAddress("224.0.0.0");

    udpSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ReuseAddressHint);

    udpSocket->joinMulticastGroup(groupAddress);

    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readyRead()));
}

void UDPSocket::ConnectSingle (int port)
{
    udpSocket = new QUdpSocket(this);

    udpSocket->bind(port, QUdpSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readyRead()));
}

void UDPSocket::ConnectSingleAltAddress (int port, QString address)
{
    udpSocket = new QUdpSocket(this);

    udpSocket->bind(QHostAddress(address), port, QUdpSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readyRead()));
}

void UDPSocket::SetAsMultiCast()
{
    QVariant v;

    v = 1;

    udpSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
}

void UDPSocket::sendCommand(QString message, QString ipaddress, int port)
{
    QByteArray Data;

    QHostAddress temp;

    //char Data_buff;

    QByteArray ba = message.toLatin1();
    //const char *c_str2 = ba.data();

    if (ipaddress.isEmpty())
    {
        temp = QHostAddress::LocalHost;
    }
    else
    {
        temp = QHostAddress(ipaddress);
    }

    //Data.append(message);

    // Sends the datagram datagram
    // to the host address and at port.
    // udpSocket->writeDatagram(Data,temp,port);
    udpSocket->writeDatagram(ba,temp,port);
}

void UDPSocket::readyRead()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(udpSocket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).
    udpSocket->readDatagram(buffer.data(), buffer.size(),&sender, &senderPort);

    emit messageReceived(buffer);
}



