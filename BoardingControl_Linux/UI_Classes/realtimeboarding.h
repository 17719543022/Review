#ifndef REALTIMEBOARDING_H
#define REALTIMEBOARDING_H

#include <QWidget>

namespace Ui {
class RealtimeBoarding;
}

class RealtimeBoarding : public QWidget
{
    Q_OBJECT

public:
    explicit RealtimeBoarding(QWidget *parent = 0);
    ~RealtimeBoarding();



private:
    Ui::RealtimeBoarding *ui;
};

#endif // REALTIMEBOARDING_H
