#include "flightenquires.h"
#include "ui_flightenquires.h"
#include "ServerInterface/httpAPI.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVBoxLayout>
#include "messagedialog.h"
#include "settings.h"
#include <QUuid>

ButtonWidget::ButtonWidget(QWidget *parent, bool isStatisticsMode, Ui::DisplayTab tab, int widgetIndex)
    : QWidget(parent),
      isStatisticsMode(isStatisticsMode),
      widgetIndex(widgetIndex)
{
    QSignalMapper *signalMapper = new QSignalMapper();
    if (isStatisticsMode && (tab == Ui::DisplayTab::DepositoryTab)) {
        QPushButton *removePushButton = new QPushButton(this);
        removePushButton->setGeometry(600, 134, 140, 40);
        removePushButton->setText("删    除");
        removePushButton->setFixedSize(140, 40);
        removePushButton->setStyleSheet("image: 0; border: 0; border-radius: 4; background: rgb(255, 0, 0); font: 19pt; color: rgb(255, 255, 255);");

        connect(removePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(removePushButton, widgetIndex);
    }

    connect(signalMapper, SIGNAL(mapped(int)), parent, SLOT(on_removeRow_clicked(int)));
}

FlightEnquires::FlightEnquires(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightEnquires),
    flight(QString()),
    isStatisticsMode(false),
    orgDepFilledNum(0),
    orgDepFillIndex(0),
    boardingFilledNum(0),
    boardingFillIndex(0),
    notboardingFilledNum(0),
    notboardingFillIndex(0)
{
    signalMapper = new QSignalMapper(this);

    ui->setupUi(this);
    this->hide();

    ui->orgDepTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->orgDepTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->orgDepTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->orgDepTableWidget->verticalHeader()->setVisible(false);
    ui->orgDepTableWidget->horizontalHeader()->setVisible(false);
    ui->orgDepTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->orgDepTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->orgDepTableWidget->setStyleSheet("image: 0; border: 0; background: transparent;");
    ui->orgDepTableWidget->setShowGrid(false);
    ui->orgDepTableWidget->setColumnCount(1);
    ui->orgDepTableWidget->setColumnWidth(0, 766);

    ui->boardingTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->boardingTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->boardingTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->boardingTableWidget->verticalHeader()->setVisible(false);
    ui->boardingTableWidget->horizontalHeader()->setVisible(false);
    ui->boardingTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->boardingTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->boardingTableWidget->setStyleSheet("image: 0; border: 0; background: transparent;");
    ui->boardingTableWidget->setShowGrid(false);
    ui->boardingTableWidget->setColumnCount(1);
    ui->boardingTableWidget->setColumnWidth(0, 766);

    ui->notboardingTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->notboardingTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->notboardingTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->notboardingTableWidget->verticalHeader()->setVisible(false);
    ui->notboardingTableWidget->horizontalHeader()->setVisible(false);
    ui->notboardingTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->notboardingTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->notboardingTableWidget->setStyleSheet("image: 0; border: 0; background: transparent;");
    ui->notboardingTableWidget->setShowGrid(false);
    ui->notboardingTableWidget->setColumnCount(1);
    ui->notboardingTableWidget->setColumnWidth(0, 766);

    connect((QWidget*)ui->orgDepTableWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(on_orgDepSlider_changed(int)));
    connect((QWidget*)ui->boardingTableWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(on_boardingSlider_changed(int)));
    connect((QWidget*)ui->notboardingTableWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(on_notBoardingSlider_changed(int)));
}

void FlightEnquires::statistics(QString flight)
{
    this->flight = flight;
    this->isStatisticsMode = true;

    ui->inputWidget->hide();
    this->on_orgDepPushButton_clicked();

    this->query();
}

void FlightEnquires::fillOrgDepWithMQ(const QJsonArray &msg)
{
    ui->orgDepTableWidget->scrollToTop();

    while (ui->orgDepTableWidget->rowCount() > 0 ) {
        ui->orgDepTableWidget->removeRow(0);
    }

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;

    for (int i = 0; i < msg.size(); i++) {
        if (0 != QString::compare(msg.at(i).toObject().value("boardingGate").toString()
                                  , LocalSettings::config->value("Device/boardingGate").toString())) {
            continue;
        }

        QJsonArray content = msg.at(i).toObject().value("content").toArray();
        for (int j = 0; j < content.size(); j++) {
            QString updateTime = content.at(j).toObject().value("updateTime").toString();
            QString boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
            QString flightNumber = content.at(j).toObject().value("flightNumber").toString();

            if (0 != QString::compare(flightNumber, "AB1250")) {
                continue;
            }

            if (response.interface.validSize == 0) {
                response.interface.results[0].boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                response.interface.results[0].boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                response.interface.results[0].cardNo = content.at(j).toObject().value("cardNo").toString();
                response.interface.results[0].flightNumber = content.at(j).toObject().value("flightNumber").toString();
                response.interface.results[0].id = content.at(j).toObject().value("id").toString();
                response.interface.results[0].passengerName = content.at(j).toObject().value("passengerName").toString();
                response.interface.results[0].photoPath = content.at(j).toObject().value("photoPath").toString();
                response.interface.results[0].repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                response.interface.results[0].seatNumber = content.at(j).toObject().value("seatNumber").toString();
                response.interface.results[0].updateTime = content.at(j).toObject().value("updateTime").toString();
                response.interface.results[0].boardingTime = content.at(j).toObject().value("boardingTime").toString();
                response.interface.results[0].isSameBoardingNumber = false;

                response.interface.validSize += 1;
            } else {
                bool isSameboardingNumber = false;
                int sameBoardingNumber = 0;
                for (int s = 0; s < response.interface.validSize; s++) {
                    if (boardingNumber == response.interface.results[s].boardingNumber) {
                        isSameboardingNumber = true;
                        sameBoardingNumber = s;
                        break;
                    }
                }

                if (isSameboardingNumber) {
                    QString tempBoardingNumber = response.interface.results[sameBoardingNumber].boardingNumber;
                    int tempBoardingStatus = response.interface.results[sameBoardingNumber].boardingStatus;
                    QString tempCardNo = response.interface.results[sameBoardingNumber].cardNo;
                    QString tempFlightNumber = response.interface.results[sameBoardingNumber].flightNumber;
                    QString tempId = response.interface.results[sameBoardingNumber].id;
                    QString tempPassengerName = response.interface.results[sameBoardingNumber].passengerName;
                    QString tempPhotoPath = response.interface.results[sameBoardingNumber].photoPath;
                    int tempRepeatFlag = response.interface.results[sameBoardingNumber].repeatFlag;
                    QString tempSeatNumber = response.interface.results[sameBoardingNumber].seatNumber;
                    QString tempUpdateTime = response.interface.results[sameBoardingNumber].updateTime;
                    QString tempBoardingTime = response.interface.results[sameBoardingNumber].boardingTime;

                    for (int k = response.interface.validSize; k > 0; k--) {
                        if (k > sameBoardingNumber + 1) {
                            response.interface.results[k].boardingNumber = response.interface.results[k - 1].boardingNumber;
                            response.interface.results[k].boardingStatus = response.interface.results[k - 1].boardingStatus;
                            response.interface.results[k].cardNo = response.interface.results[k - 1].cardNo;
                            response.interface.results[k].flightNumber = response.interface.results[k - 1].flightNumber;
                            response.interface.results[k].id = response.interface.results[k - 1].id;
                            response.interface.results[k].passengerName = response.interface.results[k - 1].passengerName;
                            response.interface.results[k].photoPath = response.interface.results[k - 1].photoPath;
                            response.interface.results[k].repeatFlag = response.interface.results[k - 1].repeatFlag;
                            response.interface.results[k].seatNumber = response.interface.results[k - 1].seatNumber;
                            response.interface.results[k].updateTime = response.interface.results[k - 1].updateTime;
                            response.interface.results[k].boardingTime = response.interface.results[k - 1].boardingTime;
                            response.interface.results[k].isSameBoardingNumber = response.interface.results[k - 1].isSameBoardingNumber;
                        } else if (k >= 2) {
                            response.interface.results[k].boardingNumber = response.interface.results[k - 2].boardingNumber;
                            response.interface.results[k].boardingStatus = response.interface.results[k - 2].boardingStatus;
                            response.interface.results[k].cardNo = response.interface.results[k - 2].cardNo;
                            response.interface.results[k].flightNumber = response.interface.results[k - 2].flightNumber;
                            response.interface.results[k].id = response.interface.results[k - 2].id;
                            response.interface.results[k].passengerName = response.interface.results[k - 2].passengerName;
                            response.interface.results[k].photoPath = response.interface.results[k - 2].photoPath;
                            response.interface.results[k].repeatFlag = response.interface.results[k - 2].repeatFlag;
                            response.interface.results[k].seatNumber = response.interface.results[k - 2].seatNumber;
                            response.interface.results[k].updateTime = response.interface.results[k - 2].updateTime;
                            response.interface.results[k].boardingTime = response.interface.results[k - 1].boardingTime;
                            response.interface.results[k].isSameBoardingNumber = response.interface.results[k - 2].isSameBoardingNumber;
                        }
                    }

                    response.interface.results[1].boardingNumber = tempBoardingNumber;
                    response.interface.results[1].boardingStatus = tempBoardingStatus;
                    response.interface.results[1].cardNo = tempCardNo;
                    response.interface.results[1].flightNumber = tempFlightNumber;
                    response.interface.results[1].id = tempId;
                    response.interface.results[1].passengerName = tempPassengerName;
                    response.interface.results[1].photoPath = tempPhotoPath;
                    response.interface.results[1].repeatFlag = tempRepeatFlag;
                    response.interface.results[1].seatNumber = tempSeatNumber;
                    response.interface.results[1].updateTime = tempUpdateTime;
                    response.interface.results[1].boardingTime = tempBoardingTime;
                    response.interface.results[1].isSameBoardingNumber = true;

                    response.interface.results[0].boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                    response.interface.results[0].boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                    response.interface.results[0].cardNo = content.at(j).toObject().value("cardNo").toString();
                    response.interface.results[0].flightNumber = content.at(j).toObject().value("flightNumber").toString();
                    response.interface.results[0].id = content.at(j).toObject().value("id").toString();
                    response.interface.results[0].passengerName = content.at(j).toObject().value("passengerName").toString();
                    response.interface.results[0].photoPath = content.at(j).toObject().value("photoPath").toString();
                    response.interface.results[0].repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                    response.interface.results[0].seatNumber = content.at(j).toObject().value("seatNumber").toString();
                    response.interface.results[0].updateTime = content.at(j).toObject().value("updateTime").toString();
                    response.interface.results[0].boardingTime = content.at(j).toObject().value("boardingTime").toString();
                    response.interface.results[0].isSameBoardingNumber = true;

                    response.interface.validSize += 1;
                } else {
                    for (int m = 0; m < response.interface.validSize; m++) {
                        if (updateTime > response.interface.results[m].updateTime && !response.interface.results[m].isSameBoardingNumber) {
                            for (int n = response.interface.validSize - 1; n >= m; n--) {
                                response.interface.results[n + 1].boardingNumber = response.interface.results[n].boardingNumber;
                                response.interface.results[n + 1].boardingStatus = response.interface.results[n].boardingStatus;
                                response.interface.results[n + 1].cardNo = response.interface.results[n].cardNo;
                                response.interface.results[n + 1].flightNumber = response.interface.results[n].flightNumber;
                                response.interface.results[n + 1].id = response.interface.results[n].id;
                                response.interface.results[n + 1].passengerName = response.interface.results[n].passengerName;
                                response.interface.results[n + 1].photoPath = response.interface.results[n].photoPath;
                                response.interface.results[n + 1].repeatFlag = response.interface.results[n].repeatFlag;
                                response.interface.results[n + 1].seatNumber = response.interface.results[n].seatNumber;
                                response.interface.results[n + 1].updateTime = response.interface.results[n].updateTime;
                                response.interface.results[n + 1].boardingTime = response.interface.results[n].boardingTime;
                                response.interface.results[n + 1].isSameBoardingNumber = response.interface.results[n].isSameBoardingNumber;
                            }
                            response.interface.results[m].boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                            response.interface.results[m].boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                            response.interface.results[m].cardNo = content.at(j).toObject().value("cardNo").toString();
                            response.interface.results[m].flightNumber = content.at(j).toObject().value("flightNumber").toString();
                            response.interface.results[m].id = content.at(j).toObject().value("id").toString();
                            response.interface.results[m].passengerName = content.at(j).toObject().value("passengerName").toString();
                            response.interface.results[m].photoPath = content.at(j).toObject().value("photoPath").toString();
                            response.interface.results[m].repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                            response.interface.results[m].seatNumber = content.at(j).toObject().value("seatNumber").toString();
                            response.interface.results[m].updateTime = content.at(j).toObject().value("updateTime").toString();
                            response.interface.results[m].boardingTime = content.at(j).toObject().value("boardingTime").toString();
                            response.interface.results[m].isSameBoardingNumber = false;
                            response.interface.validSize += 1;

                            break;
                        }
                        if (m == response.interface.validSize - 1) {
                            response.interface.results[response.interface.validSize].boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                            response.interface.results[response.interface.validSize].boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                            response.interface.results[response.interface.validSize].cardNo = content.at(j).toObject().value("cardNo").toString();
                            response.interface.results[response.interface.validSize].flightNumber = content.at(j).toObject().value("flightNumber").toString();
                            response.interface.results[response.interface.validSize].id = content.at(j).toObject().value("id").toString();
                            response.interface.results[response.interface.validSize].passengerName = content.at(j).toObject().value("passengerName").toString();
                            response.interface.results[response.interface.validSize].photoPath = content.at(j).toObject().value("photoPath").toString();
                            response.interface.results[response.interface.validSize].repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                            response.interface.results[response.interface.validSize].seatNumber = content.at(j).toObject().value("seatNumber").toString();
                            response.interface.results[response.interface.validSize].updateTime = content.at(j).toObject().value("updateTime").toString();
                            response.interface.results[response.interface.validSize].boardingTime = content.at(j).toObject().value("boardingTime").toString();
                            response.interface.results[response.interface.validSize].isSameBoardingNumber = false;
                            response.interface.validSize += 1;

                            break;
                        }
                    }
                }
            }
        }
    }

    ui->orgDepPushButton->setText("建库人数：" + QString::number(response.interface.validSize));
    fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
}

FlightEnquires::~FlightEnquires()
{
    delete ui;
}

void FlightEnquires::on_queryPushButton_clicked()
{
    ui->orgDepTableWidget->scrollToTop();

    while (ui->orgDepTableWidget->rowCount() > 0 ) {
        ui->orgDepTableWidget->removeRow(0);
    }

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;

    flight = ui->queryLineEdit->text().toUpper();

    if (flight.length() == 0) {
        MessageDialog msg(this, nullptr, "请输入有效信息!", 1);
        msg.exec();

        return;
    }

    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");

    ui->orgDepTableWidget->show();
    ui->boardingTableWidget->hide();
    ui->notboardingTableWidget->hide();

    if (Ui::DisplayType::DisplayNullErr == query()) {
         MessageDialog msg(this, nullptr, "请输入正确的航班号!", 1);
         msg.exec();
     }
}

void FlightEnquires::on_orgDepPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");

    ui->orgDepTableWidget->show();
    ui->boardingTableWidget->hide();
    ui->notboardingTableWidget->hide();
}

void FlightEnquires::on_boardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");

    ui->orgDepTableWidget->hide();
    ui->boardingTableWidget->show();
    ui->notboardingTableWidget->hide();
}

void FlightEnquires::on_notboardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");

    ui->orgDepTableWidget->hide();
    ui->boardingTableWidget->hide();
    ui->notboardingTableWidget->show();
}

void FlightEnquires::on_orgDepSlider_changed(int p)
{
    if (p > orgDepFilledNum * 2 - 6) {
        fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
    }
}

void FlightEnquires::on_boardingSlider_changed(int p)
{
    if (p > boardingFilledNum * 2 - 6) {
        fillTableGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
    }
}

void FlightEnquires::on_notBoardingSlider_changed(int p)
{
    if (p > notboardingFilledNum * 2 - 6) {
        fillTableGradually(response, ui->notboardingTableWidget, Ui::DisplayTab::NotBoardingTab);
    }
}

void FlightEnquires::on_removeRow_clicked(int widgetIndex)
{
    LibDeleteRequest libDelReq;
    LibDeleteResponse libDelRsp;
    QString uuid = QUuid::createUuid().toString();
    uuid.remove("{").remove("}").remove("-");
    libDelReq.reqId = uuid;
    libDelReq.flightNo = response.interface.results[widgetIndex/2].flightNumber;
    libDelReq.gateNo = LocalSettings::config->value("Device/gateNo").toString();
    libDelReq.boardingGate = LocalSettings::config->value("Device/boardingGate").toString();
    libDelReq.deviceCode = LocalSettings::config->value("Device/deviceId").toString();
    libDelReq.id = response.interface.results[widgetIndex/2].id;

    for (int i = widgetIndex/2; i < response.interface.validSize && i < 1000; i++) {
        response.interface.results[i] = response.interface.results[i + 1];
    }
    response.interface.validSize -= 1;

    ui->orgDepTableWidget->scrollToTop();
    while (ui->orgDepTableWidget->rowCount() > 0 ) {
        ui->orgDepTableWidget->removeRow(0);
    }

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;

    ui->orgDepPushButton->setText("建库人数：" + QString::number(response.interface.validSize));
    fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);

    libDelRsp = HttpAPI::instance()->removeSpecific(libDelReq);
}

QPixmap FlightEnquires::getQPixmapSync(QString str)
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

void FlightEnquires::fillTableGradually(const FlightReviewResponse &response, QTableWidget *table, Ui::DisplayTab tab)
{
    FlightReviewResult results[1000];

    for (int i = 0; i < response.interface.validSize; i++) {
        if (tab == Ui::DisplayTab::DepositoryTab) {
            results[i] = response.interface.results[i];
        }
        if (tab == Ui::DisplayTab::BoardingTab) {
            results[i] = response.interface.boarded[i];
        }
        if (tab == Ui::DisplayTab::NotBoardingTab) {
            results[i] = response.interface.unboard[i];
        }
    }

    int widgetIndex = 0;
    int resultIndex = 0;

    if (tab == Ui::DisplayTab::DepositoryTab) {
        widgetIndex = orgDepFilledNum * 2;
        resultIndex = orgDepFillIndex;
    }
    if (tab == Ui::DisplayTab::BoardingTab) {
        widgetIndex = boardingFilledNum * 2;
        resultIndex = boardingFillIndex;
    }
    if (tab == Ui::DisplayTab::NotBoardingTab) {
        widgetIndex = notboardingFilledNum * 2;
        resultIndex = notboardingFillIndex;
    }

    int batch = 0;
    for (int i = resultIndex; (i < response.interface.validSize) && (batch < 3); i++) {
        if (tab == Ui::DisplayTab::DepositoryTab) {
            orgDepFillIndex += 1;

            if (results[i].id == "") {
                continue;
            }
        }

        if (tab == Ui::DisplayTab::BoardingTab) {
            boardingFillIndex += 1;

            if (results[i].boardingStatus != 1) {
                continue;
            }
        }

        if (tab == Ui::DisplayTab::NotBoardingTab) {
            notboardingFillIndex += 1;

            if (!(results[i].id != ""
                  && results[i].boardingStatus == 0)) {
                continue;
            }
        }

        table->setRowHeight(widgetIndex, 186);
        table->insertRow(widgetIndex);
        table->setRowHeight(widgetIndex, 186);

        QWidget *itemWidget = new ButtonWidget(this, isStatisticsMode, tab, widgetIndex);

        if (results[i].isSameBoardingNumber) {
            QImage sameBoardingNumberBGImage;
            sameBoardingNumberBGImage.load(":/6航班回查/Images/6航班回查/矩形-4365.png");
            sameBoardingNumberBGImage = sameBoardingNumberBGImage.scaled(766
                                                                         , 185
                                                                         , Qt::IgnoreAspectRatio
                                                                         , Qt::SmoothTransformation);
            QPixmap sameBoardingNumberBGPixmap = QPixmap::fromImage(sameBoardingNumberBGImage);
            QLabel *sameBoardingNumberBGLabel = new QLabel(itemWidget);
            sameBoardingNumberBGLabel->setGeometry(0, 0, 766, 185);
            sameBoardingNumberBGLabel->setPixmap(sameBoardingNumberBGPixmap);
            sameBoardingNumberBGLabel->setFixedSize(766, 185);
            sameBoardingNumberBGLabel->setAlignment(Qt::AlignCenter);
        }

        QPixmap pixmap = getQPixmapSync(results[i].photoPath);
        pixmap = pixmap.scaled(131
                               , 186
                               , Qt::IgnoreAspectRatio
                               , Qt::SmoothTransformation);
        QLabel *photoLabel = new QLabel(itemWidget);
        photoLabel->setGeometry(30, 0, 131, 186);
        photoLabel->setPixmap(pixmap);
        photoLabel->setFixedSize(131, 186);
        photoLabel->setAlignment(Qt::AlignCenter);

        QLabel *nameTitleLabel = new QLabel(itemWidget);
        nameTitleLabel->setGeometry(161, 14, 145, 38);
        nameTitleLabel->setText("旅客姓名：");
        nameTitleLabel->setFixedSize(145, 38);
        nameTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        nameTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *nameLabel = new QLabel(itemWidget);
        nameLabel->setGeometry(306, 14, 300, 38);
        nameLabel->setText(results[i].passengerName);
        nameLabel->setFixedSize(300, 38);
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nameLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *flightTitleLabel = new QLabel(itemWidget);
        flightTitleLabel->setGeometry(161, 74, 195, 38);
        flightTitleLabel->setText("航班号／序号：");
        flightTitleLabel->setFixedSize(195, 38);
        flightTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        flightTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *flightLabel = new QLabel(itemWidget);
        flightLabel->setGeometry(356, 74, 300, 38);
        flightLabel->setText(results[i].flightNumber + "/" + results[i].boardingNumber);
        flightLabel->setFixedSize(300, 38);
        flightLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        flightLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *seatTitleLabel = new QLabel(itemWidget);
        seatTitleLabel->setGeometry(161, 134, 117, 38);
        seatTitleLabel->setText("座位号：");
        seatTitleLabel->setFixedSize(117, 38);
        seatTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        seatTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *seatLabel = new QLabel(itemWidget);
        seatLabel->setGeometry(278, 134, 300, 38);
        seatLabel->setText(results[i].seatNumber);
        seatLabel->setFixedSize(300, 38);
        seatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        seatLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        table->setCellWidget(widgetIndex, 0, itemWidget);
        batch += 1;
        widgetIndex += 1;

        if (tab == Ui::DisplayTab::DepositoryTab) {
            orgDepFilledNum += 1;
        }
        if (tab == Ui::DisplayTab::BoardingTab) {
            boardingFilledNum += 1;
        }
        if (tab == Ui::DisplayTab::NotBoardingTab) {
            notboardingFilledNum += 1;
        }

        table->setRowHeight(widgetIndex, 2);
        table->insertRow(widgetIndex);
        table->setRowHeight(widgetIndex, 2);

        QWidget *splitWidget = new QWidget();
        QLabel *splitLabel = new QLabel(splitWidget);
        splitLabel->setGeometry(13, 0, 740, 2);
        splitLabel->setStyleSheet("image: 0; background: 0; border-radius: 0; border-width: 1px; border-style: dashed; border-color: rgb(135, 183, 194);");

        table->setCellWidget(widgetIndex, 0, splitWidget);
        widgetIndex = widgetIndex + 1;
    }
}

int FlightEnquires::query()
{
    FlightReviewRequest request;
    request.input = flight;
    // queryType写死为3,以后0/1/2都不用了
    request.queryType = 3;

    response = HttpAPI::instance()->get(request);

//    for (int i = 0; i < response.interface.validSize; i++) {
//        qDebug() << "boardingStatus: " << response.interface.boarded[i].boardingStatus << "\t\tboardingTime" << response.interface.boarded[i].boardingTime;
//    }

    ui->orgDepPushButton->setText("建库人数：" + QString::number(response.interface.dataInfo.faceNums));
    ui->boardingPushButton->setText("已登机人数：" + QString::number(response.interface.dataInfo.boardingNum));
    ui->notboardingPushButton->setText("未登机人数：" + QString::number(response.interface.total - response.interface.dataInfo.boardingNum));

    if (!response.founded) {
        qDebug() << "Queried Flight Not Found!!";

        ui->orgDepTableWidget->clear();
        ui->boardingTableWidget->clear();
        ui->notboardingTableWidget->clear();

        return Ui::DisplayType::DisplayNullErr;
    }

    fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
    fillTableGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
    fillTableGradually(response, ui->notboardingTableWidget, Ui::DisplayTab::NotBoardingTab);

    return Ui::DisplayType::DisplayNormal;
}
