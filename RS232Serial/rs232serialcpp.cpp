#include "RS232Serialcpp.h"
#include <QDebug>
#include <windows.h>
#include <QByteArray>
#include <QThread>
#include <QDebug>



RS232Serialcpp::RS232Serialcpp(QObject *parent)
    :QObject (parent)
{


    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

    // setFlag(ItemHasContents, true);

    setASCIIMode(0);
    setBYTEMODESIM(1);
    setBYTEMODE(2);

    setFlowHardware(1);
    setFlowNONE(0);
    setFlowOther(2);

    setReadMode(0);
    setBaud110(110);
    setBaud300(300);
    setBaud600(600);
    setBaud1200(1200);

    setBaud4800(4800);
    setBaud9600(9600);
    setBaud14400(14400);
    setBaud19200(19200);

    setBaud38400(38400);
    setBaud56000(56000);
    setBaud57600(57600);
    setBaud115200(115200);

    setBaud128000(128000);
    setBaud256000(256000);

    setData_5(5);
    setData_6(6);
    setData_7(7);
    setData_8(8);

    SetPar_None(0);
    SetPar_Odd(1);
    SetPar_Even(2);
    SetPar_Mark(3);
    SetPar_Space(4);

    setStop_1(0);
    setStop_1_5(1);
    setStop_2(2);


    //Defaultvalue
    setBaudrate(Baud38400());
    setparity(Par_None());
    setdatabits(Data_8());
    setstopbits(Stop_1());

    m_readThread = new readThreadcpp();
    m_readThread->setMode(0);
    connect(m_readThread,SIGNAL(receivedData(QByteArray)),this,SLOT(receivedDatafromThread(QByteArray)));


}

RS232Serialcpp::~RS232Serialcpp()
{
    delete m_readThread;
    if (m_readThread != NULL)
        m_readThread = NULL;

}

int RS232Serialcpp::readMode() const
{
    return m_readMode;
}

void RS232Serialcpp::setReadMode(const int &readM)
{

    m_readMode = readM;

}

int RS232Serialcpp::flowcontrol() const
{
    return m_flowcontrol;
}

void RS232Serialcpp::setflowcontrol(const int &flowcontrol)
{
    m_flowcontrol = flowcontrol;
}

int RS232Serialcpp::baudrate() const
{

    return m_bBaudrate;
}

void RS232Serialcpp::setBaudrate(const int &baudrate)
{     
    m_bBaudrate = baudrate;
}

QString RS232Serialcpp::portname() const
{
    return m_portname;
}

void RS232Serialcpp::setportname(const QString &portname)
{
    m_portname = portname;   
}


int RS232Serialcpp::parity() const
{
    return m_parity;
}

void RS232Serialcpp::setparity(const int &parity)
{
    m_parity = parity;
}


int RS232Serialcpp::databits() const
{
    return m_databits;
}

void RS232Serialcpp::setdatabits(const int &databits)
{
    m_databits = databits;
}

int RS232Serialcpp::stopbits() const
{
    return m_stopbits;
}

void RS232Serialcpp::setstopbits(const int &stopbits)
{
    m_stopbits = stopbits;
}

bool RS232Serialcpp::connection() const
{
    return m_connect;
}

void RS232Serialcpp::setconnection(const bool &connect)
{
    bool success;     
    if (connect == true)
    {
        success = this->Openport();
        if (success == true)
        {
            m_connect = true;
            qDebug()<<"Connection success";
            emit erroronCom("Connection success");
            PurgeComm(m_hComPort, 0x0008|0x0002|0x0001|0x0004);
            m_readThread->setHandle(m_hComPort);
            m_readThread->setMode(m_readMode);
            m_readThread->startThread();


        }
        else
        {
            m_connect = false;
            qDebug()<<"Connection failed";
            emit erroronCom("Connection failed");
        }
    }
    else
    {
        m_connect = false;
        this->Closeport();
    }


}

void RS232Serialcpp::receivedDatafromThread(QByteArray data)
{   
    emit receivedData(data);
}



bool RS232Serialcpp::Openport()
{
    OVERLAPPED myO;
     std::wstring compath (L"\\\\.\\");
     compath += portname().toStdWString();
    LPCTSTR portnum = compath.c_str();

    m_hComPort = CreateFile(portnum, GENERIC_READ|GENERIC_WRITE,
                             FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);


    if ( INVALID_HANDLE_VALUE != m_hComPort )
    {

        if (config())
        {
            ZeroMemory(&myO, sizeof (OVERLAPPED));
            myO.hEvent = CreateEvent (NULL,true ,false ,NULL);
            myO.Internal =0;
            myO.InternalHigh =0;
            if (!SetCommMask(m_hComPort,EV_RXCHAR))
                return false;
            FlushFileBuffers(m_hComPort);
        }
        else
            return false;
    }
    else
    {
        return false;
    }

    return true;

}

bool RS232Serialcpp::config()
{
    BOOL bConfigurePortSuccess = FALSE;
    if( SetupComm( m_hComPort, 1024, 1024 ) )
    {
        COMMTIMEOUTS sCommTimeout = {0};
        if( GetCommTimeouts( m_hComPort, &sCommTimeout) ) /* Configuring Read & Write Time Outs */
        {
            DWORD dwTimeOutInMilliSec = 10;
            sCommTimeout.ReadIntervalTimeout = dwTimeOutInMilliSec;
            sCommTimeout.ReadTotalTimeoutConstant = 10;
            sCommTimeout.ReadTotalTimeoutMultiplier = dwTimeOutInMilliSec;
            sCommTimeout.WriteTotalTimeoutConstant = dwTimeOutInMilliSec;
            sCommTimeout.WriteTotalTimeoutMultiplier = dwTimeOutInMilliSec;

            if( SetCommTimeouts( m_hComPort, &sCommTimeout ) )
            {
                DCB sDCBConfig = {0};
                if( GetCommState( m_hComPort, &sDCBConfig ) ) /* Configuring Serial Port Settings */
                {
                    sDCBConfig.BaudRate = baudrate();//115200;
                    sDCBConfig.ByteSize = databits();
                    sDCBConfig.Parity = parity();
                    sDCBConfig.StopBits = stopbits();
                    sDCBConfig.fBinary = TRUE;
                    sDCBConfig.fParity = TRUE;


                    if( FlowHardware() == m_flowcontrol)
                    {
                        //Handshake Hardware

                        sDCBConfig.fOutX = false;
                        sDCBConfig.fInX = false;
                        sDCBConfig.fOutxCtsFlow = true;
                        sDCBConfig.fOutxDsrFlow = true;
                        sDCBConfig.fDsrSensitivity = true;
                        sDCBConfig.fRtsControl = RTS_CONTROL_HANDSHAKE;
                        sDCBConfig.fDtrControl = DTR_CONTROL_HANDSHAKE;


                    }
                    else if( FlowNONE() == m_flowcontrol)
                    {
                        //Handshake NONE
                       /* sDCBConfig.fOutX = false;
                        sDCBConfig.fInX = false;
                        sDCBConfig.fOutxCtsFlow = false;
                        sDCBConfig.fOutxDsrFlow = false;
                        sDCBConfig.fDsrSensitivity = false;
                        sDCBConfig.fRtsControl = RTS_CONTROL_DISABLE;
                        sDCBConfig.fDtrControl = DTR_CONTROL_DISABLE;*/

                       // qDebug()<<"Set No flow";
                    }
                    else
                    {
                        //Handshake Software

                        sDCBConfig.fOutX = true;
                        sDCBConfig.fInX = true;
                        sDCBConfig.fOutxCtsFlow = false;
                        sDCBConfig.fOutxDsrFlow = false;
                        sDCBConfig.fDsrSensitivity = false;
                        sDCBConfig.fRtsControl = RTS_CONTROL_DISABLE;
                        sDCBConfig.fDtrControl = DTR_CONTROL_DISABLE;


                    }


                    if( SetCommState( m_hComPort, &sDCBConfig ) )
                    {
                        bConfigurePortSuccess = TRUE;
                    }
                }
            }
        }
    }
    return bConfigurePortSuccess;

}

void RS232Serialcpp::Closeport()
{
    m_readThread->stopThread();


    if( m_hComPort != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hComPort );
        m_hComPort = INVALID_HANDLE_VALUE;
    }
}

bool RS232Serialcpp::readData()
{
    DWORD dwEvtMask;
    OVERLAPPED ov;
    BYTE data[128] = {0};											// data receiver
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

            char Data_buff;

            //QByteArray ba = message.toLatin1();
            const char *c_str2 = nullptr;

            for (int i =0; i<ARRAYSIZE(data); i++)
            {
                c_str2 += data[i];
            }

            //Emit signal
            //emit receivedData(temp);
            FlushFileBuffers(m_hComPort);
        }
        return true;
    }
}
bool RS232Serialcpp::writeData(SHORT snLength, BYTE* pbyData)
{
    myuMutex.lock();
    INT retVal=0;
    INT lastErr;
    DWORD Win_BytesWritten;
    OVERLAPPED ov;
    ov.hEvent = CreateEvent (0,true,0,0);
    FlushFileBuffers(m_hComPort);

    if (!WriteFile(m_hComPort, (void*)pbyData, (DWORD)snLength, &Win_BytesWritten, &ov))
    {
        lastErr = -1;
        retVal = lastErr;
        qDebug()<<"Error on writing :"<< retVal;
        WriteFile(m_hComPort, (void*)pbyData, (DWORD)snLength, &Win_BytesWritten, NULL);
    }
    else
    {
        retVal=((int)Win_BytesWritten);
    }

    FlushFileBuffers( m_hComPort );
    myuMutex.unlock();
    return retVal;

}


