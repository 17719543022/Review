#include "flightenquires.h"
#include "ui_flightenquires.h"
#include "ServerInterface/httpAPI.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVBoxLayout>
#include "messagedialog.h"

RemovePushButton::RemovePushButton(QWidget *parent, int index) :
    QPushButton (parent),
    index(index)
{

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

FlightEnquires::~FlightEnquires()
{
    delete ui;
}

void FlightEnquires::on_queryPushButton_clicked()
{
    flight = ui->queryLineEdit->text();

    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");

    ui->orgDepTableWidget->show();
    ui->boardingTableWidget->hide();
    ui->notboardingTableWidget->hide();

    if (Ui::DisplayType::DisplayNullErr == query()) {
         MessageDialog msg(this, NULL, "请输入正确的航班号!", 1);
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
        tableFillGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
    }
}

void FlightEnquires::on_boardingSlider_changed(int p)
{
    if (p > boardingFilledNum * 2 - 6) {
        tableFillGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
    }
}

void FlightEnquires::on_notBoardingSlider_changed(int p)
{
    if (p > notboardingFilledNum * 2 - 6) {
        tableFillGradually(response, ui->notboardingTableWidget, Ui::DisplayTab::NotBoardingTab);
    }
}

void FlightEnquires::on_removeRow_clicked(int widgetIndex)
{
     qDebug() << "FlightEnquires::on_removeRow_clicked: " << widgetIndex;
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

    return pixmap;
}

void FlightEnquires::tableFillGradually(const FlightReviewResponse &response, QTableWidget *table, Ui::DisplayTab tab)
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

        QWidget *itemWidget = new QWidget();

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

        if (isStatisticsMode) {
            RemovePushButton *removePushButton = new RemovePushButton(itemWidget, widgetIndex);
            removePushButton->setGeometry(600, 134, 140, 40);
            removePushButton->setText("删    除");
            removePushButton->setFixedSize(140, 40);
            removePushButton->setStyleSheet("image: 0; border: 0; border-radius: 4; background: rgb(255, 0, 0); font: 19pt; color: rgb(255, 255, 255);");

            connect(removePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
            signalMapper->setMapping(removePushButton, widgetIndex);
        }

        table->setCellWidget(widgetIndex, 0, itemWidget);
        batch += 1;
        widgetIndex += 1;

        if (tab == Ui::DisplayTab::DepositoryTab) {
            orgDepFilledNum += 1;
            if (orgDepFilledNum == response.interface.dataInfo.faceNums) {
                continue;
            }
        }
        if (tab == Ui::DisplayTab::BoardingTab) {
            boardingFilledNum += 1;
            if (boardingFilledNum == response.interface.dataInfo.boardingNum) {
                continue;
            }
        }
        if (tab == Ui::DisplayTab::NotBoardingTab) {
            notboardingFilledNum += 1;
            if (notboardingFilledNum == response.interface.total - response.interface.dataInfo.boardingNum) {
                continue;
            }
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
    request.queryType = 0;

    response = HttpAPI::instance()->get(request);

//    for (int i = 0; i < response.interface.validSize; i++) {
//        qDebug() << response.interface.unboard[i].seatNumber;
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

    tableFillGradually(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
//    tableFillGradually(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
    tableFillGradually(response, ui->notboardingTableWidget, Ui::DisplayTab::NotBoardingTab);

    return Ui::DisplayType::DisplayNormal;
}
