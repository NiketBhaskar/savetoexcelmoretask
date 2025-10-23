#include "imocast.h"
#include <QTimer>
#include <QDebug>
#include <QQuickWindow>

#include "taskdatainput.h"
//#include "drivingDatainput.h"


imocast::imocast(QObject* parent): QObject(parent)
{
    //QTimer* timer =new QTimer(this);
    //connect(timer, &QTimer::timeout, this, &androidmulticast::getMulticastData);
    //timer->start(signalRefreshTimer);
    taskDataInput *taskDataInput_ptr;


}
void imocast::receiveMulticastData(QVariant time,QVariant lanepos,QVariant steeringangle,QVariant collisions,
                                        QVariant speed,QVariant throttle,QVariant braking,QVariant distancetravel,
                                        QVariant nearestcar,QVariant leftBlindSpot,QVariant rightBlindSpot,
                                        QVariant indicatorStatus, QVariant SD_LP, QVariant SD_S,QVariant unixTime)
{
    //sending only QML data since we are doing qml prototypes on Android
        QString message ;
        QString time_Msg = time.toString();
        QString lanepos_Msg = lanepos.toString();
        QString steeringAngle_Msg = steeringangle.toString();
        QString crashes_Msg = collisions.toString();
        QString speed_Msg = speed.toString();
        QString throttle_Msg = throttle.toString();
        QString braking_Msg = braking.toString();
        QString distancetravel_Msg = distancetravel.toString();
        QString minrange_Msg = nearestcar.toString();
        QString leftBlindSpot_Msg = leftBlindSpot.toString();
        QString rightBlindSpot_Msg = rightBlindSpot.toString();

        QString indicatorStatus_Msg = indicatorStatus.toString();
        QString sdlp_Msg = SD_LP.toString();
        QString sds_Msg = SD_S.toString();

        float mph_speed = speed_Msg.toFloat() *  0.621371;
        QString mph_speed_str = QString::number(mph_speed);


//        qDebug() << "simTime: " << timeMsg;
//        qDebug() << "LanePos: " << laneposMsg;
//        qDebug() << "strWhl: " << steeringangleMsg;
//        qDebug() << "crashes: " << collisionsMsg;
//        qDebug() << "speed: " << mph_speed_str;
//        qDebug() << "throttle: " << throttleMsg;
//        qDebug() << "brake: " << braking;
//        qDebug() << "distance: " << brakingMsg;
//        qDebug() << "nearestcar: " << nearestcar;
//        qDebug() << "lbs: " << leftBlindSpotMsg;
//        qDebug() << "rbs: " << rightBlindSpotMsg;
//        qDebug() << "indicatorStatus: " << indicatorStatusMsg;
//        qDebug() << "SD_LP: " << SD_LP;
//        qDebug() << "SD_S: " << SD_S;
//        qDebug() << "unixTime: " << unixTime;
//        message.append(time_Msg + "," + lanepos_Msg + "," + steeringAngle_Msg + ",");        //1. TIME //2. LANE POSITION //3. STEERING ANGLE
//        message.append(crashes_Msg + "," + mph_speed_str + "," + throttle_Msg + ",");      //4. COLLISIONS //5. SPEED //6. THROTTLE
//        message.append(braking_Msg + "," + distancetravel_Msg + "," +minrange_Msg + ",");     //7. BRAKING //8. DISTANCE TRAVELLED //9. MINRANGE
//        message.append(leftBlindSpot_Msg + "," + rightBlindSpot_Msg + ","+indicatorStatus_Msg ); //10. LBS SPOT  //11. RBS SPOT //12. INDICATOR STATUS
//        message.append(sdlp_Msg + "," + sds_Msg + ",");  //13. SDLP //14.SDS //
        //message.append(taskDataInput->lastEvent);

        QString EventSourceID = "simIntegration";
        QString SampleID_A = "simData";
        QString DataChannel11 =steeringAngle_Msg;
        QString DataChannel12 =mph_speed_str;

        QString SampleID_B = "simData";
        QString DataChannel13 =throttle_Msg;
        QString DataChannel14 =braking_Msg;

        //message.append("E;1;"+EventSourceID+";1;0.0;;;"+SampleID_A+";"+DataChannel11+";"+ DataChannel12+"\r\n");
        message.append("E;1;"+EventSourceID+";1;0.0;;;"+SampleID_B+";"+DataChannel11+";"+ DataChannel12+";"+ DataChannel13+";"+ DataChannel14+"\r\n");

        castData(message,imoAddress,imoPort);
       // qDebug()<<message;

}

void imocast::castData(QString message, QHostAddress address, int port)
{
    QByteArray msg = message.toLatin1();
    udpSocket.writeDatagram(msg,address,port);  //Send data on Multicast Port

    //qDebug()<<"sent"<<message;
}
