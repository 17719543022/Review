#-------------------------------------------------
#
# Project created by QtCreator 2019-12-19T11:29:47
#
#-------------------------------------------------

QT       += core gui network
CONFIG+=C++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BoardingControl_Linux
TEMPLATE = app


SOURCES += main.cpp\
    UI_Classes/flightenquires.cpp \
    UI_Classes/portraitcollection.cpp \
    UI_Classes/realtimeboarding.cpp \
    UI_Classes/workflowrecording.cpp \
    homepage.cpp \
    LogTrace/loghandler.cpp \
    settings.cpp \
    ServerInterface/amqpImp.cpp \
    rabbitmq_c/amqp_time.cpp \
    ServerInterface/httpAPI.cpp \
    ServerInterface/mqmsg_parsing.cpp

HEADERS  += \
    UI_Classes/flightenquires.h \
    UI_Classes/portraitcollection.h \
    UI_Classes/realtimeboarding.h \
    UI_Classes/workflowrecording.h \
    homepage.h \
    LogTrace/loghandler.h \
    LogTrace/singleton.h \
    settings.h \
    ServerInterface/amqpImp.h\
    rabbitmq_c/amqp_time.h \
    ServerInterface/api_param_def.h \
    ServerInterface/httpAPI.h \
    ServerInterface/mqmsg_parsing.h

FORMS    += \
    UI_Classes/workflowrecording.ui \
    UI_Classes/realtimeboarding.ui \
    UI_Classes/portraitcollection.ui \
    UI_Classes/flightenquires.ui \
    homepage.ui

#unix:!macx: LIBS +=/usr/local/lib/libssl.so.1.0.0
unix:!macx: LIBS += -L$$PWD/../SDK/bin -Wl,--as-needed -lHTTP -ljsoncpp \
                                                     -Wl,--no-as-needed -lrabbitmq -Wl,--as-needed

INCLUDEPATH += $$PWD/../SDK/include
DEPENDPATH += $$PWD/../SDK/include

QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN/../SDK/bin\'

RESOURCES += \
    boardingcontrol_linux.qrc
