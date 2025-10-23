TEMPLATE = app


QT += qml quick widgets
QT += quickwidgets
QT += serialport
QT += core
QT += network
QT += xml
QT += charts
QT += multimedia
QT += websockets

CONFIG += c++11

TARGET = SIMEYELOGv4.4

# RC_FILE += app.rc

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        RS232Serial/readthreadcpp.cpp \
        RS232Serial/rs232serialcpp.cpp \
        adascontrol.cpp \
        androidmulticast.cpp \
        drivingdatainput.cpp \
        eyedatainput.cpp \
        filemanagement.cpp \
        hrhdatainput.cpp \
        imocast.cpp \
        jercserialcommv10.cpp \
        ledcontrol.cpp \
        loggingdatatofile.cpp \
        main.cpp \
        obscontroller.cpp \
        server.cpp \
        swcontrol.cpp \
        taskdatainput.cpp \
        taskwebsocketserver.cpp \
        ttsotask.cpp \
        ttsotasklistmodel.cpp \
        ttsotaskmanager.cpp \
        udpmessagingoutput.cpp \
        udpsocket.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    RS232Serial/readthreadcpp.h \
    RS232Serial/rs232Serialcpp.h \
    RS232Serial/serialcontrolcpp_global.h \
    adascontrol.h \
    androidmulticast.h \
    drivingdatainput.h \
    eyedatainput.h \
    filemanagement.h \
    hrhdatainput.h \
    imocast.h \
    include_SE/SECANMsgTypes.h \
    include_SE/SEDataTypes.h \
    include_SE/SEErrors.h \
    include_SE/SEOutputData.h \
    include_SE/SEOutputDataIds.h \
    include_SE/SEPacketAPI.h \
    include_SE/SETrackerStates.h \
    include_SE/TimeService.h \
    include_SE/json.hpp \
    jercserialcommv10.h \
    ledcontrol.h \
    loggingdatatofile.h \
    obscontroller.h \
    server.h \
    swcontrol.h \
    taskdatainput.h \
    taskwebsocketserver.h \
    ttsotask.h \
    ttsotasklistmodel.h \
    ttsotaskmanager.h \
    udpmessagingoutput.h \
    udpsocket.h

DISTFILES += \
    include_SE/SEOutputDataIds.cs \
    include_SE/data_output.json \
    include_SE/data_types.json \
    include_SE/errors.json \
    include_SE/record_states.json \
    include_SE/tracker_states.json
