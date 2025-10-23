#ifndef EYEDATAINPUT_H
#define EYEDATAINPUT_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "windows.h"
#include "udpsocket.h"
#include <QVariant>

#include <queue>
#include <vector>

// Forward declarations
class loggingDataToFile;
class filemanagement;
class TTSOTask;   // ✅ forward declare, no need for full include here

class eyeDataInput: public QObject
{
    Q_OBJECT

public:
    explicit eyeDataInput(QObject *parent = 0);

    loggingDataToFile *logData;   // Pointer to logging data object
    filemanagement *fileData;     // Pointer to file manager object

    QTimer *timer = new QTimer(this);        // Time for polling the Multicast port
    QTimer *timerBuffer = new QTimer(this);  // Timer for writing attention buffer data file

    // --- Existing members ---
    ULONGLONG taskStartEpochTime = 0;  // member variable

    // for buffering samples
    struct GlanceSample {
        double elapsedTime;  // seconds from task start
        QString label;
        ULONGLONG epochTime;
    };
    QVector<GlanceSample> glanceSampleBuffer;  // Buffer for interpolation

    ULONGLONG makeTime();
    ULONGLONG posixTimePreviousGlance = 0;
    ULONGLONG ShortGlanceThreshold = 175;
    ULONGLONG posixTimePrevious = 0;
    ULONGLONG GlanceTimer = 0;
    qint64 pID = 0;
    qint64 rNum = 0;
    UDPSocket *m_udpsocket_Eye;

    QString PreviousGlanceLocation = "0";
    QString PreviousFilteredGlanceLocation = "0";
    QString PreciseLocation = "0";
    QString EstimatedLocation = "0";

    QString eyeHost = "192.168.101.22";

    QString GlanceLocation = "X";
    QString PreciseGlanceLocation = "";
    QString CurrentGlanceLocation = "";
    QString filteredGlanceLocation = "";

    QString Horiz_Eye_Poz = "0";
    QString Blink = "0";
    QString eyeOnOff = "true";
    QString DriverStateCondition = "";
    QString aheadFilenameTask;
    QString aheadFilenameTaskChopped;
   // float qualityGaze = 0;
    float lefteye = 0;                                                          //Eye Closure Value
    float quality = 0;
    ULONG resolutionX = 0;
    ULONG resolutionY = 0;
    ULONG resolutionZ = 0;

    float BufferArray[10] = {0,0,0,0,0,0,0,0,0,0};

    float Mean_AttenD=0.0;
    float average=0.0;
    std::vector<double> storeVal;

    float eyetrackerBrand = 0;
    float blinkThreshold = 0.92;
    float eyequality = 0;
    float EyeQualityCheck(ULONGLONG time);
    float Q_B[9];
    float currentGlanceTime = 0;
    float eye_time_buffer = 0;
    float eye_time_buffer_ahead = 0;
    float toTheRoadTime = 0;
    float fromTheRoadTime = 0;

    double KA_Buffer_Val_float = 0;
    double AHEADV1_Buffer_Val_float = 0;
    double CogV0_Buffer_Val_float = 0;

    int signalRefreshTimer = 100;
    float attentionBufferRefresh = 33.33333333;
    int GlanceCounter = 0;
    int frameNumber = 0;
    int frameNumberByTask = 0;

    int counterZ = 0;
    const int eyePort = 8080;

    void AttentionBuffer(QString GlanceLoc2, ULONGLONG time);

    double getMeanAttenD() const { return Mean_AttenD; }
    double getMeanLiveAttenD() const {
            return (m_liveAttenDCount > 0) ? (m_liveAttenDSum / m_liveAttenDCount) : 0.0;
        }
    void resetMeanLiveAttenD() {
            m_liveAttenDSum = 0.0;
            m_liveAttenDCount = 0;
        }
signals:
    void signalEyeRefresh(QVariant status);
    void signalEyeLogging(QVariant status);
    void setTerminalWindowGlance(QVariant param1, QVariant param2,
                                 QVariant param3, QVariant param4,
                                 QVariant param5, QVariant param6,
                                 QVariant param7, QVariant param8, QVariant param9);

public slots:
    void updateEyeTrackingDataOnOff(QString draOnOff);
    void recievedData(QByteArray eyeData);
    void updatePortSettingsEye(QString ipaddress,QString port);
    void dataTriggerCommand();
    void initObjects(loggingDataToFile *loggingDataToFile_ptr,
                     filemanagement *filemanagement_ptr);
    void writeAttBuffFile();
    void startWriteAttBuffFile(QString fileNum);
    void stopWriteAttBuffFile();

    // ✅ New slot to receive active task info from TTSOTaskListModel
    void onActiveTaskChanged(TTSOTask *task);

    void startDemoLogging();
    void stopDemoLogging();
    void startExpLogging();
    void stopExpLogging();

private:
    // ✅ Keep track of currently active task
    TTSOTask *m_currentTask = nullptr;
    double m_liveAttenDSum = 0.0;
    int    m_liveAttenDCount = 0;
    void writeInterpolatedAHEAD();
    bool m_customMode = false;
    QString m_customLabel; // "demo" or "exp"
    void startCustomLogging(const QString &label);
        void stopCustomLogging();
};

#endif // EYEDATAINPUT_H
