#include "jercserialcommv10.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>


jercserialcommv10::jercserialcommv10(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort();
    connect(serial, &QSerialPort::readyRead, this, readData);

}

jercserialcommv10::~jercserialcommv10()
{

    if(serial->isOpen())
        serial->close();
}


int jercserialcommv10::setCOMport(bool getfromfile, QString comport)
{

    QFile settingfile ("settings.txt");

    if(getfromfile)
    {
    if(settingfile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&settingfile);
        ComPort = in.readLine();
        settingfile.close();
        return noError;

    }
    else
    {
        ComPort = comport;
        return fileopenError;
    }
    }
    else
    {
        ComPort = comport;
        return noError;
    }
}

int jercserialcommv10::openPort(QSerialPort::BaudRate baudrate, QSerialPort::DataBits databit,
                                QSerialPort::Parity parity, QSerialPort::StopBits stopbit,
                                QSerialPort::FlowControl flowcontrol)
{
    serial->setPortName(ComPort);
    serial->setBaudRate(baudrate);
    serial->setDataBits(databit);
    serial->setParity(parity);
    serial->setStopBits(stopbit);
    serial->setFlowControl(flowcontrol);


    if(serial->open(QIODevice::ReadWrite))
    {

        serial->setDataTerminalReady(true);
        serial->flush();
        return noError;

    }
    else
    {

        return portopenError;
    }
}

void jercserialcommv10::readData()
{
    QByteArray temp = serial->readAll();

    QString data = temp.toStdString().c_str();

    emit receivedDataAsByte(temp);




    QStringList params;
    messageBuilder += data;

    if(messageBuilder.contains("\n") && messageBuilder.length() > 8)
    {
        params = messageBuilder.split("\n");
    }

    if(params.count() >= 1)
    {
        messageBuilder = "";
        emit receivedData(params[0]);


        if(params.count() >= 2)
        {
            messageBuilder = params[1];
        }
    }
}

void jercserialcommv10::writeData(const QByteArray &data)
{
    serial->write(data);
}

void jercserialcommv10::writeData(const QString &data)
{
    QByteArray temp = data.toUtf8();
    serial->write(temp);
}

