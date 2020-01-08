#ifndef FLIGHTENQUIRES_H
#define FLIGHTENQUIRES_H

#include <QWidget>
#include <QTableWidget>
#include "ServerInterface/api_param_def.h"

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

class FlightEnquires : public QWidget
{
    Q_OBJECT

public:
    explicit FlightEnquires(QWidget *parent = 0);
    ~FlightEnquires();

private slots:
    void on_queryPushButton_clicked();

    void on_orgDepPushButton_clicked();

    void on_boardingPushButton_clicked();

    void on_notboardingPushButton_clicked();

private:
    int query(int queryType);

    void tableUp(const FlightReviewResponse &response, QTableWidget *table);

    QPixmap getQPixmapSync(QString str);

private:
    Ui::FlightEnquires *ui;

    QString flight;

    int orgDepNum;

    int boardingNum;

    int notboardingNum;
};

#endif // FLIGHTENQUIRES_H
