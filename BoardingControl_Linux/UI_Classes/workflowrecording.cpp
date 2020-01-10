#include "workflowrecording.h"
#include "ui_workflowrecording.h"
#include "ServerInterface/httpAPI.h"
#include <QGridLayout>

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
    ui->flowTableWidget->setColumnCount(1);
    ui->flowTableWidget->setColumnWidth(0, 766);
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

    if (!response.founded) {
        qDebug() << "Queried Passenger Not Found!!";
    }

    ui->passengerNameLabel_2->setText(response.interface.result[0].userInfo.passengerName);
    ui->passengerCodeLabel_2->setText(response.interface.result[0].userInfo.certificateNumber);
    ui->passengerFlightLabel_2->setText(response.interface.result[0].userInfo.flightNo);
    ui->boardingNumberLabel_2->setText(response.interface.result[0].userInfo.seatId);

    // 人证验证
    ui->flowTableWidget->setRowHeight(0, 226);
    ui->flowTableWidget->insertRow(0);
    ui->flowTableWidget->setRowHeight(0, 226);

    QLabel *securityLabel = new QLabel();
    securityLabel->setText("人证验证");
    securityLabel->setFixedSize(151, 226);
    securityLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    securityLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

    QImage securityPhotoImage;
    securityPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
    securityPhotoImage = securityPhotoImage.scaled(131
                                                   , 171
                                                   , Qt::IgnoreAspectRatio
                                                   , Qt::SmoothTransformation);
    QPixmap securityPhotoPixmap = QPixmap::fromImage(securityPhotoImage);
    QLabel *securityPhotoLabel = new QLabel();
    securityPhotoLabel->setPixmap(securityPhotoPixmap);
    securityPhotoLabel->setFixedWidth(151);
    securityPhotoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *securityGateTitleLabel = new QLabel();
    securityGateTitleLabel->setText("通过位置：");
    securityGateTitleLabel->setFixedSize(120, 38);
    securityGateTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

    QLabel *securityTimeTitleLabel = new QLabel();
    securityTimeTitleLabel->setText("通过时间：");
    securityTimeTitleLabel->setFixedSize(120, 38);
    securityTimeTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

    QVBoxLayout *securityTitleVBoxLayout = new QVBoxLayout;
    securityTitleVBoxLayout->addWidget(securityGateTitleLabel);
    securityTitleVBoxLayout->addWidget(securityTimeTitleLabel);

    QLabel *securityGateValueLabel = new QLabel();
    securityGateValueLabel->setText("2号自助验证闸机");
    securityGateValueLabel->setFixedSize(200,38);
    securityGateValueLabel->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QLabel *securityTimeValueLabel_1 = new QLabel();
    securityTimeValueLabel_1->setText("2019／09／12");
    securityTimeValueLabel_1->setFixedSize(200, 38);
    securityTimeValueLabel_1->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QLabel *securityTimeValueLabel_2 = new QLabel();
    securityTimeValueLabel_2->setText("15：32：26");
    securityTimeValueLabel_2->setFixedSize(200, 38);
    securityTimeValueLabel_2->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QImage securityModalImage;
    securityModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
    securityModalImage = securityModalImage.scaled(169
                                                   , 50
                                                   , Qt::IgnoreAspectRatio
                                                   , Qt::SmoothTransformation);
    QPixmap securityModalPixmap = QPixmap::fromImage(securityModalImage);
    QLabel *securityModalLabel = new QLabel();
    securityModalLabel->setFixedSize(169, 50);
    securityModalLabel->setPixmap(securityModalPixmap);
    securityModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

    QVBoxLayout *securityValueVBoxLayout = new QVBoxLayout;
    securityValueVBoxLayout->addWidget(securityGateValueLabel);
    securityValueVBoxLayout->addWidget(securityTimeValueLabel_1);
    securityValueVBoxLayout->addWidget(securityTimeValueLabel_2);
    securityValueVBoxLayout->addWidget(securityModalLabel);

    QHBoxLayout *securityHBoxLayOut = new QHBoxLayout;
    securityHBoxLayOut->addWidget(securityLabel);
    securityHBoxLayOut->addWidget(securityPhotoLabel);
    securityHBoxLayOut->addLayout(securityTitleVBoxLayout);

    QWidget *securityWidget = new QWidget();
    QGridLayout *securityLayOut = new QGridLayout(securityWidget);
    securityLayOut->addLayout(securityHBoxLayOut, 0, 0);
    securityLayOut->addLayout(securityValueVBoxLayout, 0, 1);

    ui->flowTableWidget->setCellWidget(0, 0, securityWidget);

    // 通道复核
    ui->flowTableWidget->setRowHeight(1, 226);
    ui->flowTableWidget->insertRow(1);
    ui->flowTableWidget->setRowHeight(1, 226);

    QLabel *reviewgLabel = new QLabel();
    reviewgLabel->setText("通道复核");
    reviewgLabel->setFixedSize(151, 226);
    reviewgLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    reviewgLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

    QImage reviewgPhotoImage;
    reviewgPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
    reviewgPhotoImage = reviewgPhotoImage.scaled(131
                                                 , 171
                                                 , Qt::IgnoreAspectRatio
                                                 , Qt::SmoothTransformation);
    QPixmap reviewgPhotoPixmap = QPixmap::fromImage(reviewgPhotoImage);
    QLabel *reviewgPhotoLabel = new QLabel();
    reviewgPhotoLabel->setPixmap(reviewgPhotoPixmap);
    reviewgPhotoLabel->setFixedWidth(151);
    reviewgPhotoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *reviewgGateTitleLabel = new QLabel();
    reviewgGateTitleLabel->setText("通过位置：");
    reviewgGateTitleLabel->setFixedSize(120, 38);
    reviewgGateTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

    QLabel *reviewgTimeTitleLabel = new QLabel();
    reviewgTimeTitleLabel->setText("通过时间：");
    reviewgTimeTitleLabel->setFixedSize(120, 38);
    reviewgTimeTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

    QVBoxLayout *reviewgTitleVBoxLayout = new QVBoxLayout;
    reviewgTitleVBoxLayout->addWidget(reviewgGateTitleLabel);
    reviewgTitleVBoxLayout->addWidget(reviewgTimeTitleLabel);

    QLabel *reviewgGateValueLabel = new QLabel();
    reviewgGateValueLabel->setText("2号自助验证闸机");
    reviewgGateValueLabel->setFixedSize(200,38);
    reviewgGateValueLabel->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QLabel *reviewgTimeValueLabel_1 = new QLabel();
    reviewgTimeValueLabel_1->setText("2019／09／12");
    reviewgTimeValueLabel_1->setFixedSize(200, 38);
    reviewgTimeValueLabel_1->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QLabel *reviewgTimeValueLabel_2 = new QLabel();
    reviewgTimeValueLabel_2->setText("15：32：26");
    reviewgTimeValueLabel_2->setFixedSize(200, 38);
    reviewgTimeValueLabel_2->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QImage reviewgModalImage;
    reviewgModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
    reviewgModalImage = reviewgModalImage.scaled(169
                                                 , 50
                                                 , Qt::IgnoreAspectRatio
                                                 , Qt::SmoothTransformation);
    QPixmap reviewgModalPixmap = QPixmap::fromImage(reviewgModalImage);
    QLabel *reviewgModalLabel = new QLabel();
    reviewgModalLabel->setFixedSize(169, 50);
    reviewgModalLabel->setPixmap(reviewgModalPixmap);
    reviewgModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

    QVBoxLayout *reviewgValueVBoxLayout = new QVBoxLayout;
    reviewgValueVBoxLayout->addWidget(reviewgGateValueLabel);
    reviewgValueVBoxLayout->addWidget(reviewgTimeValueLabel_1);
    reviewgValueVBoxLayout->addWidget(reviewgTimeValueLabel_2);
    reviewgValueVBoxLayout->addWidget(reviewgModalLabel);

    QHBoxLayout *reviewgHBoxLayOut = new QHBoxLayout;
    reviewgHBoxLayOut->addWidget(reviewgLabel);
    reviewgHBoxLayOut->addWidget(reviewgPhotoLabel);
    reviewgHBoxLayOut->addLayout(reviewgTitleVBoxLayout);

    QWidget *reviewgWidget = new QWidget();
    QGridLayout *reviewgLayOut = new QGridLayout(reviewgWidget);
    reviewgLayOut->addLayout(reviewgHBoxLayOut, 0, 0);
    reviewgLayOut->addLayout(reviewgValueVBoxLayout, 0, 1);

    ui->flowTableWidget->setCellWidget(1, 0, reviewgWidget);

    // 登机口复核
    ui->flowTableWidget->setRowHeight(2, 226);
    ui->flowTableWidget->insertRow(2);
    ui->flowTableWidget->setRowHeight(2, 226);

    QLabel *revieweLabel = new QLabel();
    revieweLabel->setText("登机口复核");
    revieweLabel->setFixedSize(151, 226);
    revieweLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    revieweLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

    QImage reviewePhotoImage;
    reviewePhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
    reviewePhotoImage = reviewePhotoImage.scaled(131
                                                 , 171
                                                 , Qt::IgnoreAspectRatio
                                                 , Qt::SmoothTransformation);
    QPixmap reviewePhotoPixmap = QPixmap::fromImage(reviewePhotoImage);
    QLabel *reviewePhotoLabel = new QLabel();
    reviewePhotoLabel->setPixmap(reviewePhotoPixmap);
    reviewePhotoLabel->setFixedWidth(151);
    reviewePhotoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *revieweGateTitleLabel = new QLabel();
    revieweGateTitleLabel->setText("通过位置：");
    revieweGateTitleLabel->setFixedSize(120, 38);
    revieweGateTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

    QLabel *revieweTimeTitleLabel = new QLabel();
    revieweTimeTitleLabel->setText("通过时间：");
    revieweTimeTitleLabel->setFixedSize(120, 38);
    revieweTimeTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

    QVBoxLayout *revieweTitleVBoxLayout = new QVBoxLayout;
    revieweTitleVBoxLayout->addWidget(revieweGateTitleLabel);
    revieweTitleVBoxLayout->addWidget(revieweTimeTitleLabel);

    QLabel *revieweGateValueLabel = new QLabel();
    revieweGateValueLabel->setText("2号自助验证闸机");
    revieweGateValueLabel->setFixedSize(200,38);
    revieweGateValueLabel->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QLabel *revieweTimeValueLabel_1 = new QLabel();
    revieweTimeValueLabel_1->setText("2019／09／12");
    revieweTimeValueLabel_1->setFixedSize(200, 38);
    revieweTimeValueLabel_1->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QLabel *revieweTimeValueLabel_2 = new QLabel();
    revieweTimeValueLabel_2->setText("15：32：26");
    revieweTimeValueLabel_2->setFixedSize(200, 38);
    revieweTimeValueLabel_2->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

    QImage revieweModalImage;
    revieweModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
    revieweModalImage = revieweModalImage.scaled(169
                                                 , 50
                                                 , Qt::IgnoreAspectRatio
                                                 , Qt::SmoothTransformation);
    QPixmap revieweModalPixmap = QPixmap::fromImage(revieweModalImage);
    QLabel *revieweModalLabel = new QLabel();
    revieweModalLabel->setFixedSize(169, 50);
    revieweModalLabel->setPixmap(revieweModalPixmap);
    revieweModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

    QVBoxLayout *revieweValueVBoxLayout = new QVBoxLayout;
    revieweValueVBoxLayout->addWidget(revieweGateValueLabel);
    revieweValueVBoxLayout->addWidget(revieweTimeValueLabel_1);
    revieweValueVBoxLayout->addWidget(revieweTimeValueLabel_2);
    revieweValueVBoxLayout->addWidget(revieweModalLabel);

    QHBoxLayout *revieweHBoxLayOut = new QHBoxLayout;
    revieweHBoxLayOut->addWidget(revieweLabel);
    revieweHBoxLayOut->addWidget(reviewePhotoLabel);
    revieweHBoxLayOut->addLayout(revieweTitleVBoxLayout);

    QWidget *revieweWidget = new QWidget();
    QGridLayout *revieweLayOut = new QGridLayout(revieweWidget);
    revieweLayOut->addLayout(revieweHBoxLayOut, 0, 0);
    revieweLayOut->addLayout(revieweValueVBoxLayout, 0, 1);

    ui->flowTableWidget->setCellWidget(2, 0, revieweWidget);
}
