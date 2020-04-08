#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QMessageBox>
#include "ui_messagedialog.h"
#include "ServerInterface/api_param_def.h"
#include <QSignalMapper>

class QGridLayout;
class QLabel;
namespace Ui {
    class MessageDialog;
} // namespace Ui

class FlowButtonWidget : public QWidget
{
    Q_OBJECT

public:
    FlowButtonWidget(QWidget *parent = Q_NULLPTR
            , int widgetIndex = 0
            , QString flightNo = QString()
            , QString boardingNumber = QString()
            , QString flightDay = QString());

private:
    int widgetIndex;
    QString flightNo;
    QString boardingNumber;
    QString flightDay;
};

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDialog(QWidget *parent = nullptr
            , const QString &title = nullptr
            , const QString &text = nullptr
            , int buttonNum = 2
            , int flowNum = 1
            , const FlowReviewResponse &response = FlowReviewResponse());
    ~MessageDialog();


    void setUI();
    // 设置默认按钮
    void setDefaultButton(QPushButton *button);
    //设置没有按钮
    void setNoNutton();
    // 设置提示信息
    void setText(const QString &text);
    // 添加控件-替换提示信息所在的QLabel
    void addWidget(QWidget *pWidget);
public slots:
    void close_this(int n);
    void clicked(int);

private slots:
    void on_pushButton_YES_clicked();

    void on_pushButton_NO_clicked();

    void on_pushButton_X_clicked();

private:
    int execReturnCode(QAbstractButton *button);

private:
    Ui::MessageDialog *ui;
    QGridLayout *m_pGridLayout;
    QAbstractButton *m_pDefaultButton;
};


#endif // MESSAGEDIALOG_H
