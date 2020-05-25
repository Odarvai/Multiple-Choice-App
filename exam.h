#ifndef EXAM_H
#define EXAM_H

#include <QMainWindow>
#include <QtSql>
#include <QtCore>
#include <QtGui>
#include <QSplitter>
#include <QDialogButtonBox>
#include "logindialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Exam; }
QT_END_NAMESPACE

class Exam : public QMainWindow
{
    Q_OBJECT

public:
    Exam(QWidget *parent = nullptr);
    QSqlDatabase db;
    ~Exam();

private slots:
    void on_profBtn_clicked();
    void OpenDatabase();
    void on_examBtn_clicked();

private:
    Ui::Exam *ui;
    QSqlQueryModel *qModel;
    QSqlQueryModel *ansModel;
    QDialogButtonBox *box;
    LoginDialog *log;
    //QSqlDatabase db;
};
#endif // EXAM_H
