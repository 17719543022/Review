#ifndef FLIGHTENQUIRES_H
#define FLIGHTENQUIRES_H

#include <QWidget>
#include <QTableWidget>
#include "ServerInterface/api_param_def.h"
#include <QPushButton>
#include <QSignalMapper>
#include <QNetworkAccessManager>

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

class SingleButtonWidget : public QWidget
{
    Q_OBJECT

public:
    SingleButtonWidget(QWidget *parent = Q_NULLPTR
            , int widgetIndex = 0
            , int isInterceptLabel = 0);

private:
    int widgetIndex;
};

class DoubleButtonWidget : public QWidget
{
    Q_OBJECT

public:
    DoubleButtonWidget(QWidget *parent = Q_NULLPTR
            , int widgetIndex = 0
            , int isInterceptLabel = 0);

private:
    int widgetIndex;
};

class FlightEnquires : public QWidget
{
    Q_OBJECT

public:
    explicit FlightEnquires(QWidget *parent = Q_NULLPTR);

    void statistics(QString flight);

    void fillDepAndUnboardWithMQ(const QJsonArray &msg);

    void fillBoardAndUnboardWithControl(QString flightNo
                                        , QString flightDay
                                        , int recheckType
                                        , const QJsonObject &userInfo
                                        , const QJsonObject &manualInfo);

    ~FlightEnquires();

private slots:
    void on_queryPushButton_clicked();

    void on_orgDepPushButton_clicked();

    void on_boardingPushButton_clicked();

    void on_notboardingPushButton_clicked();

    void on_orgDepSlider_changed(int p);

    void on_boardingSlider_changed(int p);

    void on_notBoardingSlider_changed(int p);

    void on_filterPushButton_clicked();

    void on_removeRow_clicked(int);

    void on_intercept_clicked(int);

private:
    int query();

    void fillTableGradually(const FlightReviewResponse &response, QTableWidget *table, Ui::DisplayTab tab, QString boardingNumber = QString());

    QPixmap getQPixmapSync(QString str);

    bool isAlreadyExist(const QJsonValue &json);

    QString findPhotoPath(QString cardNumber, QString flightNumber, QString boardingNumber, QString seatNumber);

private:
    Ui::FlightEnquires *ui;

    QSignalMapper *signalMapper;

    QNetworkAccessManager *naManager;

    FlightReviewResponse response;

    QString flight;

    bool isStatisticsMode;

    int orgDepFilledNum;

    int orgDepFillIndex;

    int boardingFilledNum;

    int boardingFillIndex;

    int notboardingFilledNum;

    int notboardingFillIndex;

    QString boardingNumberForSlider;
};

#endif // FLIGHTENQUIRES_H
