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

    if (response.interface.result[0].hasUserInfo) {
        ui->passengerNameLabel_2->setText(response.interface.result[0].userInfo.passengerName);
        ui->passengerCodeLabel_2->setText(response.interface.result[0].userInfo.certificateNumber);
        ui->passengerFlightLabel_2->setText(response.interface.result[0].userInfo.flightNo);
        ui->boardingNumberLabel_2->setText(response.interface.result[0].userInfo.seatId);
    }

    int widgetIndex = 0;

    // 中转/经停采集
    if (response.interface.result[0].hasTransferInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

        QLabel *transferLabel = new QLabel();
        transferLabel->setText("人证验证");
        transferLabel->setFixedSize(151, 226);
        transferLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        transferLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QImage transferPhotoImage;
        transferPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
        transferPhotoImage = transferPhotoImage.scaled(131
                                                       , 171
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap transferPhotoPixmap = QPixmap::fromImage(transferPhotoImage);
        QLabel *transferPhotoLabel = new QLabel();
        transferPhotoLabel->setPixmap(transferPhotoPixmap);
        transferPhotoLabel->setFixedWidth(151);
        transferPhotoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QLabel *transferGateTitleLabel = new QLabel();
        transferGateTitleLabel->setText("通过位置：");
        transferGateTitleLabel->setFixedSize(120, 38);
        transferGateTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

        QLabel *transferTimeTitleLabel = new QLabel();
        transferTimeTitleLabel->setText("通过时间：");
        transferTimeTitleLabel->setFixedSize(120, 38);
        transferTimeTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

        QVBoxLayout *transferTitleVBoxLayout = new QVBoxLayout;
        transferTitleVBoxLayout->addWidget(transferGateTitleLabel);
        transferTitleVBoxLayout->addWidget(transferTimeTitleLabel);

        QLabel *transferGateValueLabel = new QLabel();
        transferGateValueLabel->setText("2号自助验证闸机");
        transferGateValueLabel->setFixedSize(200,38);
        transferGateValueLabel->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *transferTimeValueLabel_1 = new QLabel();
        transferTimeValueLabel_1->setText("2019／09／12");
        transferTimeValueLabel_1->setFixedSize(200, 38);
        transferTimeValueLabel_1->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *transferTimeValueLabel_2 = new QLabel();
        transferTimeValueLabel_2->setText("15：32：26");
        transferTimeValueLabel_2->setFixedSize(200, 38);
        transferTimeValueLabel_2->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QImage transferModalImage;
        transferModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
        transferModalImage = transferModalImage.scaled(169
                                                       , 50
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap transferModalPixmap = QPixmap::fromImage(transferModalImage);
        QLabel *transferModalLabel = new QLabel();
        transferModalLabel->setFixedSize(169, 50);
        transferModalLabel->setPixmap(transferModalPixmap);
        transferModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

        QVBoxLayout *transferValueVBoxLayout = new QVBoxLayout;
        transferValueVBoxLayout->addWidget(transferGateValueLabel);
        transferValueVBoxLayout->addWidget(transferTimeValueLabel_1);
        transferValueVBoxLayout->addWidget(transferTimeValueLabel_2);
        transferValueVBoxLayout->addWidget(transferModalLabel);

        QHBoxLayout *transferHBoxLayOut = new QHBoxLayout;
        transferHBoxLayOut->addWidget(transferLabel);
        transferHBoxLayOut->addWidget(transferPhotoLabel);
        transferHBoxLayOut->addLayout(transferTitleVBoxLayout);

        QWidget *transferWidget = new QWidget();
        QGridLayout *transferLayOut = new QGridLayout(transferWidget);
        transferLayOut->addLayout(transferHBoxLayOut, 0, 0);
        transferLayOut->addLayout(transferValueVBoxLayout, 0, 1);

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, transferWidget);
        widgetIndex = widgetIndex + 1;
    }

    // 人证验证
    if (response.interface.result[0].hasSecurityInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

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

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, securityWidget);
        widgetIndex = widgetIndex + 1;
    }

    // 通道复核
    if (response.interface.result[0].hasReviewInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

        QLabel *reviewLabel = new QLabel();
        reviewLabel->setText("通道复核");
        reviewLabel->setFixedSize(151, 226);
        reviewLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        reviewLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QImage reviewPhotoImage;
        reviewPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
        reviewPhotoImage = reviewPhotoImage.scaled(131
                                                     , 171
                                                     , Qt::IgnoreAspectRatio
                                                     , Qt::SmoothTransformation);
        QPixmap reviewPhotoPixmap = QPixmap::fromImage(reviewPhotoImage);
        QLabel *reviewPhotoLabel = new QLabel();
        reviewPhotoLabel->setPixmap(reviewPhotoPixmap);
        reviewPhotoLabel->setFixedWidth(151);
        reviewPhotoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QLabel *reviewGateTitleLabel = new QLabel();
        reviewGateTitleLabel->setText("通过位置：");
        reviewGateTitleLabel->setFixedSize(120, 38);
        reviewGateTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

        QLabel *reviewTimeTitleLabel = new QLabel();
        reviewTimeTitleLabel->setText("通过时间：");
        reviewTimeTitleLabel->setFixedSize(120, 38);
        reviewTimeTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

        QVBoxLayout *reviewTitleVBoxLayout = new QVBoxLayout;
        reviewTitleVBoxLayout->addWidget(reviewGateTitleLabel);
        reviewTitleVBoxLayout->addWidget(reviewTimeTitleLabel);

        QLabel *reviewGateValueLabel = new QLabel();
        reviewGateValueLabel->setText("2号自助验证闸机");
        reviewGateValueLabel->setFixedSize(200,38);
        reviewGateValueLabel->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *reviewTimeValueLabel_1 = new QLabel();
        reviewTimeValueLabel_1->setText("2019／09／12");
        reviewTimeValueLabel_1->setFixedSize(200, 38);
        reviewTimeValueLabel_1->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *reviewTimeValueLabel_2 = new QLabel();
        reviewTimeValueLabel_2->setText("15：32：26");
        reviewTimeValueLabel_2->setFixedSize(200, 38);
        reviewTimeValueLabel_2->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QImage reviewModalImage;
        reviewModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
        reviewModalImage = reviewModalImage.scaled(169
                                                     , 50
                                                     , Qt::IgnoreAspectRatio
                                                     , Qt::SmoothTransformation);
        QPixmap reviewModalPixmap = QPixmap::fromImage(reviewModalImage);
        QLabel *reviewModalLabel = new QLabel();
        reviewModalLabel->setFixedSize(169, 50);
        reviewModalLabel->setPixmap(reviewModalPixmap);
        reviewModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

        QVBoxLayout *reviewValueVBoxLayout = new QVBoxLayout;
        reviewValueVBoxLayout->addWidget(reviewGateValueLabel);
        reviewValueVBoxLayout->addWidget(reviewTimeValueLabel_1);
        reviewValueVBoxLayout->addWidget(reviewTimeValueLabel_2);
        reviewValueVBoxLayout->addWidget(reviewModalLabel);

        QHBoxLayout *reviewHBoxLayOut = new QHBoxLayout;
        reviewHBoxLayOut->addWidget(reviewLabel);
        reviewHBoxLayOut->addWidget(reviewPhotoLabel);
        reviewHBoxLayOut->addLayout(reviewTitleVBoxLayout);

        QWidget *reviewWidget = new QWidget();
        QGridLayout *reviewLayOut = new QGridLayout(reviewWidget);
        reviewLayOut->addLayout(reviewHBoxLayOut, 0, 0);
        reviewLayOut->addLayout(reviewValueVBoxLayout, 0, 1);

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, reviewWidget);
        widgetIndex = widgetIndex + 1;
    }

    // 登机口复核
    if (response.interface.result[0].hasBoardingInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

        QLabel *boardingLabel = new QLabel();
        boardingLabel->setText("登机口复核");
        boardingLabel->setFixedSize(151, 226);
        boardingLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        boardingLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QImage boardingPhotoImage;
        boardingPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
        boardingPhotoImage = boardingPhotoImage.scaled(131
                                                       , 171
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap boardingPhotoPixmap = QPixmap::fromImage(boardingPhotoImage);
        QLabel *boardingPhotoLabel = new QLabel();
        boardingPhotoLabel->setPixmap(boardingPhotoPixmap);
        boardingPhotoLabel->setFixedWidth(151);
        boardingPhotoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QLabel *boardingGateTitleLabel = new QLabel();
        boardingGateTitleLabel->setText("通过位置：");
        boardingGateTitleLabel->setFixedSize(120, 38);
        boardingGateTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

        QLabel *boardingTimeTitleLabel = new QLabel();
        boardingTimeTitleLabel->setText("通过时间：");
        boardingTimeTitleLabel->setFixedSize(120, 38);
        boardingTimeTitleLabel->setStyleSheet("font: 19pt; color: rgb(255, 255, 255);");

        QVBoxLayout *boardingTitleVBoxLayout = new QVBoxLayout;
        boardingTitleVBoxLayout->addWidget(boardingGateTitleLabel);
        boardingTitleVBoxLayout->addWidget(boardingTimeTitleLabel);

        QLabel *boardingGateValueLabel = new QLabel();
        boardingGateValueLabel->setText("2号自助验证闸机");
        boardingGateValueLabel->setFixedSize(200,38);
        boardingGateValueLabel->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *boardingTimeValueLabel_1 = new QLabel();
        boardingTimeValueLabel_1->setText("2019／09／12");
        boardingTimeValueLabel_1->setFixedSize(200, 38);
        boardingTimeValueLabel_1->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *boardingTimeValueLabel_2 = new QLabel();
        boardingTimeValueLabel_2->setText("15：32：26");
        boardingTimeValueLabel_2->setFixedSize(200, 38);
        boardingTimeValueLabel_2->setStyleSheet("font: bold 19pt; color: rgb(0, 228, 255);");

        QImage boardingModalImage;
        boardingModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
        boardingModalImage = boardingModalImage.scaled(169
                                                       , 50
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap boardingModalPixmap = QPixmap::fromImage(boardingModalImage);
        QLabel *boardingModalLabel = new QLabel();
        boardingModalLabel->setFixedSize(169, 50);
        boardingModalLabel->setPixmap(boardingModalPixmap);
        boardingModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

        QVBoxLayout *boardingValueVBoxLayout = new QVBoxLayout;
        boardingValueVBoxLayout->addWidget(boardingGateValueLabel);
        boardingValueVBoxLayout->addWidget(boardingTimeValueLabel_1);
        boardingValueVBoxLayout->addWidget(boardingTimeValueLabel_2);
        boardingValueVBoxLayout->addWidget(boardingModalLabel);

        QHBoxLayout *boardingHBoxLayOut = new QHBoxLayout;
        boardingHBoxLayOut->addWidget(boardingLabel);
        boardingHBoxLayOut->addWidget(boardingPhotoLabel);
        boardingHBoxLayOut->addLayout(boardingTitleVBoxLayout);

        QWidget *boardingWidget = new QWidget();
        QGridLayout *boardingLayOut = new QGridLayout(boardingWidget);
        boardingLayOut->addLayout(boardingHBoxLayOut, 0, 0);
        boardingLayOut->addLayout(boardingValueVBoxLayout, 0, 1);

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, boardingWidget);
    }
}
