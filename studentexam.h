#ifndef STUDENTEXAM_H
#define STUDENTEXAM_H

#include <QDialog>
#include <QSqlQuery>
#include <QVector>
#include <QCheckBox>
#include <QElapsedTimer>
#include <QTimer>

namespace Ui {
class studentexam;
}

class studentexam : public QDialog
{
    Q_OBJECT

public:
    explicit studentexam(QWidget *parent = nullptr);
    ~studentexam();

private slots:
    void on_nextQsBtn_clicked();

    void on_previousQsBtn_clicked();

    int on_correctExamBtn_clicked();

    QString timeConvertMS(qint64);

    void startTimer();

    void delAnswers();

    void showGrade(int);

private:
    Ui::studentexam *ui;
    QSqlQuery selectQsQry;
    QSqlQuery selectAnsQry;
    QSqlQuery TorF;
    bool answers[100][4] = {};
    QElapsedTimer *eTimer;
    QTimer *timer;
};

#endif // STUDENTEXAM_H
