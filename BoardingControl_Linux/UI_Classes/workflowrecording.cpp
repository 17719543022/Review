#include "workflowrecording.h"
#include "ui_workflowrecording.h"
#include "ServerInterface/httpAPI.h"
#include <QGridLayout>
#include "messagedialog.h"

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

QPixmap WorkflowRecording::getQPixmapSync(QString str)
{
    const QUrl url = QUrl::fromUserInput(str);

    QNetworkRequest request(url);
    QNetworkAccessManager *naManager = new QNetworkAccessManager(this);
    QNetworkReply* reply = naManager->get(request);

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QByteArray byteArray = reply->readAll();

    QPixmap pixmap;
    pixmap.loadFromData(byteArray);

    if (pixmap.isNull()) {
        QImage zhaoImage;
        zhaoImage.load(":/4全流程记录/Images/4全流程记录/照片-.png");
        zhaoImage = zhaoImage.scaled(150
                                     , 226
                                     , Qt::IgnoreAspectRatio
                                     , Qt::SmoothTransformation);
        pixmap = QPixmap::fromImage(zhaoImage);
    }

    return pixmap;
}

void WorkflowRecording::on_flowQueryPushButton_clicked()
{
    ui->flowTableWidget->scrollToTop();
    while (ui->flowTableWidget->rowCount() > 0 ) {
        ui->flowTableWidget->removeRow(0);
    }

    FlowReviewRequest request;
    request.input = ui->flowQueryLineEdit->text().toUpper();

    if ((request.input.length() == 0) || (request.input.length() > 18)) {
        MessageDialog msg(this, nullptr, "请输入有效信息!", 1);
        msg.exec();

        return;
    }

    FlowReviewResponse response = HttpAPI::instance()->get(request);

    if (!response.founded) {
        MessageDialog msg(this, nullptr, "请输入有效信息!", 1);
        msg.exec();

        return;
    } else {
        ui->flowQueryLineEdit->clear();
    }

    int flowIndex = 0;
    if (response.interface.flowNum > 1) {
        MessageDialog msg(this, "请选择旅客", "", 0, response.interface.flowNum, response);
        flowIndex = msg.exec();

        if (flowIndex < 65535) {
            return;
        } else {
            flowIndex -= 65535;
        }
    }

    if (response.interface.result[flowIndex].hasUserInfo) {
        ui->passengerNameLabel_2->setText(response.interface.result[flowIndex].userInfo.passengerName);
        ui->passengerCodeLabel_2->setText(response.interface.result[flowIndex].userInfo.certificateNumber);
        ui->passengerFlightLabel_2->setText(response.interface.result[flowIndex].userInfo.flightNo);
        ui->boardingNumberLabel_2->setText(response.interface.result[flowIndex].userInfo.seatId);
    }

    int widgetIndex = 0;

    // 中转/经停采集
    if (response.interface.result[flowIndex].hasTransferInfo) {
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);
        ui->flowTableWidget->insertRow(widgetIndex);
        ui->flowTableWidget->setRowHeight(widgetIndex, 226);

        QWidget *transferWidget = new QWidget();
        transferWidget->setFixedSize(766, 226);
        QLabel *transferLabel = new QLabel(transferWidget);
        transferLabel->setGeometry(0, 0, 180, 226);
        transferLabel->setText("中转/经停\n采集");
        transferLabel->setFixedSize(180, 226);
        transferLabel->setAlignment(Qt::AlignCenter);
        transferLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 20pt; color: rgb(0, 228, 255);");

        QPixmap pixmap = getQPixmapSync(response.interface.result[flowIndex].transferInfo.photoPath);
        pixmap = pixmap.scaled(150
                               , 198
                               , Qt::IgnoreAspectRatio
                               , Qt::SmoothTransformation);
        QLabel *transferPhotoLabel = new QLabel(transferWidget);
        transferPhotoLabel->setGeometry(180, 0, 150, 226);
        transferPhotoLabel->setPixmap(pixmap);
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
        transferGateValueLabel->setText(response.interface.result[flowIndex].transferInfo.gateNo);
        transferGateValueLabel->setFixedSize(292, 38);
        transferGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        transferGateValueLabel->setStyleSheet("iamge: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *transferTimeValueLabel_1 = new QLabel(transferWidget);
        transferTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        QString collectTime = response.interface.result[flowIndex].transferInfo.collectTime;
        transferTimeValueLabel_1->setText(collectTime.left(4) + "／" + collectTime.mid(4, 2) + "／" + collectTime.mid(6, 2));
        transferTimeValueLabel_1->setFixedSize(292, 38);
        transferTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        transferTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *transferTimeValueLabel_2 = new QLabel(transferWidget);
        transferTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        transferTimeValueLabel_2->setText(collectTime.mid(8, 2) + "：" + collectTime.mid(10, 2) + "：" + collectTime.mid(12, 2));
        transferTimeValueLabel_2->setFixedSize(292, 38);
        transferTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        transferTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage transferModalImage;
        int sourceType = response.interface.result[flowIndex].transferInfo.sourceType;
        if (sourceType == 0 || sourceType == 3) {
            transferModalImage.load(":/4全流程记录/Images/4全流程记录/中转采集.png");
        } else if (sourceType == 1 || sourceType == 4) {
            transferModalImage.load(":/4全流程记录/Images/4全流程记录/经停采集.png");
        } else {
            transferModalImage.load(":/4全流程记录/Images/4全流程记录/备降采集2.png");
        }
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

    if (response.interface.result[flowIndex].hasTransferInfo
            && (response.interface.result[flowIndex].hasSecurityInfo
                || response.interface.result[flowIndex].hasReviewInfo
                || response.interface.result[flowIndex].hasBoardingInfo)) {
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
    if (response.interface.result[flowIndex].hasSecurityInfo) {
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

        QPixmap pixmap = getQPixmapSync(response.interface.result[flowIndex].securityInfo.photoPath);
        pixmap = pixmap.scaled(150
                               , 198
                               , Qt::IgnoreAspectRatio
                               , Qt::SmoothTransformation);
        QLabel *securityPhotoLabel = new QLabel(securityWidget);
        securityPhotoLabel->setGeometry(180, 0, 150, 226);
        securityPhotoLabel->setPixmap(pixmap);
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
        securityGateValueLabel->setText(response.interface.result[flowIndex].securityInfo.gateNo);
        securityGateValueLabel->setFixedSize(292, 38);
        securityGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        securityGateValueLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *securityTimeValueLabel_1 = new QLabel(securityWidget);
        securityTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        QString passTime = response.interface.result[flowIndex].securityInfo.passTime;
        securityTimeValueLabel_1->setText(passTime.left(4) + "／" + passTime.mid(4, 2) + "／" + passTime.mid(6, 2));
        securityTimeValueLabel_1->setFixedSize(292, 38);
        securityTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        securityTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *securityTimeValueLabel_2 = new QLabel(securityWidget);
        securityTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        securityTimeValueLabel_2->setText(passTime.mid(8, 2) + "：" + passTime.mid(10, 2) + "：" + passTime.mid(12, 2));
        securityTimeValueLabel_2->setFixedSize(292, 38);
        securityTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        securityTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage securityModalImage;
        int passType = response.interface.result[flowIndex].securityInfo.passType;
        int passStatus = response.interface.result[flowIndex].securityInfo.passStatus;
        switch (passType) {
        case 0:
        case 2:
            switch (passStatus) {
            case 0:
                securityModalImage.load(":/4全流程记录/Images/4全流程记录/系统验证通过.png");
                break;
            case 1:
                securityModalImage.load(":/4全流程记录/Images/4全流程记录/系统验证不通过.png");
                break;
            case 3:
                securityModalImage.load(":/4全流程记录/Images/4全流程记录/证件失效.png");
                break;
            default:
                securityModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
                break;
            }
            break;
        case 1:
            switch (passStatus) {
            case 0:
                securityModalImage.load(":/4全流程记录/Images/4全流程记录/人工验证通过.png");
                break;
            case 1:
                securityModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
                break;
            default:
                securityModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
                break;
            }
            break;
        case 3:
            securityModalImage.load(":/4全流程记录/Images/4全流程记录/人工验证通过.png");
            break;
        default:
            securityModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
            break;
        }
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

    if (response.interface.result[flowIndex].hasSecurityInfo
            && (response.interface.result[flowIndex].hasReviewInfo
                || response.interface.result[flowIndex].hasBoardingInfo)) {
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
    if (response.interface.result[flowIndex].hasReviewInfo) {
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

        QPixmap pixmap = getQPixmapSync(response.interface.result[flowIndex].reviewInfo.photoPath);
        pixmap = pixmap.scaled(150
                               , 198
                               , Qt::IgnoreAspectRatio
                               , Qt::SmoothTransformation);
        QLabel *reviewPhotoLabel = new QLabel(reviewWidget);
        reviewPhotoLabel->setGeometry(180, 0, 150, 226);
        reviewPhotoLabel->setPixmap(pixmap);
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
        reviewGateValueLabel->setText(response.interface.result[flowIndex].reviewInfo.gateNo);
        reviewGateValueLabel->setFixedSize(292, 38);
        reviewGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        reviewGateValueLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *reviewTimeValueLabel_1 = new QLabel(reviewWidget);
        reviewTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        QString passTime = response.interface.result[flowIndex].reviewInfo.passTime;
        reviewTimeValueLabel_1->setText(passTime.left(4) + "／" + passTime.mid(4, 2)+ "／" + passTime.mid(6, 2));
        reviewTimeValueLabel_1->setFixedSize(292, 38);
        reviewTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        reviewTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *reviewTimeValueLabel_2 = new QLabel(reviewWidget);
        reviewTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        reviewTimeValueLabel_2->setText(passTime.mid(8, 2) + "：" + passTime.mid(10, 2) + "：" + passTime.mid(12, 2));
        reviewTimeValueLabel_2->setFixedSize(292, 38);
        reviewTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        reviewTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage reviewModalImage;
        int passType = response.interface.result[flowIndex].reviewInfo.passType;
        int passStatus = response.interface.result[flowIndex].reviewInfo.passStatus;
        switch (passType) {
        case 0:
        case 2:
            switch (passStatus) {
            case 0:
                reviewModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
                break;
            case 1:
                reviewModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
                break;
            default:
                reviewModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
                break;
            }
            break;
        case 1:
            switch (passStatus) {
            case 0:
                reviewModalImage.load(":/4全流程记录/Images/4全流程记录/人工复核通过.png");
                break;
            case 1:
                reviewModalImage.load(":/4全流程记录/Images/4全流程记录/icon_ylj.png");
                break;
            default:
                reviewModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
                break;
            }
            break;
        case 3:
            reviewModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
            break;
        default:
            reviewModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
            break;
        }
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

    if (response.interface.result[flowIndex].hasReviewInfo
            && response.interface.result[flowIndex].hasBoardingInfo) {
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
    if (response.interface.result[flowIndex].hasBoardingInfo) {
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

        QPixmap pixmap = getQPixmapSync(response.interface.result[flowIndex].boardingInfo.photoPath);
        pixmap = pixmap.scaled(150
                               , 198
                               , Qt::IgnoreAspectRatio
                               , Qt::SmoothTransformation);
        QLabel *boardingPhotoLabel = new QLabel(boardingWidget);
        boardingPhotoLabel->setGeometry(180, 0, 150, 226);
        boardingPhotoLabel->setPixmap(pixmap);
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
        boardingGateValueLabel->setText(response.interface.result[flowIndex].boardingInfo.gateNo);
        boardingGateValueLabel->setFixedSize(292, 38);
        boardingGateValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        boardingGateValueLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *boardingTimeValueLabel_1 = new QLabel(boardingWidget);
        boardingTimeValueLabel_1->setGeometry(474, 58, 292, 38);
        QString passTime = response.interface.result[flowIndex].boardingInfo.passTime;
        boardingTimeValueLabel_1->setText(passTime.left(4) + "／" + passTime.mid(4, 2) + "／" + passTime.mid(6, 2));
        boardingTimeValueLabel_1->setFixedSize(292, 38);
        boardingTimeValueLabel_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        boardingTimeValueLabel_1->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *boardingTimeValueLabel_2 = new QLabel(boardingWidget);
        boardingTimeValueLabel_2->setGeometry(474, 102, 292, 38);
        boardingTimeValueLabel_2->setText(passTime.mid(8, 2) + "：" + passTime.mid(10, 2) + "：" + passTime.mid(12, 2));
        boardingTimeValueLabel_2->setFixedSize(292, 38);
        boardingTimeValueLabel_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        boardingTimeValueLabel_2->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QImage boardingModalImage;
        int passType = response.interface.result[flowIndex].boardingInfo.passType;
        int passStatus = response.interface.result[flowIndex].boardingInfo.passStatus;
        switch (passType) {
        case 0:
            if (passStatus == 0) {
                boardingModalImage.load(":/4全流程记录/Images/4全流程记录/系统复核通过.png");
            } else {
                boardingModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
            }
            break;
        case 1:
            if (passStatus == 0) {
                boardingModalImage.load(":/4全流程记录/Images/4全流程记录/人工复核通过.png");
            } else {
                boardingModalImage.load(":/4全流程记录/Images/4全流程记录/icon_wjl.png");
            }
            break;
        case 2:
            boardingModalImage.load(":/4全流程记录/Images/4全流程记录/icon_ylj.png");
            break;
        default:
            break;
        }
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
