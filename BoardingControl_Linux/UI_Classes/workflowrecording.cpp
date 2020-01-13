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

        QWidget *transferWidget = new QWidget();
        transferWidget->setFixedSize(766, 226);
        QLabel *transferLabel = new QLabel(transferWidget);
        transferLabel->setGeometry(0, 0, 180, 226);
        transferLabel->setText("人证验证");
        transferLabel->setFixedSize(180, 226);
        transferLabel->setAlignment(Qt::AlignCenter);
        transferLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QImage transferPhotoImage;
        transferPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
        transferPhotoImage = transferPhotoImage.scaled(150
                                                       , 198
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap transferPhotoPixmap = QPixmap::fromImage(transferPhotoImage);
        QLabel *transferPhotoLabel = new QLabel(transferWidget);
        transferPhotoLabel->setGeometry(180, 0, 150, 226);
        transferPhotoLabel->setPixmap(transferPhotoPixmap);
        transferPhotoLabel->setFixedSize(150, 226);
        transferPhotoLabel->setAlignment(Qt::AlignCenter);

        QLabel *transferGateTitleLabel = new QLabel(transferWidget);
        transferGateTitleLabel->setGeometry(330, 14, 144, 38);
        transferGateTitleLabel->setText("通过位置：");
        transferGateTitleLabel->setFixedSize(144, 38);
        transferGateTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        transferGateTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *transferTimeTitleLabel = new QLabel(transferWidget);
        transferTimeTitleLabel->setGeometry(330, 58, 144, 38);
        transferTimeTitleLabel->setText("通过时间：");
        transferTimeTitleLabel->setFixedSize(144, 38);
        transferTimeTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        transferTimeTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *transferGateValueLabel = new QLabel(transferWidget);
        transferGateValueLabel->setGeometry(474, 14, 292, 38);
        transferGateValueLabel->setText("2号自助验证闸机");
        transferGateValueLabel->setFixedSize(292, 38);
        transferGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        transferGateValueLabel->setStyleSheet("iamge: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *transferTimeValueLabel_1 = new QLabel(transferWidget);
        transferTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        transferTimeValueLabel_1->setText("2019／09／12");
        transferTimeValueLabel_1->setFixedSize(292, 38);
        transferTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        transferTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *transferTimeValueLabel_2 = new QLabel(transferWidget);
        transferTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        transferTimeValueLabel_2->setText("15：32：26");
        transferTimeValueLabel_2->setFixedSize(292, 38);
        transferTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        transferTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage transferModalImage;
        transferModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
        transferModalImage = transferModalImage.scaled(169
                                                       , 50
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap transferModalPixmap = QPixmap::fromImage(transferModalImage);
        QLabel *transferModalLabel = new QLabel(transferWidget);
        transferModalLabel->setGeometry(474, 162, 169, 50);
        transferModalLabel->setFixedSize(169, 50);
        transferModalLabel->setPixmap(transferModalPixmap);
        transferModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, transferWidget);
        widgetIndex = widgetIndex + 1;
    }

    if (response.interface.result[0].hasTransferInfo
            && (response.interface.result[0].hasSecurityInfo
                || response.interface.result[0].hasReviewInfo
                || response.interface.result[0].hasBoardingInfo)) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 2);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 2);

        QWidget *splitWidget = new QWidget();
        QLabel *splitLabel = new QLabel(splitWidget);
        splitLabel->setGeometry(13, 0, 740, 2);
        splitLabel->setStyleSheet("image: 0; background: 0; border-radius: 0; border-width: 1px; border-style: dashed; border-color: rgb(135, 183, 194);");

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, splitWidget);
        widgetIndex = widgetIndex + 1;
    }

    // 人证验证
    if (response.interface.result[0].hasSecurityInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

        QWidget *securityWidget = new QWidget();
        securityWidget->setFixedSize(766, 226);
        QLabel *securityLabel = new QLabel(securityWidget);
        securityLabel->setGeometry(0, 0, 180, 226);
        securityLabel->setText("人证验证");
        securityLabel->setFixedSize(180, 226);
        securityLabel->setAlignment(Qt::AlignCenter);
        securityLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QImage securityPhotoImage;
        securityPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
        securityPhotoImage = securityPhotoImage.scaled(150
                                                       , 198
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap securityPhotoPixmap = QPixmap::fromImage(securityPhotoImage);
        QLabel *securityPhotoLabel = new QLabel(securityWidget);
        securityPhotoLabel->setGeometry(180, 0, 150, 226);
        securityPhotoLabel->setPixmap(securityPhotoPixmap);
        securityPhotoLabel->setFixedSize(150, 226);
        securityPhotoLabel->setAlignment(Qt::AlignCenter);

        QLabel *securityGateTitleLabel = new QLabel(securityWidget);
        securityGateTitleLabel->setGeometry(330, 14, 144, 38);
        securityGateTitleLabel->setText("通过位置：");
        securityGateTitleLabel->setFixedSize(144, 38);
        securityGateTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        securityGateTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *securityTimeTitleLabel = new QLabel(securityWidget);
        securityTimeTitleLabel->setGeometry(330, 58, 144, 38);
        securityTimeTitleLabel->setText("通过时间：");
        securityTimeTitleLabel->setFixedSize(144, 38);
        securityTimeTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignHCenter);
        securityTimeTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *securityGateValueLabel = new QLabel(securityWidget);
        securityGateValueLabel->setGeometry(474, 14, 292, 38);
        securityGateValueLabel->setText("2号自助验证闸机");
        securityGateValueLabel->setFixedSize(292, 38);
        securityGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        securityGateValueLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *securityTimeValueLabel_1 = new QLabel(securityWidget);
        securityTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        securityTimeValueLabel_1->setText("2019／09／12");
        securityTimeValueLabel_1->setFixedSize(292, 38);
        securityTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        securityTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *securityTimeValueLabel_2 = new QLabel(securityWidget);
        securityTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        securityTimeValueLabel_2->setText("15：32：26");
        securityTimeValueLabel_2->setFixedSize(292, 38);
        securityTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        securityTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage securityModalImage;
        securityModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
        securityModalImage = securityModalImage.scaled(169
                                                       , 50
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap securityModalPixmap = QPixmap::fromImage(securityModalImage);
        QLabel *securityModalLabel = new QLabel(securityWidget);
        securityModalLabel->setGeometry(474, 162, 169, 50);
        securityModalLabel->setFixedSize(169, 50);
        securityModalLabel->setPixmap(securityModalPixmap);
        securityModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, securityWidget);
        widgetIndex = widgetIndex + 1;
    }

    if (response.interface.result[0].hasSecurityInfo
            && (response.interface.result[0].hasReviewInfo
                || response.interface.result[0].hasBoardingInfo)) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 2);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 2);

        QWidget *splitWidget = new QWidget();
        QLabel *splitLabel = new QLabel(splitWidget);
        splitLabel->setGeometry(13, 0, 740, 2);
        splitLabel->setStyleSheet("image: 0; background: 0; border-radius: 0; border-width: 1px; border-style: dashed; border-color: rgb(135, 183, 194);");

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, splitWidget);
        widgetIndex = widgetIndex + 1;
    }

    // 通道复核
    if (response.interface.result[0].hasReviewInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

        QWidget *reviewWidget = new QWidget();
        reviewWidget->setFixedSize(766, 226);
        QLabel *reviewLabel = new QLabel(reviewWidget);
        reviewLabel->setGeometry(0, 0, 180, 226);
        reviewLabel->setText("通道复核");
        reviewLabel->setFixedSize(180, 226);
        reviewLabel->setAlignment(Qt::AlignCenter);
        reviewLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QImage reviewPhotoImage;
        reviewPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
        reviewPhotoImage = reviewPhotoImage.scaled(150
                                                     , 198
                                                     , Qt::IgnoreAspectRatio
                                                     , Qt::SmoothTransformation);
        QPixmap reviewPhotoPixmap = QPixmap::fromImage(reviewPhotoImage);
        QLabel *reviewPhotoLabel = new QLabel(reviewWidget);
        reviewPhotoLabel->setGeometry(180, 0, 150, 226);
        reviewPhotoLabel->setPixmap(reviewPhotoPixmap);
        reviewPhotoLabel->setFixedSize(150, 226);
        reviewPhotoLabel->setAlignment(Qt::AlignCenter);

        QLabel *reviewGateTitleLabel = new QLabel(reviewWidget);
        reviewGateTitleLabel->setGeometry(330, 14, 144, 38);
        reviewGateTitleLabel->setText("通过位置：");
        reviewGateTitleLabel->setFixedSize(144, 38);
        reviewGateTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        reviewGateTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *reviewTimeTitleLabel = new QLabel(reviewWidget);
        reviewTimeTitleLabel->setGeometry(330, 58, 144, 38);
        reviewTimeTitleLabel->setText("通过时间：");
        reviewTimeTitleLabel->setFixedSize(144, 38);
        reviewTimeTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        reviewTimeTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *reviewGateValueLabel = new QLabel(reviewWidget);
        reviewGateValueLabel->setGeometry(474, 14, 292, 38);
        reviewGateValueLabel->setText("2号自助验证闸机");
        reviewGateValueLabel->setFixedSize(292, 38);
        reviewGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        reviewGateValueLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *reviewTimeValueLabel_1 = new QLabel(reviewWidget);
        reviewTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        reviewTimeValueLabel_1->setText("2019／09／12");
        reviewTimeValueLabel_1->setFixedSize(292, 38);
        reviewTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        reviewTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *reviewTimeValueLabel_2 = new QLabel(reviewWidget);
        reviewTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        reviewTimeValueLabel_2->setText("15：32：26");
        reviewTimeValueLabel_2->setFixedSize(292, 38);
        reviewTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        reviewTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage reviewModalImage;
        reviewModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
        reviewModalImage = reviewModalImage.scaled(169
                                                     , 50
                                                     , Qt::IgnoreAspectRatio
                                                     , Qt::SmoothTransformation);
        QPixmap reviewModalPixmap = QPixmap::fromImage(reviewModalImage);
        QLabel *reviewModalLabel = new QLabel(reviewWidget);
        reviewModalLabel->setGeometry(474, 162, 169, 50);
        reviewModalLabel->setFixedSize(169, 50);
        reviewModalLabel->setPixmap(reviewModalPixmap);
        reviewModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, reviewWidget);
        widgetIndex = widgetIndex + 1;
    }

    if (response.interface.result[0].hasReviewInfo
            && response.interface.result[0].hasBoardingInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 2);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 2);

        QWidget *splitWidget = new QWidget();
        QLabel *splitLabel = new QLabel(splitWidget);
        splitLabel->setGeometry(13, 0, 740, 2);
        splitLabel->setStyleSheet("image: 0; background: 0; border-radius: 0; border-width: 1px; border-style: dashed; border-color: rgb(135, 183, 194);");

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, splitWidget);
        widgetIndex = widgetIndex + 1;
    }

    // 登机口复核
    if (response.interface.result[0].hasBoardingInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

        QWidget *boardingWidget = new QWidget();
        QLabel *boardingLabel = new QLabel(boardingWidget);
        boardingLabel->setGeometry(0, 0, 180, 226);
        boardingLabel->setText("登机口复核");
        boardingLabel->setFixedSize(180, 226);
        boardingLabel->setAlignment(Qt::AlignCenter);
        boardingLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QImage boardingPhotoImage;
        boardingPhotoImage.load(":/6航班回查/Images/6航班回查/照片.png");
        boardingPhotoImage = boardingPhotoImage.scaled(150
                                                       , 198
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap boardingPhotoPixmap = QPixmap::fromImage(boardingPhotoImage);
        QLabel *boardingPhotoLabel = new QLabel(boardingWidget);
        boardingPhotoLabel->setGeometry(180, 0, 150, 226);
        boardingPhotoLabel->setPixmap(boardingPhotoPixmap);
        boardingPhotoLabel->setFixedSize(150, 226);
        boardingPhotoLabel->setAlignment(Qt::AlignCenter);

        QLabel *boardingGateTitleLabel = new QLabel(boardingWidget);
        boardingGateTitleLabel->setGeometry(330, 14, 144, 38);
        boardingGateTitleLabel->setText("通过位置：");
        boardingGateTitleLabel->setFixedSize(144, 38);
        boardingGateTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        boardingGateTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *boardingTimeTitleLabel = new QLabel(boardingWidget);
        boardingTimeTitleLabel->setGeometry(330, 58, 144, 38);
        boardingTimeTitleLabel->setText("通过时间：");
        boardingTimeTitleLabel->setFixedSize(144, 38);
        boardingTimeTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        boardingTimeTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *boardingGateValueLabel = new QLabel(boardingWidget);
        boardingGateValueLabel->setGeometry(474, 14, 292, 38);
        boardingGateValueLabel->setText("2号自助验证闸机");
        boardingGateValueLabel->setFixedSize(292, 38);
        boardingGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        boardingGateValueLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *boardingTimeValueLabel_1 = new QLabel(boardingWidget);
        boardingTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        boardingTimeValueLabel_1->setText("2019／09／12");
        boardingTimeValueLabel_1->setFixedSize(292, 38);
        boardingTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        boardingTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *boardingTimeValueLabel_2 = new QLabel(boardingWidget);
        boardingTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        boardingTimeValueLabel_2->setText("15：32：26");
        boardingTimeValueLabel_2->setFixedSize(292, 38);
        boardingTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        boardingTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage boardingModalImage;
        boardingModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
        boardingModalImage = boardingModalImage.scaled(169
                                                       , 50
                                                       , Qt::IgnoreAspectRatio
                                                       , Qt::SmoothTransformation);
        QPixmap boardingModalPixmap = QPixmap::fromImage(boardingModalImage);
        QLabel *boardingModalLabel = new QLabel(boardingWidget);
        boardingModalLabel->setGeometry(474, 162, 169, 50);
        boardingModalLabel->setFixedSize(169, 50);
        boardingModalLabel->setPixmap(boardingModalPixmap);
        boardingModalLabel->setStyleSheet("image: 0; border: 0; background: 0;");

        ui->flowTableWidget->setCellWidget(widgetIndex, 0, boardingWidget);
    }
}
