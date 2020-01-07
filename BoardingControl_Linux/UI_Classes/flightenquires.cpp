#include "flightenquires.h"
#include "ui_flightenquires.h"
#include "ServerInterface/httpAPI.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVBoxLayout>

FlightEnquires::FlightEnquires(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightEnquires)
{
    ui->setupUi(this);
//    this->hide();

    ui->orgDepTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->orgDepTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->orgDepTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->orgDepTableWidget->verticalHeader()->setVisible(false);
    ui->orgDepTableWidget->horizontalHeader()->setVisible(false);
    ui->orgDepTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->orgDepTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->orgDepTableWidget->setStyleSheet("image: 0; border: 0; background: transparent;");
    ui->orgDepTableWidget->setShowGrid(false);
    ui->orgDepTableWidget->setColumnCount(2);
    ui->orgDepTableWidget->setColumnWidth(0, 16);
    ui->orgDepTableWidget->setColumnWidth(1, 750);
}

FlightEnquires::~FlightEnquires()
{
    delete ui;
}

void FlightEnquires::on_queryPushButton_clicked()
{
    FlightReviewRequest request;
    request.input = ui->queryLineEdit->text();
    request.queryType = 0;

    FlightReviewResponse response = HttpAPI::instance()->get(request);

    if (response.isFound) {
        qDebug() << "Queried Flight Not Found!!";
        return;
    }

    ui->orgDepPushButton->setText("建库人数：" + QString::number(response.interface.dataInfo.orgDepNum));
    ui->boardingPushButton->setText("已登机人数：" + QString::number(response.interface.dataInfo.boardingNum));
    ui->notboardingPushButton->setText("未登机人数：" + QString::number(response.interface.dataInfo.orgDepNum - response.interface.dataInfo.boardingNum));

    ui->orgDepTableWidget->setRowCount(0);
    ui->orgDepTableWidget->setColumnCount(2);

    for (int i = 0; i < 3; i++) {
        ui->orgDepTableWidget->setRowHeight(i, 186);
        ui->orgDepTableWidget->insertRow(i);
        ui->orgDepTableWidget->setRowHeight(i, 186);

//        QPixmap pixmap = getQPixmapSync(response.interface.results[i].photoPath);
        QImage img;
        img.load(":/6航班回查/Images/6航班回查/照片.png");
        img = img.scaled(131
                         , 171
                         , Qt::IgnoreAspectRatio
                         , Qt::SmoothTransformation);
        QPixmap pixmap = QPixmap::fromImage(img);
        QLabel *photoLabel = new QLabel();
        photoLabel->setFixedWidth(131);
        photoLabel->setPixmap(pixmap);
        photoLabel->setAlignment(Qt::AlignCenter);

        QLabel *nameTitleLabel = new QLabel();
        nameTitleLabel->setText("旅客姓名：");
        nameTitleLabel->setFixedWidth(120);
        nameTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");
        QLabel *nameLabel = new QLabel();
//        nameLabel->setText(response.interface.results[i].passengerName);
        nameLabel->setText("张定军");
        nameLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *flightTitleLabel = new QLabel();
        flightTitleLabel->setFixedWidth(170);
        flightTitleLabel->setText("航班号／序号：");
        flightTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");
        QLabel *flightLabel = new QLabel();
        flightLabel->setText(response.interface.results[i].flightNumber);
        flightLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QLabel *seatTitleLabel = new QLabel();
        seatTitleLabel->setFixedWidth(92);
        seatTitleLabel->setText("座位号：");
        seatTitleLabel->setStyleSheet("image: 0; border: 0; background: 0; font: 19pt; color: rgb(255, 255, 255);");
        QLabel *seatLabel = new QLabel();
        seatLabel->setText(response.interface.results[i].seatNumber);
        seatLabel->setStyleSheet("image: 0; border: 0; background: 0; font: bold 19pt; color: rgb(0, 228, 255);");

        QHBoxLayout *nameHBoxLayOut = new QHBoxLayout;
        nameHBoxLayOut->setMargin(10);
        nameHBoxLayOut->addWidget(nameTitleLabel);
        nameHBoxLayOut->addWidget(nameLabel);

        QHBoxLayout *flightHBoxLayOut = new QHBoxLayout;
        flightHBoxLayOut->setMargin(10);
        flightHBoxLayOut->addWidget(flightTitleLabel);
        flightHBoxLayOut->addWidget(flightLabel);

        QHBoxLayout *seatHBoxLayOut = new QHBoxLayout;
        seatHBoxLayOut->setMargin(10);
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

        ui->orgDepTableWidget->setCellWidget(i, 1, itemWidget);
    }
}

void FlightEnquires::on_orgDepPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
}

void FlightEnquires::on_boardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
}

void FlightEnquires::on_notboardingPushButton_clicked()
{
    ui->orgDepPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->boardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(0, 228, 255); background-color: rgb(0, 36, 60);");
    ui->notboardingPushButton->setStyleSheet("border: 0; border-radius: 4; color: rgb(255, 255, 255); background-color: rgb(88, 129, 157);");
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
