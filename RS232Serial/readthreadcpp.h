#ifndef READTHREADCPP_H
#define READTHREADCPP_H
#include <QThread>
#include <QMutex>
#include <qmath.h>

#include "windows.h"
/**********************************************
 * Class  : readThread
 * Company: JLR Research
 * Author : Jerciline Sundar
 * CDSID  : jsundar
 * Ver    : 0.1.0
 * Date   : 26/11/14
 *
 * Overview:
 * This is a thread class to read the serial communication. The serial handle has
 * to transfer from the communciation open method
 *
 * This class a emit a signal when ever it reads from the serial port
 * This calss only works in the Windows environment
 * Version History:
 *
 *
 * ********************************************/

class readThreadcpp: public QThread
{
    Q_OBJECT

public:
    explicit readThreadcpp(QObject *parent = 0);
    ~readThreadcpp();
    void setHandle(HANDLE ComPort) {m_hComPort = ComPort;}
    void setMode (int mode){m_mode = mode;}
    /*!
     * \brief startThread This function starts the thread
     */
    void startThread();
protected:
    void run();

signals:
    void receivedData(const QByteArray &data);

public slots:
    void stopThread();

private:
    int m_datalength;
    double bytetoFloat(BYTE b0, BYTE b1, BYTE b2, BYTE b3);
//    void byteProcess(BYTE *data, int nLenght);
    void byteProcess(QByteArray data, int nLenght);
    double rpm (double speed, double gear);


private:

    bool m_babort;
    QMutex mutex;
    HANDLE m_hComPort;
    int m_mode;
    BYTE *m_old_data;
    //float m_ratio[7] = {2.66,1.78,1.30,1.0,0.74,0.50,2.90};
    double m_diff_ratio = 3.42;
    int m_max_rpm = 9000;
    double m_tireDiam = 26.6;


};
#endif // READTHREAD_H
