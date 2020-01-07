#ifndef PORTRAITCOLLECTION_H
#define PORTRAITCOLLECTION_H

#include <QWidget>

namespace Ui {
class PortraitCollection;
}

class PortraitCollection : public QWidget
{
    Q_OBJECT

public:
    explicit PortraitCollection(QWidget *parent = 0);
    ~PortraitCollection();



private:
    Ui::PortraitCollection *ui;
};

#endif // PORTRAITCOLLECTION_H
