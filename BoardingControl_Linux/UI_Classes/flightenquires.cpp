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

ButtonWidget::ButtonWidget(QWidget *parent, int widgetIndex)
    : QWidget(parent),
      widgetIndex(widgetIndex)
{
    QSignalMapper *signalMapper = new QSignalMapper();

    QPushButton *removePushButton = new QPushButton(this);
    removePushButton->setGeometry(600, 134, 140, 40);
    removePushButton->setText("删    除");
    removePushButton->setFixedSize(140, 40);
    removePushButton->setStyleSheet("image: 0; border: 0; border-radius: 4; background: rgb(255, 0, 0); font: 19pt; color: rgb(255, 255, 255);");

    connect(removePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(removePushButton, widgetIndex);

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
    naManager = new QNetworkAccessManager(this);

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
    this->flight = flight.toUpper();
    this->isStatisticsMode = true;

    ui->inputWidget->hide();

    ui->orgDepTableWidget->clear();
    ui->orgDepTableWidget->scrollToTop();
    while (ui->orgDepTableWidget->rowCount() > 0) {
        ui->orgDepTableWidget->removeRow(0);
    }
    ui->boardingTableWidget->clear();
    ui->boardingTableWidget->scrollToTop();
    while (ui->boardingTableWidget->rowCount() > 0) {
        ui->boardingTableWidget->removeRow(0);
    }
    ui->notboardingTableWidget->clear();
    ui->notboardingTableWidget->scrollToTop();
    while (ui->notboardingTableWidget->rowCount() > 0) {
        ui->notboardingTableWidget->removeRow(0);
    }

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;
    boardingFilledNum = 0;
    boardingFillIndex = 0;
    notboardingFilledNum = 0;
    notboardingFillIndex = 0;

    if (flight.length() == 0) {
        qDebug() << "flight: " << flight;
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

bool FlightEnquires::isAlreadyExist(const QJsonValue &json)
{
    bool exist = false;

    for (int i = 0; i < response.interface.validSize; i++) {
        if (0 == QString::compare(response.interface.results[i].id, json.toObject().value("id").toString())) {
            response.interface.results[i].boardingNumber = json.toObject().value("boardingNumber").toString();
            response.interface.results[i].boardingStatus = json.toObject().value("boardingStatus").toInt();
            response.interface.results[i].cardNo = json.toObject().value("cardNo").toString();
            response.interface.results[i].flightNumber = json.toObject().value("flightNumber").toString();
            response.interface.results[i].id = json.toObject().value("id").toString();
            response.interface.results[i].passengerName = json.toObject().value("passengerName").toString();
            response.interface.results[i].photoPath = json.toObject().value("photoPath").toString();
            response.interface.results[i].repeatFlag = json.toObject().value("repeatFlag").toInt();
            response.interface.results[i].seatNumber = json.toObject().value("seatNumber").toString();
            response.interface.results[i].updateTime = json.toObject().value("updateTime").toString();
            response.interface.results[i].boardingTime = json.toObject().value("boardingTime").toString();

            exist = true;
        }

        if (0 == QString::compare(response.interface.unboard[i].id, json.toObject().value("id").toString())) {
            response.interface.unboard[i].boardingNumber = json.toObject().value("boardingNumber").toString();
            response.interface.unboard[i].boardingStatus = json.toObject().value("boardingStatus").toInt();
            response.interface.unboard[i].cardNo = json.toObject().value("cardNo").toString();
            response.interface.unboard[i].flightNumber = json.toObject().value("flightNumber").toString();
            response.interface.unboard[i].id = json.toObject().value("id").toString();
            response.interface.unboard[i].passengerName = json.toObject().value("passengerName").toString();
            response.interface.unboard[i].photoPath = json.toObject().value("photoPath").toString();
            response.interface.unboard[i].repeatFlag = json.toObject().value("repeatFlag").toInt();
            response.interface.unboard[i].seatNumber = json.toObject().value("seatNumber").toString();
            response.interface.unboard[i].updateTime = json.toObject().value("updateTime").toString();
            response.interface.unboard[i].boardingTime = json.toObject().value("boardingTime").toString();

            exist = true;
        }
    }

    return exist;
}

void FlightEnquires::fillDepAndUnboardWithMQ(const QJsonArray &msg)
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

            if (0 != QString::compare(flightNumber, flight)) {
                continue;
            }

            if (isAlreadyExist(content.at(j))) {
                continue;
            }

            // fill OrgDepository
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
                            break;
                        }
                    }
                }
            }

            // fill Unboard
            QString seatNumber = content.at(j).toObject().value("seatNumber").toString();
            if (response.interface.validSize == 0) {
                response.interface.unboard[0].boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                response.interface.unboard[0].boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                response.interface.unboard[0].cardNo = content.at(j).toObject().value("cardNo").toString();
                response.interface.unboard[0].flightNumber = content.at(j).toObject().value("flightNumber").toString();
                response.interface.unboard[0].id = content.at(j).toObject().value("id").toString();
                response.interface.unboard[0].passengerName = content.at(j).toObject().value("passengerName").toString();
                response.interface.unboard[0].photoPath = content.at(j).toObject().value("photoPath").toString();
                response.interface.unboard[0].repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                response.interface.unboard[0].seatNumber = content.at(j).toObject().value("seatNumber").toString();
                response.interface.unboard[0].updateTime = content.at(j).toObject().value("updateTime").toString();
                response.interface.unboard[0].boardingTime = content.at(j).toObject().value("boardingTime").toString();
                response.interface.unboard[0].isSameBoardingNumber = false;
            } else {
                for (int m = 0; m < response.interface.validSize; m++) {
                    if (seatNumber < response.interface.unboard[m].seatNumber) {
                        for (int k = response.interface.validSize - 1; k >= m; k--) {
                            response.interface.unboard[k + 1].boardingNumber = response.interface.unboard[k].boardingNumber;
                            response.interface.unboard[k + 1].boardingStatus = response.interface.unboard[k].boardingStatus;
                            response.interface.unboard[k + 1].cardNo = response.interface.unboard[k].cardNo;
                            response.interface.unboard[k + 1].flightNumber = response.interface.unboard[k].flightNumber;
                            response.interface.unboard[k + 1].id = response.interface.unboard[k].id;
                            response.interface.unboard[k + 1].passengerName = response.interface.unboard[k].passengerName;
                            response.interface.unboard[k + 1].photoPath = response.interface.unboard[k].photoPath;
                            response.interface.unboard[k + 1].repeatFlag = response.interface.unboard[k].repeatFlag;
                            response.interface.unboard[k + 1].seatNumber = response.interface.unboard[k].seatNumber;
                            response.interface.unboard[k + 1].updateTime = response.interface.unboard[k].updateTime;
                            response.interface.unboard[k + 1].boardingTime = response.interface.unboard[k].boardingTime;
                            response.interface.unboard[k + 1].isSameBoardingNumber = response.interface.unboard[k].isSameBoardingNumber;
                        }
                        response.interface.unboard[m].boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                        response.interface.unboard[m].boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                        response.interface.unboard[m].cardNo = content.at(j).toObject().value("cardNo").toString();
                        response.interface.unboard[m].flightNumber = content.at(j).toObject().value("flightNumber").toString();
                        response.interface.unboard[m].id = content.at(j).toObject().value("id").toString();
                        response.interface.unboard[m].passengerName = content.at(j).toObject().value("passengerName").toString();
                        response.interface.unboard[m].photoPath = content.at(j).toObject().value("photoPath").toString();
                        response.interface.unboard[m].repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                        response.interface.unboard[m].seatNumber = content.at(j).toObject().value("seatNumber").toString();
                        response.interface.unboard[m].updateTime = content.at(j).toObject().value("updateTime").toString();
                        response.interface.unboard[m].boardingTime = content.at(j).toObject().value("boardingTime").toString();
                        response.interface.unboard[m].isSameBoardingNumber = false;
                        break;
                    }
                    if (m == response.interface.validSize - 1) {
                        response.interface.unboard[response.interface.validSize].boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                        response.interface.unboard[response.interface.validSize].boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                        response.interface.unboard[response.interface.validSize].cardNo = content.at(j).toObject().value("cardNo").toString();
                        response.interface.unboard[response.interface.validSize].flightNumber = content.at(j).toObject().value("flightNumber").toString();
                        response.interface.unboard[response.interface.validSize].id = content.at(j).toObject().value("id").toString();
                        response.interface.unboard[response.interface.validSize].passengerName = content.at(j).toObject().value("passengerName").toString();
                        response.interface.unboard[response.interface.validSize].photoPath = content.at(j).toObject().value("photoPath").toString();
                        response.interface.unboard[response.interface.validSize].repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                        response.interface.unboard[response.interface.validSize].seatNumber = content.at(j).toObject().value("seatNumber").toString();
                        response.interface.unboard[response.interface.validSize].updateTime = content.at(j).toObject().value("updateTime").toString();
                        response.interface.unboard[response.interface.validSize].boardingTime = content.at(j).toObject().value("boardingTime").toString();
                        response.interface.unboard[response.interface.validSize].isSameBoardingNumber = false;
                    }
                }
            }
            response.interface.validSize += 1;
            response.interface.dataInfo.faceNums += 1;
        }
    }

    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");

    int unBoardingNum = 0;
    for (int i = 0; i < response.interface.validSize && i < 1000; i++) {
        if ((response.interface.results[i].boardingStatus == 0) && (response.interface.results[i].id != "")) {
            unBoardingNum += 1;
        }
    }
    ui->notboardingPushButton->setText("未登机人数：" + QString::number(unBoardingNum));
    ui->orgDepPushButton->setText("航班人数：" + QString::number(response.interface.dataInfo.faceNums));

    ui->boardingTableWidget->hide();
    ui->notboardingTableWidget->hide();
    ui->orgDepTableWidget->show();

    fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
}

QString FlightEnquires::findPhotoPath(QString cardNumber
                                      , QString flightNumber
                                      , QString boardingNumber
                                      , QString seatNumber)
{
    if (!cardNumber.isEmpty()) {
        int count = 0;
        int found = 0;
        for (int i = 0; i < response.interface.validSize; i++) {
            if (0 == QString::compare(cardNumber, response.interface.results[i].cardNo)) {
                count += 1;
                found = i;
            }
        }

        if (count == 1) {
            return response.interface.results[found].photoPath;
        } else {
            for (int i = 0; i < response.interface.validSize; i++) {
                if ((0 == QString::compare(flightNumber, response.interface.results[i].flightNumber))
                        &&(0 == QString::compare(boardingNumber, response.interface.results[i].boardingNumber))
                        &&(0 == QString::compare(seatNumber, response.interface.results[i].seatNumber))) {
                    return response.interface.results[i].photoPath;
                }
            }
        }
    } else {
        for (int i = 0; i < response.interface.validSize; i++) {
            if ((0 == QString::compare(flightNumber, response.interface.results[i].flightNumber))
                    &&(0 == QString::compare(boardingNumber, response.interface.results[i].boardingNumber))
                    &&(0 == QString::compare(seatNumber, response.interface.results[i].seatNumber))) {
                return response.interface.results[i].photoPath;
            }
        }
    }

    return QString();
}

void FlightEnquires::fillBoardAndUnboardWithControl(QString flightNo
                                                  , QString flightDay
                                                  , int recheckType
                                                  , const QJsonObject &userInfo
                                                  , const QJsonObject &manualInfo)
{
    Q_UNUSED(flightNo)
    Q_UNUSED(flightDay)

    ui->boardingTableWidget->scrollToTop();

    while (ui->boardingTableWidget->rowCount() > 0 ) {
        ui->boardingTableWidget->removeRow(0);
    }

    boardingFilledNum = 0;
    boardingFillIndex = 0;

    QString flightNumber = "";
    QString boardingNumber = "";
    QString seatNumber = "";
    QString photoPath = "";

    if ((recheckType == 0) || (recheckType == 2)) {
        flightNumber = userInfo.value("flightNumber").toString();
        boardingNumber = userInfo.value("boardingNumber").toString();
        seatNumber = userInfo.value("seatNumber").toString();
        photoPath = findPhotoPath(userInfo.value("cardNo").toString(), flightNumber, boardingNumber, seatNumber);
    } else if ((recheckType == 1) || (recheckType == 3)) {
        flightNumber = manualInfo.value("flightNo").toString();
        boardingNumber = manualInfo.value("boardingNumber").toString();
        seatNumber = manualInfo.value("seatId").toString();
        photoPath = findPhotoPath(manualInfo.value("cardId").toString(), flightNumber, boardingNumber, seatNumber);
    } else {
        qDebug() << "recheckType error: " << recheckType;
        return;
    }

    // fill boarded
    bool isRepeated = false;
    for (int i = 0; i < response.interface.validSize; i++) {
        if ((response.interface.boarded[i].flightNumber == flightNumber)
                && (response.interface.boarded[i].boardingNumber == boardingNumber)
                && (response.interface.boarded[i].seatNumber == seatNumber)) {
            isRepeated = true;

            QString tempBoardingNumber = response.interface.boarded[i].boardingNumber;
            int tempBoardingStatus = 1;
            QString tempCardNo = "";
            QString tempPassengerName = "";
            QString tempBasePhoto = "";
            if ((recheckType == 0) || (recheckType == 2)) {
                tempCardNo = userInfo.value("cardNo").toString();
                tempPassengerName = userInfo.value("nameZh").toString();
                tempBasePhoto = userInfo.value("basePhoto").toString();
            }
            if ((recheckType == 1) || (recheckType == 3)) {
                tempCardNo = manualInfo.value("cardId").toString();
                tempPassengerName = manualInfo.value("passengerName").toString();
                tempBasePhoto = manualInfo.value("scenePhoto").toString();
            }
            QString tempFlightNumber = response.interface.boarded[i].flightNumber;
            QString tempId = "";
            QString tempPhotoPath = photoPath;
            int tempRepeatFlag = 0;
            QString tempSeatNumber = response.interface.boarded[i].seatNumber;
            QString tempUpdateTime = "";
            QString tempBoardingTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
            bool tempIsSameBoardingNumber = false;

            for (int j = i; j > 0; j--) {
                response.interface.boarded[j].boardingNumber = response.interface.boarded[j - 1].boardingNumber;
                response.interface.boarded[j].boardingStatus = response.interface.boarded[j - 1].boardingStatus;
                response.interface.boarded[j].cardNo = response.interface.boarded[j - 1].cardNo;
                response.interface.boarded[j].flightNumber = response.interface.boarded[j - 1].flightNumber;
                response.interface.boarded[j].id = response.interface.boarded[j - 1].id;
                response.interface.boarded[j].passengerName = response.interface.boarded[j - 1].passengerName;
                response.interface.boarded[j].photoPath = response.interface.boarded[j - 1].photoPath;
                response.interface.boarded[j].basePhoto = response.interface.boarded[j - 1].basePhoto;
                response.interface.boarded[j].repeatFlag = response.interface.boarded[j - 1].repeatFlag;
                response.interface.boarded[j].seatNumber = response.interface.boarded[j - 1].seatNumber;
                response.interface.boarded[j].updateTime = response.interface.boarded[j - 1].updateTime;
                response.interface.boarded[j].boardingTime = response.interface.boarded[j - 1].boardingTime;
                response.interface.boarded[j].isSameBoardingNumber = response.interface.boarded[j - 1].isSameBoardingNumber;
            }

            response.interface.boarded[0].boardingNumber = tempBoardingNumber;
            response.interface.boarded[0].boardingStatus = tempBoardingStatus;
            response.interface.boarded[0].cardNo = tempCardNo;
            response.interface.boarded[0].flightNumber = tempFlightNumber;
            response.interface.boarded[0].id = tempId;
            response.interface.boarded[0].passengerName = tempPassengerName;
            response.interface.boarded[0].photoPath = tempPhotoPath;
            response.interface.boarded[0].basePhoto = tempBasePhoto;
            response.interface.boarded[0].repeatFlag = tempRepeatFlag;
            response.interface.boarded[0].seatNumber = tempSeatNumber;
            response.interface.boarded[0].updateTime = tempUpdateTime;
            response.interface.boarded[0].boardingTime = tempBoardingTime;
            response.interface.boarded[0].isSameBoardingNumber = tempIsSameBoardingNumber;
        }
    }

    if (!isRepeated) {
        for (int i = response.interface.validSize; i > 0; i--) {
            response.interface.boarded[i].boardingNumber = response.interface.boarded[i - 1].boardingNumber;
            response.interface.boarded[i].boardingStatus = response.interface.boarded[i - 1].boardingStatus;
            response.interface.boarded[i].cardNo = response.interface.boarded[i - 1].cardNo;
            response.interface.boarded[i].flightNumber = response.interface.boarded[i - 1].flightNumber;
            response.interface.boarded[i].id = response.interface.boarded[i - 1].id;
            response.interface.boarded[i].passengerName = response.interface.boarded[i - 1].passengerName;
            response.interface.boarded[i].photoPath = response.interface.boarded[i - 1].photoPath;
            response.interface.boarded[i].basePhoto = response.interface.boarded[i - 1].basePhoto;
            response.interface.boarded[i].repeatFlag = response.interface.boarded[i - 1].repeatFlag;
            response.interface.boarded[i].seatNumber = response.interface.boarded[i - 1].seatNumber;
            response.interface.boarded[i].updateTime = response.interface.boarded[i - 1].updateTime;
            response.interface.boarded[i].boardingTime = response.interface.boarded[i - 1].boardingTime;
            response.interface.boarded[i].isSameBoardingNumber = response.interface.boarded[i - 1].isSameBoardingNumber;
        }

        if ((recheckType == 0) || (recheckType == 2)) {
            response.interface.boarded[0].boardingNumber = userInfo.value("boardingNumber").toString();
            response.interface.boarded[0].cardNo = userInfo.value("cardNo").toString();
            response.interface.boarded[0].flightNumber = userInfo.value("flightNumber").toString();
            response.interface.boarded[0].passengerName = userInfo.value("nameZh").toString();
            response.interface.boarded[0].seatNumber = userInfo.value("seatNumber").toString();
            response.interface.boarded[0].basePhoto = userInfo.value("basePhoto").toString();
        }
        if ((recheckType == 1) || (recheckType == 3)) {
            response.interface.boarded[0].boardingNumber = manualInfo.value("boardingNumber").toString();
            response.interface.boarded[0].cardNo = manualInfo.value("cardId").toString();
            response.interface.boarded[0].flightNumber = manualInfo.value("flightNo").toString();
            response.interface.boarded[0].passengerName = manualInfo.value("passengerName").toString();
            response.interface.boarded[0].seatNumber = manualInfo.value("seatId").toString();
            response.interface.boarded[0].basePhoto = manualInfo.value("scenePhoto").toString();
        }
        response.interface.boarded[0].boardingStatus = 1;
        response.interface.boarded[0].id = "";
        response.interface.boarded[0].photoPath = photoPath;
        response.interface.boarded[0].repeatFlag = 0;
        response.interface.boarded[0].updateTime = "";
        response.interface.boarded[0].boardingTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        response.interface.boarded[0].isSameBoardingNumber = false;

        response.interface.validSize += 1;
        response.interface.dataInfo.boardingNum += 1;
    }

    // del Unboard
    for (int i = 0; i < response.interface.validSize; i++) {
        if ((response.interface.unboard[i].flightNumber == flightNumber)
                && (response.interface.unboard[i].boardingNumber == boardingNumber)
                && (response.interface.unboard[i].seatNumber == seatNumber)) {
            for (int j = i; j < response.interface.validSize; j++) {
                response.interface.unboard[j].boardingNumber = response.interface.unboard[j + 1].boardingNumber;
                response.interface.unboard[j].boardingStatus = response.interface.unboard[j + 1].boardingStatus;
                response.interface.unboard[j].cardNo = response.interface.unboard[j + 1].cardNo;
                response.interface.unboard[j].flightNumber = response.interface.unboard[j + 1].flightNumber;
                response.interface.unboard[j].id = response.interface.unboard[j + 1].id;
                response.interface.unboard[j].passengerName = response.interface.unboard[j + 1].passengerName;
                response.interface.unboard[j].photoPath = response.interface.unboard[j + 1].photoPath;
                response.interface.unboard[j].repeatFlag = response.interface.unboard[j + 1].repeatFlag;
                response.interface.unboard[j].seatNumber = response.interface.unboard[j + 1].seatNumber;
                response.interface.unboard[j].updateTime = response.interface.unboard[j + 1].updateTime;
                response.interface.unboard[j].boardingTime = response.interface.unboard[j + 1].boardingTime;
                response.interface.unboard[j].isSameBoardingNumber = response.interface.unboard[j + 1].isSameBoardingNumber;
            }
        }
    }

    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");

    int unBoardingNum = 0;
    for (int i = 0; i < response.interface.validSize && i < 1000; i++) {
        if ((response.interface.unboard[i].boardingStatus == 0) && (response.interface.results[i].id != "")) {
            unBoardingNum += 1;
        }
    }
    ui->boardingPushButton->setText("已登机人数：" + QString::number(response.interface.dataInfo.boardingNum));
    ui->notboardingPushButton->setText("未登机人数：" + QString::number(unBoardingNum));

    ui->notboardingTableWidget->hide();
    ui->orgDepTableWidget->hide();
    ui->boardingTableWidget->show();

    fillTableGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
}

FlightEnquires::~FlightEnquires()
{
    delete ui;
}

void FlightEnquires::on_queryPushButton_clicked()
{
    ui->orgDepTableWidget->clear();
    ui->orgDepTableWidget->scrollToTop();
    while (ui->orgDepTableWidget->rowCount() > 0) {
        ui->orgDepTableWidget->removeRow(0);
    }
    ui->boardingTableWidget->clear();
    ui->boardingTableWidget->scrollToTop();
    while (ui->boardingTableWidget->rowCount() > 0) {
        ui->boardingTableWidget->removeRow(0);
    }
    ui->notboardingTableWidget->clear();
    ui->notboardingTableWidget->scrollToTop();
    while (ui->notboardingTableWidget->rowCount() > 0) {
        ui->notboardingTableWidget->removeRow(0);
    }

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;
    boardingFilledNum = 0;
    boardingFillIndex = 0;
    notboardingFilledNum = 0;
    notboardingFillIndex = 0;

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

//    ui->orgDepTableWidget->show();
//    ui->boardingTableWidget->hide();
//    ui->notboardingTableWidget->hide();
    ui->boardingTableWidget->clear();
    ui->boardingTableWidget->scrollToTop();
    while (ui->boardingTableWidget->rowCount() > 0) {
        ui->boardingTableWidget->removeRow(0);
    }
    ui->boardingTableWidget->hide();

    ui->notboardingTableWidget->clear();
    ui->notboardingTableWidget->scrollToTop();
    while (ui->notboardingTableWidget->rowCount() > 0) {
        ui->notboardingTableWidget->removeRow(0);
    }
    ui->notboardingTableWidget->hide();

    ui->orgDepTableWidget->clear();
    ui->orgDepTableWidget->scrollToTop();
    while (ui->orgDepTableWidget->rowCount() > 0) {
        ui->orgDepTableWidget->removeRow(0);
    }
    ui->orgDepTableWidget->show();

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;
    boardingFilledNum = 0;
    boardingFillIndex = 0;
    notboardingFilledNum = 0;
    notboardingFillIndex = 0;

    fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
}

void FlightEnquires::on_boardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");

//    ui->orgDepTableWidget->hide();
//    ui->boardingTableWidget->show();
//    ui->notboardingTableWidget->hide();
    ui->orgDepTableWidget->clear();
    ui->orgDepTableWidget->scrollToTop();
    while (ui->orgDepTableWidget->rowCount() > 0 ) {
        ui->orgDepTableWidget->removeRow(0);
    }
    ui->orgDepTableWidget->hide();

    ui->notboardingTableWidget->clear();
    ui->notboardingTableWidget->scrollToTop();
    while (ui->notboardingTableWidget->rowCount() > 0 ) {
        ui->notboardingTableWidget->removeRow(0);
    }
    ui->notboardingTableWidget->hide();

    ui->boardingTableWidget->clear();
    ui->boardingTableWidget->scrollToTop();
    while (ui->boardingTableWidget->rowCount() > 0 ) {
        ui->boardingTableWidget->removeRow(0);
    }
    ui->boardingTableWidget->show();

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;
    boardingFilledNum = 0;
    boardingFillIndex = 0;
    notboardingFilledNum = 0;
    notboardingFillIndex = 0;

    fillTableGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
}

void FlightEnquires::on_notboardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");

//    ui->orgDepTableWidget->hide();
//    ui->boardingTableWidget->hide();
//    ui->notboardingTableWidget->show();
    ui->orgDepTableWidget->clear();
    ui->orgDepTableWidget->scrollToTop();
    while (ui->orgDepTableWidget->rowCount() > 0) {
        ui->orgDepTableWidget->removeRow(0);
    }
    ui->orgDepTableWidget->hide();

    ui->boardingTableWidget->clear();
    ui->boardingTableWidget->scrollToTop();
    while (ui->boardingTableWidget->rowCount() > 0) {
        ui->boardingTableWidget->removeRow(0);
    }
    ui->boardingTableWidget->hide();

    ui->notboardingTableWidget->clear();
    ui->notboardingTableWidget->scrollToTop();
    while (ui->notboardingTableWidget->rowCount() > 0) {
        ui->notboardingTableWidget->removeRow(0);
    }
    ui->notboardingTableWidget->show();

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;
    boardingFilledNum = 0;
    boardingFillIndex = 0;
    notboardingFilledNum = 0;
    notboardingFillIndex = 0;

    fillTableGradually(response, ui->notboardingTableWidget, Ui::DisplayTab::NotBoardingTab);
}

void FlightEnquires::on_orgDepSlider_changed(int p)
{
    if (p + 2 == orgDepFilledNum) {
        fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
    }
}

void FlightEnquires::on_boardingSlider_changed(int p)
{
    if (p + 2 == boardingFilledNum) {
        fillTableGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
    }
}

void FlightEnquires::on_notBoardingSlider_changed(int p)
{
    if (p + 2 == notboardingFilledNum) {
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
    libDelReq.flightNo = response.interface.results[widgetIndex].flightNumber;
    libDelReq.gateNo = LocalSettings::config->value("Device/gateNo").toString();
    libDelReq.boardingGate = LocalSettings::config->value("Device/boardingGate").toString();
    libDelReq.deviceCode = LocalSettings::config->value("Device/deviceId").toString();
    libDelReq.id = response.interface.results[widgetIndex].id;
    libDelReq.date = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    libDelRsp = HttpAPI::instance()->removeSpecific(libDelReq);

    if (libDelRsp.status != 0) {
        qDebug() << "libDelRsp.status: " << libDelRsp.status;
        MessageDialog msg(this, nullptr, "删除底库失败!", 1);
        msg.exec();

        return;
    }

    QString flightNoClicked = response.interface.results[widgetIndex].flightNumber;
    QString boardingNumberClicked = response.interface.results[widgetIndex].boardingNumber;

    for (int i = widgetIndex; i < response.interface.validSize - 1 && i < 1000; i++) {
        response.interface.results[i] = response.interface.results[i + 1];
    }
    response.interface.results[response.interface.validSize - 1] = FlightReviewResult();
    response.interface.validSize -= 1;

    FlightReviewResult partener;
    int count = 0;
    int index = -1;
    for (int i = 0; i < response.interface.validSize; i++) {
        if ((response.interface.results[i].flightNumber == flightNoClicked)
                && (response.interface.results[i].boardingNumber == boardingNumberClicked)) {
            count += 1;
            index = i;
        }
    }

    if (count == 1) {
        partener = response.interface.results[index];
        partener.isSameBoardingNumber = false;

        for (int i = index; i < response.interface.validSize - 1; i++) {
            response.interface.results[i] = response.interface.results[i + 1];
        }
        response.interface.results[response.interface.validSize - 1] = FlightReviewResult();
        response.interface.validSize -= 1;

        for (int i = 0; i < response.interface.validSize; i++) {
            if (partener.updateTime > response.interface.results[i].updateTime && !response.interface.results[i].isSameBoardingNumber) {
                for (int k = response.interface.validSize - 1; k >= i; k--) {
                    response.interface.results[k + 1] = response.interface.results[k];
                }
                response.interface.results[i] = partener;
                response.interface.validSize += 1;
                break;
            }
            if (i == response.interface.validSize - 1) {
                response.interface.results[i] = partener;
                response.interface.validSize += 1;
            }
        }
    }

    ui->orgDepTableWidget->scrollToTop();
    while (ui->orgDepTableWidget->rowCount() > 0 ) {
        ui->orgDepTableWidget->removeRow(0);
    }

    orgDepFilledNum = 0;
    orgDepFillIndex = 0;

    ui->orgDepPushButton->setText("航班人数：" + QString::number(response.interface.validSize));
    fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
}

QPixmap FlightEnquires::getQPixmapSync(QString str)
{
    const QUrl url = QUrl::fromUserInput(str);

    QNetworkRequest request(url);
    QNetworkReply* reply = naManager->get(request);

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QByteArray byteArray = reply->readAll();

    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    delete reply;

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
        widgetIndex = orgDepFilledNum;
        resultIndex = orgDepFillIndex;
    }
    if (tab == Ui::DisplayTab::BoardingTab) {
        widgetIndex = boardingFilledNum;
        resultIndex = boardingFillIndex;
    }
    if (tab == Ui::DisplayTab::NotBoardingTab) {
        widgetIndex = notboardingFilledNum;
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

        table->setRowHeight(widgetIndex, 206);
        table->insertRow(widgetIndex);
        table->setRowHeight(widgetIndex, 206);

        QWidget *itemWidget = nullptr;
        if ((tab == Ui::DisplayTab::DepositoryTab)
                && results[i].isSameBoardingNumber
                && isStatisticsMode
                && ((results[i].sourceType == 0) || (results[i].sourceType == 4) || (results[i].sourceType == 5) || (results[i].sourceType == 7))) {
            itemWidget = new ButtonWidget(this, widgetIndex);
        } else {
            itemWidget = new QWidget(this);
        }

        if (results[i].isSameBoardingNumber) {
            QImage sameBoardingNumberBGImage;
            sameBoardingNumberBGImage.load(":/6航班回查/Images/6航班回查/矩形-4365.png");
            sameBoardingNumberBGImage = sameBoardingNumberBGImage.scaled(766
                                                                         , 206
                                                                         , Qt::IgnoreAspectRatio
                                                                         , Qt::SmoothTransformation);
            QPixmap sameBoardingNumberBGPixmap = QPixmap::fromImage(sameBoardingNumberBGImage);
            QLabel *sameBoardingNumberBGLabel = new QLabel(itemWidget);
            sameBoardingNumberBGLabel->lower();
            sameBoardingNumberBGLabel->setGeometry(0, 0, 766, 206);
            sameBoardingNumberBGLabel->setPixmap(sameBoardingNumberBGPixmap);
            sameBoardingNumberBGLabel->setFixedSize(766, 206);
            sameBoardingNumberBGLabel->setAlignment(Qt::AlignCenter);
        }

        QPixmap pixmap = QPixmap();
        pixmap = getQPixmapSync(results[i].photoPath);
        pixmap = pixmap.scaled(131
                               , 186
                               , Qt::IgnoreAspectRatio
                               , Qt::SmoothTransformation);
        QLabel *photoLabel = new QLabel(itemWidget);
        photoLabel->setGeometry(30, 10, 131, 186);
        photoLabel->setPixmap(pixmap);
        photoLabel->setFixedSize(131, 186);
        photoLabel->setAlignment(Qt::AlignCenter);

        if ((results[i].sourceType == 5) || (results[i].sourceType == 6)) {
            QImage zhongZhuanModalImage;
            zhongZhuanModalImage.load(":/6航班回查/Images/6航班回查/中转.png");
            zhongZhuanModalImage = zhongZhuanModalImage.scaled(48
                                                               , 24
                                                               , Qt::IgnoreAspectRatio
                                                               , Qt::SmoothTransformation);
            QPixmap zhongZhuanModalPixmap = QPixmap::fromImage(zhongZhuanModalImage);
            QLabel *zhongZhuanModalLabel = new QLabel(itemWidget);
            zhongZhuanModalLabel->raise();
            zhongZhuanModalLabel->setGeometry(32, 12, 48, 24);
            zhongZhuanModalLabel->setPixmap(zhongZhuanModalPixmap);
            zhongZhuanModalLabel->setFixedSize(48, 24);
            zhongZhuanModalLabel->setAlignment(Qt::AlignCenter);
        }

        if ((results[i].sourceType == 7) || (results[i].sourceType == 8)) {
            QImage guoZhanModalImage;
            guoZhanModalImage.load(":/6航班回查/Images/6航班回查/过站.png");
            guoZhanModalImage = guoZhanModalImage.scaled(48
                                                         , 24
                                                         , Qt::IgnoreAspectRatio
                                                         , Qt::SmoothTransformation);
            QPixmap guoZhanModalPixmap = QPixmap::fromImage(guoZhanModalImage);
            QLabel *guoZhanModalLabel = new QLabel(itemWidget);
            guoZhanModalLabel->raise();
            guoZhanModalLabel->setGeometry(35, 15, 48, 24);
            guoZhanModalLabel->setPixmap(guoZhanModalPixmap);
            guoZhanModalLabel->setFixedSize(48, 24);
            guoZhanModalLabel->setAlignment(Qt::AlignCenter);
        }

        QLabel *nameTitleLabel = new QLabel(itemWidget);
        nameTitleLabel->setGeometry(180, 15, 145, 38);
        nameTitleLabel->setText("旅客姓名：");
        nameTitleLabel->setFixedSize(145, 38);
        nameTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nameTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *nameLabel = new QLabel(itemWidget);
        nameLabel->setGeometry(306, 15, 300, 38);
        nameLabel->setText(results[i].passengerName);
        nameLabel->setFixedSize(300, 38);
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nameLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *cardTitleLabel = new QLabel(itemWidget);
        cardTitleLabel->setGeometry(180, 60, 117, 38);
        cardTitleLabel->setText("证件号：");
        cardTitleLabel->setFixedSize(117, 38);
        cardTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        cardTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *cardLabel = new QLabel(itemWidget);
        cardLabel->setGeometry(278, 60, 300, 38);
        cardLabel->setText(results[i].cardNo);
        cardLabel->setFixedSize(300, 38);
        cardLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        cardLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *flightTitleLabel = new QLabel(itemWidget);
        flightTitleLabel->setGeometry(180, 105, 195, 38);
        flightTitleLabel->setText("航班号/序号：");
        flightTitleLabel->setFixedSize(195, 38);
        flightTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        flightTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *flightLabel = new QLabel(itemWidget);
        flightLabel->setGeometry(342, 105, 300, 38);
        flightLabel->setText(results[i].flightNumber + "/" + results[i].boardingNumber);
        flightLabel->setFixedSize(300, 38);
        flightLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        flightLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *seatTitleLabel = new QLabel(itemWidget);
        seatTitleLabel->setGeometry(180, 150, 117, 38);
        seatTitleLabel->setText("座位号：");
        seatTitleLabel->setFixedSize(117, 38);
        seatTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        seatTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");

        QLabel *seatLabel = new QLabel(itemWidget);
        seatLabel->setGeometry(278, 150, 300, 38);
        seatLabel->setText(results[i].seatNumber);
        seatLabel->setFixedSize(300, 38);
        seatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        seatLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *splitLabel = new QLabel(itemWidget);
        splitLabel->setGeometry(13, 0, 740, 2);
        splitLabel->setStyleSheet("image: 0; background: 0; border-radius: 0; border-width: 1px; border-style: dashed; border-color: rgb(135, 183, 194);");

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
    int unBoardingNum = 0;
    for (int i = 0; i < response.interface.validSize && i < 1000; i++) {
        if ((response.interface.results[i].boardingStatus == 0) && (response.interface.results[i].id != "")) {
            unBoardingNum += 1;
        }
    }

    ui->orgDepPushButton->setText("航班人数：" + QString::number(response.interface.dataInfo.faceNums));
    ui->boardingPushButton->setText("已登机人数：" + QString::number(response.interface.dataInfo.boardingNum));
    ui->notboardingPushButton->setText("未登机人数：" + QString::number(unBoardingNum));

    if (!response.founded) {
        qDebug() << "Queried Flight Not Found!!";

        ui->orgDepTableWidget->clear();
        ui->boardingTableWidget->clear();
        ui->notboardingTableWidget->clear();

        return Ui::DisplayType::DisplayNullErr;
    } else {
        ui->queryLineEdit->clear();
    }

    fillTableGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
//    fillTableGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
//    fillTableGradually(response, ui->notboardingTableWidget, Ui::DisplayTab::NotBoardingTab);

    return Ui::DisplayType::DisplayNormal;
}
