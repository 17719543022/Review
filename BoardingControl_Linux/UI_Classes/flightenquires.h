#ifndef FLIGHTENQUIRES_H
#define FLIGHTENQUIRES_H

#include <QWidget>

namespace Ui {
class FlightEnquires;
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
    QPixmap getQPixmapSync(QString str);

private:
    Ui::FlightEnquires *ui;
};

#endif // FLIGHTENQUIRES_H
