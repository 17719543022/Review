#include "portraitcollection.h"
#include "ui_portraitcollection.h"

PortraitCollection::PortraitCollection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PortraitCollection)
{
    ui->setupUi(this);
    this->hide();
}

PortraitCollection::~PortraitCollection()
{
    delete ui;
}

