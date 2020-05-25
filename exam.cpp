#include "exam.h"
#include "ui_exam.h"
#include "profdialog.h"
#include <QDialog>
#include <QtSql>
#include <QtCore>
#include <QtGui>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QtDebug>
#include "logindialog.h"
#include "studentexam.h"
#include <QSqlDriver>

Exam::Exam(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Exam)
{
    ui->setupUi(this);
    OpenDatabase();
}

Exam::~Exam()
{
    delete ui;
}

void Exam::on_profBtn_clicked()
{
    log = new LoginDialog(this);
    
    if (log->exec())
    {
        profDialog profDiag;
        profDiag.setModal(true);

        hide();
        profDiag.resize(1000,1000);
        if (profDiag.exec())
        {
            qDebug() << "changes accepted";
        }
        else
        {
            qDebug() << "changes didn't occur";
        }
        show();
        /*if (profDiag.result() == QDialog::Accepted)
            QSqlDatabase::database("om").commit();
        else if (profDiag.result() == QDialog::Rejected)
        {
            if (QSqlDatabase::database("om").rollback())
                qDebug() << "rollbackul reusit";
            else
                qDebug() << "rollbackul n-a reusit\n" <<
            QSqlDatabase::database("om").lastError().text();
        }
        */
    }
}

void Exam::on_examBtn_clicked()
{
    studentexam *e = new studentexam(this);
    e->setModal(true);
    e->exec();
}

void Exam::OpenDatabase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("om");
    db.setHostName("localhost");
    db.setUserName("odv");
    db.setPassword("root");
    db.setPort(3306);
    if(!db.open())
    {
        QMessageBox::information(qApp->activeModalWidget(), "title","nu s-a facut conexiunea");
        ui->profBtn->setDisabled(true);
        ui->examBtn->setDisabled(true);
    }
    else
    {
        //db.transaction();
        qDebug() << "Conexiune reusita";
    }

}
