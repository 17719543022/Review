#include "homepage.h"
#include "ui_homepage.h"
#include "UI_Classes/flightenquires.h"
#include "UI_Classes/portraitcollection.h"
#include "UI_Classes/realtimeboarding.h"
#include "UI_Classes/workflowrecording.h"
#include <QMessageBox>
#include <QTimeLine>

#include <ServerInterface/mqmsg_parse.h>

HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);
    m_RealtimeBoarding = new RealtimeBoarding(ui->centralWidget);
//    m_RealtimeBoarding->move(0,100);
    m_FlightEnquires =  new FlightEnquires(ui->centralWidget);
//    m_FlightEnquires->move(0,100);
    m_WorkflowRecording = new WorkflowRecording(ui->centralWidget);
//    m_WorkflowRecording->move(0,100);
    m_PortraitCollection = new PortraitCollection(ui->centralWidget);
//    m_PortraitCollection->move(0,100);
    ui->workLayout->addWidget(m_RealtimeBoarding);
    ui->workLayout->addWidget(m_FlightEnquires);
    ui->workLayout->addWidget(m_WorkflowRecording);
    ui->workLayout->addWidget(m_PortraitCollection);

    ui->verticalLayoutWidget->move(-200,300);
    ui->verticalLayoutWidget->raise();

    timeLine = new QTimeLine(100, this);
    timeLine->setFrameRange(0, 200);
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(frameChange(int)));
    connect(this, SIGNAL(timeLineStart()), timeLine, SLOT(start()));//use connection to be compatible with multiThread;

    initTranslationWidget();
    initMqServer();
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::initTranslationWidget()
{
    m_RealtimeBoarding = new RealtimeBoarding(ui->centralWidget);
    m_FlightEnquires = new FlightEnquires(ui->centralWidget);
    m_WorkflowRecording = new WorkflowRecording(ui->centralWidget);
    m_PortraitCollection = new PortraitCollection(ui->centralWidget);
}

void HomePage::initMqServer()
{
    m_pMQmsg = new MsgParse(this);

    connect(m_pMQmsg, &MsgParse::recived_mq_msg, this, &HomePage::on_recivedMQmsg);
}

void HomePage::on_recivedMQmsg(int type)
{
    switch (type) {
    case 302:
        m_FlightEnquires->fillOrgDepWithMQ(m_pMQmsg->m_new_repository);
        break;

    default:
        break;
    }
}

void HomePage::on_Button_RealtimeBoarding_clicked()
{
    m_RealtimeBoarding->show();
    m_WorkflowRecording->hide();
    m_FlightEnquires->hide();
    m_PortraitCollection->hide();
}

void HomePage::on_Button_WorkflowRecording_clicked()
{
    m_WorkflowRecording->show();
    m_RealtimeBoarding->hide();
    m_FlightEnquires->hide();
    m_PortraitCollection->hide();
}

void HomePage::on_Button_FlightEnquires_clicked()
{
    m_FlightEnquires->show();
    m_FlightEnquires->statistics("AB1251");
    m_RealtimeBoarding->hide();
    m_WorkflowRecording->hide();
    m_PortraitCollection->hide();

}

void HomePage::on_Button_PortraitCollection_clicked()
{
    m_PortraitCollection->show();
    m_RealtimeBoarding->hide();
    m_WorkflowRecording->hide();
    m_FlightEnquires->hide();

}

void HomePage::on_Button_menu_clicked()
{
    emit timeLineStart();
//    m_RealtimeBoarding->hide();
//    m_WorkflowRecording->hide();
//    m_FlightEnquires->hide();
//    m_PortraitCollection->hide();
}

void HomePage::frameChange(int f)
{
    if(!ui->Button_menu->isChecked()){
        ui->verticalLayoutWidget->move(0-f,300);
    }
    else{
        ui->verticalLayoutWidget->move(-200+f,300);
    }
}


