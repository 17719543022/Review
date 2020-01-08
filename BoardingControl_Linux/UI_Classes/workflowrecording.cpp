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

    ui->flowTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->flowTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->flowTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->flowTableWidget->verticalHeader()->setVisible(false);
    ui->flowTableWidget->horizontalHeader()->setVisible(false);
    ui->flowTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->flowTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->flowTableWidget->setStyleSheet("image: 0; border: 0; background: transparent;");
    ui->flowTableWidget->setShowGrid(false);
    ui->flowTableWidget->setColumnCount(5);
    ui->flowTableWidget->setColumnWidth(0, 16);
    ui->flowTableWidget->setColumnWidth(1, 150);
    ui->flowTableWidget->setColumnWidth(2, 150);
    ui->flowTableWidget->setColumnWidth(3, 130);
    ui->flowTableWidget->setColumnWidth(4, 270);
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
