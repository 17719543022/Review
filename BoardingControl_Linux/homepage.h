#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>

namespace Ui {
class HomePage;
}

class RealtimeBoarding;
class WorkflowRecording;
class FlightEnquires;
class PortraitCollection;

class QTimeLine;
class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = 0);
    ~HomePage();

private slots:
    void on_Button_RealtimeBoarding_clicked();

    void on_Button_WorkflowRecording_clicked();

    void on_Button_FlightEnquires_clicked();

    void on_Button_PortraitCollection_clicked();

    void on_Button_menu_clicked();

    void frameChange(int f);

signals:
        void timeLineStart();

private:
    Ui::HomePage *ui{nullptr};

    RealtimeBoarding *m_RealtimeBoarding{nullptr};
    WorkflowRecording *m_WorkflowRecording{nullptr};
    FlightEnquires * m_FlightEnquires{nullptr};
    PortraitCollection *m_PortraitCollection{nullptr};

    QTimeLine *timeLine;
};

#endif // HOMEPAGE_H
