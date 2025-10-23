#ifndef JERCSERIALCOMMV10_H
#define JERCSERIALCOMMV10_H

#include <QObject>
#include <QSerialPort>

/**
 * @brief The jercserialcommv10 class
 * This is RS232 serial class which can communicate with any RS 232 serial devices
 * including Ardunio
 */
class jercserialcommv10 : public QObject
{
    Q_OBJECT
public:
    explicit jercserialcommv10(QObject *parent = 0);
    ~jercserialcommv10();

    /**
     * @brief noError
     */
    const int noError = 0;
    /**
     * @brief portopenError
     */
    const int portopenError = -1;
    /**
     * @brief fileopenError
     */
    const int fileopenError = -2;


    /**
     * @brief setCOMport
     * @param getfromfile
     * @param comport
     * @return
     */
    int setCOMport(bool getfromfile,QString comport);

    /**
     * @brief openPort
     * @param baudrate
     * @param databit
     * @param parity
     * @param stopbit
     * @param flowcontrol
     * @return
     */
    int openPort (QSerialPort::BaudRate baudrate, QSerialPort::DataBits databit,
                  QSerialPort::Parity parity, QSerialPort::StopBits stopbit, QSerialPort::FlowControl flowcontrol);

    /**
     * @brief writeData
     * @param data
     */
    void writeData(const QByteArray &data);
    /**
     * @brief writeData
     * @param data
     */
    void writeData(const QString &data);

private:


signals:
    void receivedData(QString data);
    void receivedDataAsByte(QByteArray data);

public slots:
    void readData();

private:
    QSerialPort *serial;
    QString ComPort;


    QString messageBuilder;
};

#endif // JERCSERIALCOMMV10_H
