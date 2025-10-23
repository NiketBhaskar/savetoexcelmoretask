#include "ledcontrol.h"
#include <QDebug>

ledcontrol::ledcontrol(QObject *parent) : QObject(parent)
{
    // Configure the serial port

    serialPort = new QSerialPort(this);
    serialPort->setPortName("COM4");  // Set the appropriate port name
    serialPort->setBaudRate(QSerialPort::Baud115200);  // Set the appropriate baud rate
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);

    if(serialPort->open(QIODevice::ReadWrite))
    {
        qDebug() << "Failed to open port:" << serialPort->errorString();
    }
    //DATA INCOMING FROM LED INTERFACE///////////////////////////////////////////////
    connect(timer, SIGNAL(timeout()), this, SLOT(dataTriggerCommand()));

    //CENTRE SCREEN INPUT/////////////////////////////////////////////////////////////
    udpLEDcomms = new UDPSocket(this);                                           //Setup UDP Socket to communicate ADAS messages
    udpLEDcomms->ConnectSingle(7003);                                            //ADAS messages ?
    connect(udpLEDcomms,SIGNAL(messageReceived(QByteArray)),this,SLOT(getProData(QByteArray)));

}

ledcontrol::~ledcontrol()
{
    // if(serialPort->isOpen())//
    serialPort->close();
}

//data is being sent to ESP module directly throught serial comm
void ledcontrol::changePattern(QString desiredPattern)
{
    QByteArray data =  desiredPattern.toLatin1();
    serialPort->write(data);
    //qDebug() <<"led.cpp"<< data ;//working
}

//Created in order to receive data from proto and send the correct message to the comm port
void ledcontrol::getProData(QByteArray LEDpattern)
{
    //qDebug() <<"led.cpp"<< LEDpattern ;//working
    ProtoMessage = LEDpattern;
    //qDebug() << "pattern" <<ProtoMessage;//working
    changePattern(ProtoMessage);
}
void ledcontrol::dataTriggerCommand()                             //Timer Trigger
{
    timer->stop();                                                 //Stop timer and reset
}
