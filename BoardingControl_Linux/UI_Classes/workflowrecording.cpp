#include "workflowrecording.h"
#include "ui_workflowrecording.h"
#include "ServerInterface/httpAPI.h"

WorkflowRecording::WorkflowRecording(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkflowRecording),
    isBabyMode(false)
{
    ui->setupUi(this);
    this->hide();
}

WorkflowRecording::~WorkflowRecording()
{
    delete ui;
}


void WorkflowRecording::on_flowBabyPushButton_clicked()
{
    isBabyMode = !isBabyMode;

    if (isBabyMode) {
        ui->flowBabyPushButton->setStyleSheet("image: url(:/4全流程记录/Images/4全流程记录/勾选.png);");
    } else {
        ui->flowBabyPushButton->setStyleSheet("image: url(:/4全流程记录/Images/4全流程记录/未勾选.png);");
    }
}

void WorkflowRecording::on_flowQueryPushButton_clicked()
{
    FlowReviewRequest request;
    request.input = ui->flowQueryLineEdit->text();

    FlowReviewResponse response = HttpAPI::instance()->get(request);
}
