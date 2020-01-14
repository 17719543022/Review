#ifndef FLIGHTENQUIRES_H
#define FLIGHTENQUIRES_H

#include <QWidget>
#include <QTableWidget>
#include "ServerInterface/api_param_def.h"
#include <QPushButton>
#include <QSignalMapper>

namespace Ui {
class FlightEnquires;

enum QueryType {
    DepositoryQuery,
    BoardingQuery,
    NotBoardingQuery
};

enum DisplayType {
    DisplayNormal,
    DisplayNullErr
};
}

//QSignalMapper 或者  QObject::sender()
class RemovePushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit RemovePushButton(QWidget *parent = Q_NULLPTR, int index = 0);

signals:
    void removeRow(int index);

private:
    int index;
};

class FlightEnquires : public QWidget
{
    Q_OBJECT

public:
    explicit FlightEnquires(QWidget *parent = Q_NULLPTR);
    void statistics(QString flight);
    ~FlightEnquires();

private slots:
    void on_queryPushButton_clicked();

    void on_orgDepPushButton_clicked();

    void on_boardingPushButton_clicked();

    void on_notboardingPushButton_clicked();

    void removeRow(int);

private:
    int query(int queryType);

    void tableUp(const FlightReviewResponse &response, QTableWidget *table);

    QPixmap getQPixmapSync(QString str);

private:
    Ui::FlightEnquires *ui;

    QSignalMapper *signalMapper;

    QString flight;

    bool isStatisticsMode;

    int orgDepNum;

    int boardingNum;

    int notboardingNum;
};

#endif // FLIGHTENQUIRES_H
