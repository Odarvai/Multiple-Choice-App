#ifndef PROFDIALOG_H
#define PROFDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QSplitter>
#include <QDialogButtonBox>
#include "exam.h"

namespace Ui {
class profDialog;
}

class profDialog : public QDialog
{
    Q_OBJECT

public:
    explicit profDialog(QWidget *parent = nullptr);
    int GetTimer();

    ~profDialog();

private slots:
    void on_intrebari_View_clicked(const QModelIndex &index);

    void on_addButton_clicked();

    void on_deleteAllbtn_clicked();

    void on_editButton_clicked();

    void on_buttonBox_rejected();

    void on_deleteButton_clicked();

    void refreshQModels();

protected:
    void closeEvent(QCloseEvent *ev) override/*{ev->ignore();}*/;
    void reject() override;


private:
    Ui::profDialog *ui;
    QSqlQueryModel *qModel;
    QSqlQueryModel *ansModel;
    QSqlQueryModel *adev_fals;
    QModelIndex *editIndex;
    QVariantList delQuestions;
    QVariantList delAnswers[4];
    QVariantList delOptions[4];
};

#endif // PROFDIALOG_H
