#ifndef FLIGHTENQUIRES_H
#define FLIGHTENQUIRES_H

#include <QWidget>
#include <QTableWidget>
#include "ServerInterface/api_param_def.h"
#include <QPushButton>
#include <QSignalMapper>

namespace Ui {
class FlightEnquires;

enum DisplayTab {
    DepositoryTab,
    BoardingTab,
    NotBoardingTab
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
    explicit FlightEnquires(QWidget *parent = Q_NULLPTR);

    void statistics(QString flight);

    void fillOrgDepWithMQ(const QJsonArray &msg);

    ~FlightEnquires();

private slots:
    void on_queryPushButton_clicked();

    void on_orgDepPushButton_clicked();

    void on_boardingPushButton_clicked();

    void on_notboardingPushButton_clicked();

    void on_orgDepSlider_changed(int p);

    void on_boardingSlider_changed(int p);

    void on_notBoardingSlider_changed(int p);

    void on_removeRow_clicked(int);

private:
    int query();

    void fillTableGradually(const FlightReviewResponse &response, QTableWidget *table, Ui::DisplayTab tab);

    QPixmap getQPixmapSync(QString str);

private:
    Ui::FlightEnquires *ui;

    QSignalMapper *signalMapper;

    FlightReviewResponse response;

    QString flight;

    bool isStatisticsMode;

    int orgDepFilledNum;

    int orgDepFillIndex;

    int boardingFilledNum;

    int boardingFillIndex;

    int notboardingFilledNum;

    int notboardingFillIndex;
};

#endif // FLIGHTENQUIRES_H
