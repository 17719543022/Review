#include "messagedialog.h"
#include "ui_messagedialog.h"

#include <QGridLayout>
#include <QApplication>
#include <QDebug>


MessageDialog::MessageDialog(QWidget *parent, const QString &title, const QString &text, int buttonNum)
    :QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);

    if (title != NULL){
        ui->label_title->setText(title);
        ui->centralWidget->setStyleSheet("#centralWidget{image: url(:/3实时登机/Images/3实时登机/弹窗/大弹窗BG.png);}");
        ui->pushButton_X->move(475,18);
    }
    else {ui->label_title->setText("");}
    ui->label_txt->setText(text);


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
