#include "flightenquires.h"
#include "ui_flightenquires.h"
#include "ServerInterface/httpAPI.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVBoxLayout>

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
    orgDepNum(0),
    boardingNum(0),
    notboardingNum(0)
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
}

void FlightEnquires::statistics(QString flight)
{
    this->flight = flight;
    this->isStatisticsMode = true;

    ui->inputWidget->hide();
    this->on_orgDepPushButton_clicked();

    this->show();
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
         // TODO
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

void FlightEnquires::removeRow(int widgetIndex)
{
    qDebug() << "FlightEnquires::removeRow: " << widgetIndex;
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

void FlightEnquires::tableUp(const FlightReviewResponse &response, QTableWidget *table, Ui::DisplayTab tab)
{
    int widgetIndex = 0;

    for (int i = 0; i < response.interface.validSize; i++) {
        if (tab == Ui::DisplayTab::DepositoryTab
                && response.interface.results[i].id == "") {
            continue;
        }

        if (tab == Ui::DisplayTab::BoardingTab
                && response.interface.results[i].boardingStatus != 1) {
            continue;
        }

        if (tab == Ui::DisplayTab::NotBoardingTab
                && !(response.interface.results[i].id != ""
                    && response.interface.results[i].boardingStatus == 0)) {
            continue;
        }

        table->setRowHeight(widgetIndex, 186);
        table->insertRow(widgetIndex);
        table->setRowHeight(widgetIndex, 186);

        QWidget *itemWidget = new QWidget();
        QImage img;
        img.load(":/6航班回查/Images/6航班回查/照片.png");
        img = img.scaled(131
                         , 171
                         , Qt::IgnoreAspectRatio
                         , Qt::SmoothTransformation);
        QPixmap pixmap = QPixmap::fromImage(img);
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
        nameLabel->setText("张定军");
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
        flightLabel->setText(response.interface.results[i].flightNumber);
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
        seatLabel->setText(response.interface.results[i].seatNumber);
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
        widgetIndex = widgetIndex + 1;

        if (i < response.interface.validSize - 1) {
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

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(removeRow(int)));
}

int FlightEnquires::query()
{
    FlightReviewRequest request;
    request.input = flight;
    request.queryType = 3;
    orgDepNum = 0;
    boardingNum = 0;
    notboardingNum = 0;

    FlightReviewResponse response = HttpAPI::instance()->get(request);

    if (!response.founded) {
        qDebug() << "Queried Flight Not Found!!";

        orgDepNum = 0;
        boardingNum = 0;
        notboardingNum = 0;
        ui->orgDepPushButton->setText("建库人数：" + QString::number(orgDepNum));
        ui->boardingPushButton->setText("已登机人数：" + QString::number(boardingNum));
        ui->notboardingPushButton->setText("未登机人数：" + QString::number(notboardingNum));
        ui->orgDepTableWidget->clear();
        ui->boardingTableWidget->clear();
        ui->notboardingTableWidget->clear();

        return Ui::DisplayType::DisplayNullErr;
    }

    orgDepNum = response.interface.dataInfo.orgDepNum;
    ui->orgDepPushButton->setText("建库人数：" + QString::number(orgDepNum));
    boardingNum = response.interface.dataInfo.boardingNum;
    ui->boardingPushButton->setText("已登机人数：" + QString::number(boardingNum));
    for (int i = 0; i < response.interface.validSize; i++) {
        if (response.interface.results[i].id != ""
                && response.interface.results[i].boardingStatus == 0) {
            notboardingNum += 1;
        }
    }
    ui->notboardingPushButton->setText("未登机人数：" + QString::number(notboardingNum));

    tableUp(response, ui->orgDepTableWidget, Ui::DisplayTab::DepositoryTab);
    tableUp(response, ui->boardingTableWidget, Ui::DisplayTab::BoardingTab);
    tableUp(response, ui->notboardingTableWidget, Ui::DisplayTab::NotBoardingTab);

    return Ui::DisplayType::DisplayNormal;
}
