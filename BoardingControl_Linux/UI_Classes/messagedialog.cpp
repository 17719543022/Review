#include "messagedialog.h"
#include "ui_messagedialog.h"

#include <QGridLayout>
#include <QApplication>
#include <QDebug>

FlowButtonWidget::FlowButtonWidget(QWidget *parent
                                   , int widgetIndex
                                   , QString flightNo
                                   , QString boardingNumber
                                   , QString flightDay)
    : QWidget(parent)
    , widgetIndex(widgetIndex)
    , flightNo(flightNo)
    , boardingNumber(boardingNumber)
    , flightDay(flightDay)
{
    QSignalMapper *signalMapper = new QSignalMapper();

    QPushButton *removePushButton = new QPushButton(this);
    removePushButton->setGeometry(64, 29, 312, 47);
    removePushButton->setFixedSize(312, 47);
    removePushButton->setStyleSheet("image: 0; border: 0; border-radius: 4; background: rgb(2, 112, 187); font: 19pt; color: rgb(255, 255, 255);");

    QLabel *flightLabel = new QLabel(removePushButton);
    flightLabel->setStyleSheet("color: rgb(255, 255, 255); font: bold; font-size: 32px;");
    flightLabel->setText(flightNo + "/" + boardingNumber);
    flightLabel->setGeometry(15, 0, 200, 47);
    flightLabel->setFixedSize(200, 47);

    QLabel *dateLabel = new QLabel(removePushButton);
    dateLabel->setStyleSheet("color: rgb(255, 255, 255); font: bold; font-size: 16px;");
    dateLabel->setText("(" + flightDay.left(4) + "/" + flightDay.mid(4, 2) + "/" + flightDay.mid(6, 2) + ")");
    dateLabel->setGeometry(205, 5, 100, 47);
    dateLabel->setFixedSize(100, 47);

    connect(removePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(removePushButton, widgetIndex);

    connect(signalMapper, SIGNAL(mapped(int)), parent, SLOT(clicked(int)));
}

void MessageDialog::clicked(int widgetIndex)
{
    done(65535 + widgetIndex);
}

MessageDialog::MessageDialog(QWidget *parent
                             , const QString &title
                             , const QString &text
                             , int buttonNum
                             , int flowNum
                             , const FlowReviewResponse &response)
    :QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    ui->flowTableWidget->hide();

    if (title != nullptr){
        ui->label_title->setText(title);
        ui->centralWidget->setStyleSheet("#centralWidget{image: url(:/3实时登机/Images/3实时登机/弹窗/大弹窗BG.png);}");
        ui->pushButton_X->move(475,18);

        if (0 == QString::compare(title, "请选择旅客")) {

        }
    }
    else {
        ui->label_title->setText("");
    }

    ui->label_txt->setText(text);

    if (flowNum > 1) {
        ui->flowTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->flowTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->flowTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->flowTableWidget->verticalHeader()->setVisible(false);
        ui->flowTableWidget->horizontalHeader()->setVisible(false);
        ui->flowTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        ui->flowTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui->flowTableWidget->setStyleSheet("image: 0; border: 0; background: transparent;");
        ui->flowTableWidget->setShowGrid(false);
        ui->flowTableWidget->setColumnCount(1);
        ui->flowTableWidget->setColumnWidth(0, 440);

        for (int i = 0; i < flowNum; i++) {
            ui->flowTableWidget->setRowHeight(i, 105);
            ui->flowTableWidget->insertRow(i);
            ui->flowTableWidget->setRowHeight(i, 105);

            QString flightNo = response.interface.result[i].userInfo.flightNo;
            QString boardingNumber = response.interface.result[i].userInfo.boardingNumber;
            QString flightDay = response.interface.result[i].userInfo.flightDay;
            QWidget *widget = new FlowButtonWidget(this, i, flightNo, boardingNumber, flightDay);

            ui->flowTableWidget->setCellWidget(i, 0, widget);
        }

        ui->flowTableWidget->show();
    }

    switch (buttonNum) {
    case 0:
        ui->pushButton_NO->hide();
        ui->pushButton_YES->hide();
        break;
    case 1:
        ui->pushButton_NO->hide();
        ui->pushButton_YES->move(206, 240);
        break;
    case 2:
        break;
    default:
        break;
    }

    setUI();
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

void MessageDialog::setUI()
{
    this->setWindowFlags(Qt::FramelessWindowHint);          //Set no border, no minimize button.
    this->setAttribute(Qt::WA_TranslucentBackground,true);        //Set transparent background.
//    this->QWidget::setCursor(QCursor(Qt::BlankCursor));           //Hide mouse point.

    this->setModal(true);
    this->setFixedSize(800, 978);
    this->move(0,0);
    this->setDefaultButton(ui->pushButton_NO);

    m_pGridLayout = new QGridLayout(ui->centralWidget);
    m_pGridLayout->setContentsMargins(38, 65, 38, 110);
}

void MessageDialog::setDefaultButton(QPushButton *button)
{
    m_pDefaultButton = button;
    button->setDefault(true);
}

void MessageDialog::setNoNutton()
{
    ui->pushButton_NO->hide();
    ui->pushButton_NO->hide();
    ui->pushButton_YES->hide();
}

void MessageDialog::setText(const QString &text)
{
    ui->label_txt->setText(text);
}

void MessageDialog::addWidget(QWidget *pWidget)
{
    ui->label_txt->hide();
    m_pGridLayout->addWidget(pWidget);
}

void MessageDialog::close_this(int n)
{
    done(n);
}


void MessageDialog::on_pushButton_YES_clicked()
{
    done(1);
}

void MessageDialog::on_pushButton_NO_clicked()
{
    done(0);
}

void MessageDialog::on_pushButton_X_clicked()
{
    done(0);
}
