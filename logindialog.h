#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QDialogButtonBox>
#include <QVBoxLayout>

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    void setUpGUI();
    void setPassword(QString &password);

private:
    QLabel *passwordLabel;
    QLineEdit *pwdLineEdit;
    QDialogButtonBox *box;
    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout;
    QString password = "buiga";

private slots:
    void CheckPwd();
};

#endif // LOGINDIALOG_H
