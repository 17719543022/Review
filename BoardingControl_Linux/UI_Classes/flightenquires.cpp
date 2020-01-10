#include "flightenquires.h"
#include "ui_flightenquires.h"
#include "ServerInterface/httpAPI.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVBoxLayout>

FlightEnquires::FlightEnquires(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightEnquires),
    flight(QString()),
    orgDepNum(0),
    boardingNum(0),
    notboardingNum(0)
{
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

    if (Ui::DisplayType::DisplayNullErr == query(Ui::QueryType::DepositoryQuery)) {
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

    query(Ui::QueryType::DepositoryQuery);
}

void FlightEnquires::on_boardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");

    ui->orgDepTableWidget->hide();
    ui->boardingTableWidget->show();
    ui->notboardingTableWidget->hide();

    query(Ui::QueryType::BoardingQuery);
}

void FlightEnquires::on_notboardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");

    ui->orgDepTableWidget->hide();
    ui->boardingTableWidget->hide();
    ui->notboardingTableWidget->show();

    query(Ui::QueryType::NotBoardingQuery);
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

void FlightEnquires::tableUp(const FlightReviewResponse &response, QTableWidget *table)
{
    for (int i = 0; i < response.interface.validSize; i++) {
        table->setRowHeight(i, 186);
        table->insertRow(i);
        qDebug() << "table->insertRow(" << i << ")";
        table->setRowHeight(i, 186);

//        QPixmap pixmap = getQPixmapSync(response.interface.results[i].photoPath);
        QImage img;
        img.load(":/6航班回查/Images/6航班回查/照片.png");
        img = img.scaled(131
                         , 171
                         , Qt::IgnoreAspectRatio
                         , Qt::SmoothTransformation);
        QPixmap pixmap = QPixmap::fromImage(img);
        QLabel *photoLabel = new QLabel();
        photoLabel->setPixmap(pixmap);
        photoLabel->setFixedWidth(151);
        photoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QLabel *nameTitleLabel = new QLabel();
        nameTitleLabel->setText("旅客姓名：");
        nameTitleLabel->setFixedWidth(140);
        nameTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        nameTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");
        QLabel *nameLabel = new QLabel();
//        nameLabel->setText(response.interface.results[i].passengerName);
        nameLabel->setText("张定军");
        nameLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *flightTitleLabel = new QLabel();
        flightTitleLabel->setText("航班号／序号：");
        flightTitleLabel->setFixedWidth(190);
        flightTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        flightTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");
        QLabel *flightLabel = new QLabel();
        flightLabel->setText(response.interface.results[i].flightNumber);
        flightLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *seatTitleLabel = new QLabel();
        seatTitleLabel->setText("座位号：");
        seatTitleLabel->setFixedWidth(112);
        seatTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        seatTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");
        QLabel *seatLabel = new QLabel();
        seatLabel->setText(response.interface.results[i].seatNumber);
        seatLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QHBoxLayout *nameHBoxLayOut = new QHBoxLayout;
        nameHBoxLayOut->addWidget(nameTitleLabel);
        nameHBoxLayOut->addWidget(nameLabel);

        QHBoxLayout *flightHBoxLayOut = new QHBoxLayout;
        flightHBoxLayOut->addWidget(flightTitleLabel);
        flightHBoxLayOut->addWidget(flightLabel);

        QHBoxLayout *seatHBoxLayOut = new QHBoxLayout;
        seatHBoxLayOut->addWidget(seatTitleLabel);
        seatHBoxLayOut->addWidget(seatLabel);

        QVBoxLayout *textVBoxLayOut = new QVBoxLayout;
        textVBoxLayOut->addLayout(nameHBoxLayOut);
        textVBoxLayOut->addLayout(flightHBoxLayOut);
        textVBoxLayOut->addLayout(seatHBoxLayOut);

        QWidget *itemWidget = new QWidget();
        QGridLayout *itemLayOut = new QGridLayout(itemWidget);
        itemLayOut->addWidget(photoLabel);
        itemLayOut->addLayout(textVBoxLayOut, 0, 1);

        table->setCellWidget(i, 0, itemWidget);
    }
}

int FlightEnquires::query(int queryType)
{
    FlightReviewRequest request;
    request.input = flight;
    request.queryType = queryType;

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

    if (queryType == Ui::QueryType::DepositoryQuery) {
        orgDepNum = response.interface.dataInfo.orgDepNum;
        ui->orgDepPushButton->setText("建库人数：" + QString::number(orgDepNum));
        boardingNum = response.interface.dataInfo.boardingNum;
        ui->boardingPushButton->setText("已登机人数：" + QString::number(boardingNum));
        notboardingNum = response.interface.dataInfo.orgDepNum - response.interface.dataInfo.boardingNum;
        ui->notboardingPushButton->setText("未登机人数：" + QString::number(notboardingNum));

        tableUp(response, ui->orgDepTableWidget);
    }

    if (queryType == Ui::QueryType::BoardingQuery) {
        ui->orgDepPushButton->setText("建库人数：" + QString::number(orgDepNum));
        boardingNum = response.interface.dataInfo.boardingNum;
        ui->boardingPushButton->setText("已登机人数：" + QString::number(boardingNum));
        notboardingNum = orgDepNum - boardingNum;
        ui->notboardingPushButton->setText("未登机人数：" + QString::number(notboardingNum));

        tableUp(response, ui->boardingTableWidget);
    }
    if (queryType == Ui::QueryType::NotBoardingQuery) {
        ui->orgDepPushButton->setText("建库人数：" + QString::number(orgDepNum));
        ui->boardingPushButton->setText("已登机人数：" + QString::number(boardingNum));
        ui->notboardingPushButton->setText("未登机人数：" + QString::number(notboardingNum));

        tableUp(response, ui->notboardingTableWidget);
    }

    return Ui::DisplayType::DisplayNormal;
}
