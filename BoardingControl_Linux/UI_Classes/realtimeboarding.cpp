#include "realtimeboarding.h"
#include "ui_realtimeboarding.h"

RealtimeBoarding::RealtimeBoarding(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RealtimeBoarding)
{
    ui->setupUi(this);
    this->hide();
}

RealtimeBoarding::~RealtimeBoarding()
{
    delete ui;
}

