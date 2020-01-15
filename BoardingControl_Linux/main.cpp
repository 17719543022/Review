#include "homepage.h"
#include "LogTrace/loghandler.h"
#include "settings.h"
#include <QApplication>
#include <QDebug>
#include "UI_Classes/workflowrecording.h"
#include "UI_Classes/flightenquires.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try {
        //initialize local config file;
        LocalSettings::instance();
        //  安装log消息处理函数
        Singleton<LogHandler>::getInstance().installMessageHandler();
    } catch (std::exception &ex) {
        qCritical()<<ex.what();
        qApp->exit();
    }

//   HomePage w;
//   w.show();

//    WorkflowRecording w;
//    w.show();

    FlightEnquires f;
    f.show();
//    f.statistics("AB1250");

    return a.exec();
}
