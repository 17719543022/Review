#ifndef WORKFLOWRECORDING_H
#define WORKFLOWRECORDING_H

#include <QWidget>
#include <QNetworkReply>

namespace Ui {
class WorkflowRecording;
}

class WorkflowRecording : public QWidget
{
    Q_OBJECT

public:
    explicit WorkflowRecording(QWidget *parent = 0);
    ~WorkflowRecording();


private slots:
    void on_flowBabyPushButton_clicked();

    void on_flowQueryPushButton_clicked();

private:
    Ui::WorkflowRecording *ui;

private:
    bool isBabyMode;
};

#endif // WORKFLOWRECORDING_H
