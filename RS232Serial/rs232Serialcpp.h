#ifndef RS232SERIAL_H
#define RS232SERIAL_H

#include <QObject>
#include <QMutex>
#include <windows.h>
#include "serialcontrolcpp_global.h"
#include "readThreadcpp.h"

/**********************************************
 * Class  : RS232Serial plugin
 * Company: JLR Research
 * Author : Jerciline Sundar
 * CDSID  : jsundar
 * Ver    : 0.1.0
 * Date   : 26/11/14
 *
 * Overview:
 * This is a qml plugin class. This class communicates with RS232 comport and
 * send the events to the qml. This calss works with any RS 232 serial controller
 * This calss only works in the Windows environment *
 * Version History:
 * ********************************************/

class RS232Serialcpp: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(RS232Serialcpp)

    Q_PROPERTY(int readMode READ readMode WRITE setReadMode)


    Q_PROPERTY(QString portname READ portname WRITE setportname)
    Q_PROPERTY(int baudrate READ baudrate WRITE setBaudrate)
    Q_PROPERTY(int parity READ parity WRITE setparity)
    Q_PROPERTY(int databits READ databits WRITE setdatabits)
    Q_PROPERTY(int stopbits READ stopbits WRITE setstopbits)
    Q_PROPERTY(int flowcontrol READ flowcontrol WRITE setflowcontrol)

    Q_PROPERTY(bool connection READ connection WRITE setconnection)

    //Mode Constants
    Q_PROPERTY(int ASCIIMode READ ASCIIMode WRITE setASCIIMode)
    Q_PROPERTY(int BYTEMODESIM READ BYTEMODESIM WRITE setBYTEMODESIM)
    Q_PROPERTY(int BYTEMODE READ BYTEMODE WRITE setBYTEMODE)


    Q_PROPERTY(int FlowHardware READ FlowHardware WRITE setFlowHardware)
    Q_PROPERTY(int FlowNONE READ FlowNONE WRITE setFlowNONE)
    Q_PROPERTY(int FlowOther READ FlowOther WRITE setFlowOther)


    //BaudRATE Constants
    Q_PROPERTY(int Baud110 READ Baud110 WRITE setBaud110)
    Q_PROPERTY(int Baud300 READ Baud300 WRITE setBaud300)
    Q_PROPERTY(int Baud600 READ Baud600 WRITE setBaud600)
    Q_PROPERTY(int Baud1200 READ Baud1200 WRITE setBaud1200)
    Q_PROPERTY(int Baud4800 READ Baud4800 WRITE setBaud4800)

    Q_PROPERTY(int Baud9600 READ Baud9600 WRITE setBaud9600)
    Q_PROPERTY(int Baud19200 READ Baud19200 WRITE setBaud19200)
    Q_PROPERTY(int Baud38400 READ Baud38400 WRITE setBaud38400)
    Q_PROPERTY(int Baud57600 READ Baud57600 WRITE setBaud57600)
    Q_PROPERTY(int Baud115200 READ Baud115200 WRITE setBaud115200)

    Q_PROPERTY(int Baud14400 READ Baud14400 WRITE setBaud14400)
    Q_PROPERTY(int Baud56000 READ Baud56000 WRITE setBaud56000)
    Q_PROPERTY(int Baud128000 READ Baud128000 WRITE setBaud128000)
    Q_PROPERTY(int Baud256000 READ Baud256000 WRITE setBaud256000)

    //DataBitsType
    Q_PROPERTY(int Data_5 READ Data_5 WRITE setData_5)
    Q_PROPERTY(int Data_6 READ Data_6 WRITE setData_6)
    Q_PROPERTY(int Data_7 READ Data_7 WRITE setData_7)
    Q_PROPERTY(int Data_8 READ Data_8 WRITE setData_8)

    //ParityType
    Q_PROPERTY(int Par_None READ Par_None WRITE SetPar_None)
    Q_PROPERTY(int Par_Odd READ Par_Odd WRITE SetPar_Odd)
    Q_PROPERTY(int Par_Even READ Par_Even WRITE SetPar_Even)
    Q_PROPERTY(int Par_Mark READ Par_Mark WRITE SetPar_Mark)
    Q_PROPERTY(int Par_Space READ Par_Space WRITE SetPar_Space)

    Q_PROPERTY(int Stop_1   READ Stop_1   WRITE setStop_1)
    Q_PROPERTY(int Stop_1_5 READ Stop_1_5 WRITE setStop_1_5)
    Q_PROPERTY(int Stop_2   READ Stop_2   WRITE setStop_2)





public:
    explicit RS232Serialcpp(QObject *parent = 0);

    int readMode() const;
    void setReadMode(const int &readM);

    int flowcontrol() const;
    void setflowcontrol(const int &fcontrol);

    /*!
     * \brief baudrate function retruns the set baud rate
     * \return
     */
    int baudrate() const;
    /*!
     * \brief setBaudrate function set the baudrate (use the baudrate constant to set the baud rate).
     * \param baudrate
     */
    void setBaudrate (const int &baudrate);

    /*!
     * \brief portname returns the set port name
     * \return
     */
    QString portname() const;
    /*!
     * \brief setportname sets the port name (COM1, COM2...)
     * \param portname
     */
    void setportname (const QString &portname);

    /*!
     * \brief parity function returns the parity of the communication
     * \return
     */
    int parity() const;
    /*!
     * \brief setparity sets the parity of the communication (use the parity  constant)
     * \param parity
     */
    void setparity (const int &parity);

    /*!
     * \brief databits function retunrs the databits of the communication
     * \return
     */
    int databits() const;
    /*!
     * \brief setdatabits function sets the databits of the communication port
     * \param databits
     */
    void setdatabits (const int &databits);

    /*!
     * \brief stopbits function returns the stopbits of the communication port
     * \return
     */
    int stopbits() const;
    /*!
     * \brief setstopbits  function sets the stopbits of the communication port
     * \param stopbits
     */
    void setstopbits (const int &stopbits);

    /*!
     * \brief connection returns the connuction status
     * \return  true - if it is connected to the communication port
     *          false - not connected to the communication port
     */
    bool connection() const;
    /*!
     * \brief setconnection Connect and disconnect the communication port based on the parameter setting
     * \param connect - true - connects to the communication port and start the read thread
     *                  false - stop the read thread and disconnect the communication
     */
    void setconnection (const bool &connect);

    /*!
     * \brief stopThread stops the read thread
     */
    void stopThread();

    //Mode constants
    int ASCIIMode() const {return m_ASCIIMode;}
    int BYTEMODESIM() const {return m_BYTEMODESIM;}
    int BYTEMODE() const {return m_BYTEMODE;}

    void setASCIIMode (const int &asscii_mode){m_ASCIIMode = asscii_mode;}
    void setBYTEMODESIM (const int &bytemodesim){m_BYTEMODESIM = bytemodesim;}
    void setBYTEMODE (const int &bytemode){m_BYTEMODE = bytemode;}

    Q_PROPERTY(int FlowHardware READ FlowHardware WRITE setFlowHardware)
    Q_PROPERTY(int FlowNONE READ FlowNONE WRITE setFlowNONE)
    Q_PROPERTY(int FlowOther READ FlowOther WRITE setFlowOther)


    int FlowHardware() const {return m_flowhardware;}
    int FlowNONE()const {return m_flownone;}
    int FlowOther() const {return m_flowother;}

    void setFlowHardware (const int &hardware) {m_flowhardware = hardware;}
    void setFlowNONE (const int &flownone) {m_flownone = flownone;}
    void setFlowOther (const int &flownother) {m_flowother = flownother;}




    //Baudrate constants property    
    int Baud110() const {return m_Baud110;}
    int Baud300() const {return m_Baud300;}
    int Baud600() const {return m_Baud600;}

    int Baud1200() const {return m_Baud1200;}
    int Baud4800() const {return m_Baud4800;}
    int Baud9600() const {return m_Baud9600;}

    int Baud19200() const {return m_Baud19200;}
    int Baud38400() const {return m_Baud38400;}
    int Baud57600() const {return m_Baud57600;}

    int Baud115200() const{return m_Baud115200;}
    int Baud14400() const {return m_Baud14400;}
    int Baud56000() const {return m_Baud56000;}
    int Baud128000() const {return m_Baud128000;}
    int Baud256000() const {return m_Baud256000;}

    void setBaud110(const int &Baud110) {m_Baud110 = Baud110;}
    void setBaud300(const int &Baud300) {m_Baud300 = Baud300;}
    void setBaud600(const int &Baud600) {m_Baud600 = Baud600;}

    void setBaud1200(const int &Baud1200) {m_Baud1200 = Baud1200;}
    void setBaud4800(const int &Baud4800) {m_Baud4800 = Baud4800;}
    void setBaud9600(const int &Baud9600) {m_Baud9600 = Baud9600;}

    void setBaud19200(const int &Baud19200) {m_Baud19200 = Baud19200;}
    void setBaud38400(const int &Baud38400) {m_Baud38400 = Baud38400;}
    void setBaud57600(const int &Baud57600) {m_Baud57600 = Baud57600;}

    void setBaud115200(const int &Baud115200) {m_Baud115200 = Baud115200;}
    void setBaud14400(const int &Baud14400)   {m_Baud14400 = Baud14400;}
    void setBaud56000(const int &Baud56000)   {m_Baud56000 = Baud56000;}
    void setBaud128000(const int &Baud128000) {m_Baud128000 = Baud128000;}
    void setBaud256000(const int &Baud256000) {m_Baud256000 = Baud256000;}

    //Data bits constant property
    int Data_5() const {return m_Data_5;}
    int Data_6() const {return m_Data_6;}
    int Data_7() const {return m_Data_7;}
    int Data_8() const {return m_Data_8;}

    void setData_5(const int &Data_5) {m_Data_5 = Data_5;}
    void setData_6(const int &Data_6) {m_Data_6 = Data_6;}
    void setData_7(const int &Data_7) {m_Data_7 = Data_7;}
    void setData_8(const int &Data_8) {m_Data_8 = Data_8;}

    //Parity constant property
    int Par_None() const {return m_Par_None;}
    int Par_Odd () const {return m_Par_Odd;}
    int Par_Even() const {return m_Par_Even;}
    int Par_Mark() const {return m_Par_Mark;}
    int Par_Space() const{return m_Par_Space;}

    void SetPar_None(const int &Par_None){m_Par_None = Par_None;}
    void SetPar_Odd(const int &Par_Odd){m_Par_Odd  = Par_Odd;}
    void SetPar_Even(const int &Par_Even){m_Par_Even = Par_Even;}
    void SetPar_Mark(const int &Par_Mark){m_Par_Mark = Par_Mark;}
    void SetPar_Space(const int&Par_Space){m_Par_Space= Par_Space;}

    //Stop bits constant property
    int Stop_1  () const {return m_Stop_1  ;}
    int Stop_1_5() const {return m_Stop_1_5;}
    int Stop_2  () const {return m_Stop_2  ;}

    void setStop_1  (const int &Stop_1) {m_Stop_1   = Stop_1;}
    void setStop_1_5(const int &Stop_1_5) {m_Stop_1_5 = Stop_1_5;}
    void setStop_2  (const int &Stop_2) {m_Stop_2   = Stop_2;}


    ~RS232Serialcpp();

signals:

    /*!
     * \brief receivedData signal emits when ever the data received from serial port
     * \param data - String data
     */
    void receivedData(QByteArray data);
    /*!
     * \brief erroronCom signal emits whenever there is an error
     * \param error - error as string
     */
    void erroronCom(QString error);


private:
    bool Openport();
    void Closeport();
    bool readData();
    bool writeData(SHORT snLength, BYTE* pbyData);
    bool config();



private slots:
    void receivedDatafromThread(QByteArray data);


private:
    //QString m_Baudrate;
    int m_bBaudrate;
    QString m_portname;
    int m_parity;
    int m_databits;
    int m_stopbits;
    int m_flowcontrol;

    bool m_connect;

    QMutex myuMutex;
    HANDLE m_hComPort;
    COMMCONFIG Win_CommConfig;
    bool threadcont;
    readThreadcpp *m_readThread;


    int m_ASCIIMode;
    int m_BYTEMODESIM;
    int m_BYTEMODE;

    int m_flowhardware;
    int m_flownone;
    int m_flowother;


    int m_readMode;

    int m_Baud110;
    int m_Baud300;
    int m_Baud600;

    int m_Baud1200;
    int m_Baud4800;
    int m_Baud9600;

    int m_Baud19200;
    int m_Baud38400;
    int m_Baud57600;

    int m_Baud115200;
    int m_Baud14400;
    int m_Baud56000;
    int m_Baud128000;
    int m_Baud256000;

    int m_Data_5;
    int m_Data_6;
    int m_Data_7;
    int m_Data_8;

    int m_Par_None;
    int m_Par_Odd;
    int m_Par_Even;
    int m_Par_Mark;
    int m_Par_Space;

    int m_Stop_1;
    int m_Stop_1_5;
    int m_Stop_2;


};

#endif // RS232SERIAL_H

