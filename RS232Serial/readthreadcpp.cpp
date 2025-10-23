#include "readthreadcpp.h"
#include <QDebug>
#include <QByteArray>
#include<math.h>


readThreadcpp::readThreadcpp(QObject *parent):
  QThread(parent)
{
    mutex.lock();
    m_babort = false;
    mutex.unlock();
}

readThreadcpp::~readThreadcpp()
{
    mutex.lock();
    m_babort = true;
    mutex.unlock();
    wait();
}

void readThreadcpp::startThread()
{
    mutex.lock();
    m_babort = false;
    mutex.unlock();
    start();
}

void readThreadcpp::run()
{
    forever
    {

        DWORD dwEvtMask;
        OVERLAPPED ov;
        //BYTE data[128] = {0};
        char data[128] = {0};

        QByteArray tempdata;// data receiver

        //for(int i = 0; i < 128; i++)
        //{
            //data[i] = ' ';
       // }

        ov.hEvent = CreateEvent (0,true,0,0);
        if (WaitCommEvent(m_hComPort, &dwEvtMask, NULL))

       {
            if (dwEvtMask & EV_RXCHAR)
            {
                Sleep(10);


                DWORD transferred;
                //finish = true;
                COMSTAT    ComStat ;
                DWORD      dwErrorFlags;
                DWORD      dwLength;
                // check number of bytes in queue
                ClearCommError(m_hComPort, &dwErrorFlags, &ComStat ) ;
                dwLength = ComStat.cbInQue;
                ReadFile (m_hComPort, &data, dwLength, &transferred, NULL);//&myO);

                QString temp;
                if(data[0] != NULL)
                {
                     m_datalength = (int)dwLength;

                     tempdata = QByteArray::fromRawData(data,m_datalength);
                     byteProcess(tempdata, m_datalength);

                }
                PurgeComm(m_hComPort, 0x0008|0x0002|0x0001|0x0004);


            }
        }


        if(m_babort)
            return;
        msleep(50);
    }
}

void readThreadcpp::byteProcess(QByteArray data, int nLength)
{
    emit receivedData(data);
}

void readThreadcpp::stopThread()
{
    if (m_babort == false)
    {
        mutex.lock();
        m_babort = true;
        mutex.unlock();
    }
}

double readThreadcpp::rpm(double speed, double gear)
{
    double calculated_rpm = 0;
    //calculte rmp

   // return (((INCHESPERMILE / (Math.PI * tireDiam)) * (ratios[gear] * finalDrive)) / 60) * speed;
    //int INCHESPERMILE = 63360;


   // qDebug()<<speed;
   // qDebug()<<gear;

    //calculated_rpm = ((((63360/(M_PI * m_tireDiam)) *(m_ratio[(int)gear-1]* m_diff_ratio))/60)*speed)*100;
   // qDebug()<< "Calculated rpm:";
   // qDebug()<<calculated_rpm;


    if(calculated_rpm <=1000)
        calculated_rpm = 1000;

    if(calculated_rpm >= m_max_rpm)
        calculated_rpm = m_max_rpm;


    return calculated_rpm;
}

double readThreadcpp::bytetoFloat(BYTE b0, BYTE b1, BYTE b2, BYTE b3)
{
    if((b0 == 0 || b0 == 0x80) && (b1 == 0) && (b2 == 0) && (b3 == 0))
    {
        return 0;
    }
    else
    {
        return qPow(-1, ((b0 >> 7)&1)) * ((((b1 | 0x80) <<16) + (b2 << 8) + b3) * qPow(2, -23))
                * qPow(2, ((((b0 & 0x7F)<<1) |((b1>>7) &1)) - 127));

        //        int sign = (b0 >> 7)&1;
        //        int expoonebit = (b1>>7) &1;
        //        int expo7bit = b0 & 0x7F ;
        //        int expo = (expo7bit<<1) |expoonebit;
        //        int signt = b1  & 0x7F;
        //        int significant = ((b1 | 0x80) <<16) + (b2 << 8) + b3;
        //        float signi = significant * qPow(2, -23);
        //        int powerval = expo - 127;
        //        return qPow(-1, sign) * (signi) * qPow(2, powerval);
    }


}
