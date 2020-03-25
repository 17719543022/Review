#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QMessageBox>
#include "ui_messagedialog.h"

class QGridLayout;
class QLabel;
namespace Ui {
    class MessageDialog;
} // namespace Ui
class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDialog(QWidget *parent = 0, const QString &title = NULL, const QString &text = NULL, int buttonNum = 2);
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
