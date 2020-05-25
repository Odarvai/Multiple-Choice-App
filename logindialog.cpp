#include "logindialog.h"
#include <QPushButton>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)  
  , passwordLabel{new QLabel(this)}
  , pwdLineEdit{new QLineEdit(this)}
  , box{new QDialogButtonBox(this)}
  , hlayout{new QHBoxLayout(this)}
  //, vlayout{new QVBoxLayout(this)}
{
    setWindowTitle("Login as Proffesor");
    setModal(true);

    passwordLabel->setText("Introduce password to the box in the right");
    box->addButton("Login", QDialogButtonBox::AcceptRole);
    box->addButton(QDialogButtonBox::Cancel);

    hlayout->addWidget(passwordLabel);
    hlayout->addWidget(pwdLineEdit);
    hlayout->addWidget(box);
    //vlayout->addLayout(hlayout);
    setLayout(hlayout);


    connect(box, &QDialogButtonBox::accepted, this, &LoginDialog::CheckPwd);
    //connect(box, SIGNAL(accepted()), this, SLOT(CheckPwd()));
    //connect(box, &QDialogButtonBox::accepted, this, CheckPwd);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    pwdLineEdit->setEchoMode(QLineEdit::Password);
}

void LoginDialog::CheckPwd()
{
    if(pwdLineEdit->text() == password)
        accept();
}
