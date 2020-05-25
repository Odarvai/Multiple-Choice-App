#include "studentexam.h"
#include "ui_studentexam.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QList>
#include <iterator>
#include <QProcess>
#include "profdialog.h"
#include <cstring>

studentexam::studentexam(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::studentexam)
    ,timer{new QTimer}
{
    ui->setupUi(this);
    resize(1290, 1000);

    selectQsQry.setForwardOnly(true);
    selectAnsQry.setForwardOnly(true);

    selectQsQry.exec("select *from intrebari");

    foreach (QLabel *lbl, this->findChildren<QLabel*>())
    {
        if (lbl == ui->timeLabel)
        {
            qDebug() << lbl->objectName();
            ui->timeLabel->setText("loading");
            continue;
        }

        selectQsQry.next();
        lbl->setText(selectQsQry.value(1).toString());
        qDebug() << lbl->objectName();
    }

    selectAnsQry.exec("select *from raspunsuri");
    int i = 1;
    selectAnsQry.next();
    foreach (QCheckBox *box, this->findChildren<QCheckBox*>())
    {
        if(i == 5)
        {
            selectAnsQry.next();
            i = 1;
        }

        box->setText(selectAnsQry.value(i).toString());
        i++;
    }
    //qDebug() << selectQsQry.at() << selectAnsQry.at();
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    timer->setInterval(1000);
    timer->start(1001);
    connect(timer, &QTimer::timeout, this, &studentexam::startTimer);
    //connect(this, &studentexam::accepted, this, &studentexam::delAnswers);
}

studentexam::~studentexam()
{
    delete ui;
}

void studentexam::on_nextQsBtn_clicked()
{
    QProcess::execute("CLS");
    selectQsQry.setForwardOnly(true);
    selectAnsQry.setForwardOnly(true);

    if (selectQsQry.at() == selectQsQry.size()-1)
    {
        QMessageBox::information(this,"You've reached the end!", "There are no more questions");
    }
    else
    {
        //qDebug() << selectAnsQry.at();
        // Salveaza alegerile de pe pagina curenta
        int i = selectAnsQry.at()-9, j = 1;
        foreach (QCheckBox *checkB, this->findChildren<QCheckBox*>())
        {
            answers[i][j] = checkB->isChecked();
            if (j == 4)
            {
                i++;
                j = 1;
                continue;
            }
            j++;
        }
                        
        //Seteaza intrebarile de pe pagina urmatoare
        foreach (QLabel *lbl, this->findChildren<QLabel*>())
        {
            if (lbl == ui->timeLabel)
            {
                qDebug() << lbl->objectName();
                //lbl->setText("Ai apelat next");
                continue;
            }
            selectQsQry.next();
            lbl->setText(selectQsQry.value(1).toString());
            qDebug() << lbl->objectName();
        }

        // Seteaza raspunsurile de pe pagina urmatoare
        j = 1;
        selectAnsQry.next();
        i = selectAnsQry.at();
        foreach (QCheckBox *box, this->findChildren<QCheckBox*>())
        {
            if(j == 5)
            {
                selectAnsQry.next();
                i++;
                j = 1;
            }
            box->setText(selectAnsQry.value(j).toString());
            box->setChecked(answers[i][j]);
            j++;
        }
    }
    //qDebug() << selectQsQry.at() << selectAnsQry.at();
}

void studentexam::on_previousQsBtn_clicked()
{
    QProcess::execute("CLS");

    selectQsQry.setForwardOnly(false);
    selectAnsQry.setForwardOnly(false);
    if (selectQsQry.at() == 9)
    {
        QMessageBox::information(this, "You can't turn the page back", "You're on the first page");
        return;
    }

    // Salveaza alegerile de pe pagina curenta
    int i = selectAnsQry.at()-9, j = 1;
    foreach (QCheckBox *checkB, this->findChildren<QCheckBox*>())
    {
        answers[i][j] = checkB->isChecked();
        if (j == 4)
        {
            i++;
            j = 1;
            continue;
        }
        j++;
    }

    //qDebug() << selectQsQry.at() << selectAnsQry.at();

    selectQsQry.seek(selectQsQry.at()-19);
    selectAnsQry.seek(selectAnsQry.at()-19);

    //Incarca intrebarile de pe pagina anterioara
    QList<QLabel*> labelList = this->findChildren<QLabel*>();
    QList<QLabel*>::iterator it = labelList.begin();
    do
    {
        /*if ( (*it)->objectName() == ui->timeLabel->objectName())
        {
            it++;
            break;
        }
        */
        //it->setText(selectQsQry.value(1).toString());

        (*it)->setText(selectQsQry.value(1).toString());
        qDebug() << (*it)->objectName();
        if (it == labelList.end()-2)
        {
            break;
        }
        selectQsQry.next();
        it++;

    }while (it != labelList.end());

    qDebug() << selectQsQry.at() << selectAnsQry.at();

    //Incarca raspunsurile de pe pagina anterioara
    i = selectAnsQry.at(), j = 1;
    foreach(QCheckBox *box, this->findChildren<QCheckBox*>())
    {
        if(j == 5)
        {
            selectAnsQry.next();
            i++;
            j = 1;
        }
        box->setChecked(answers[i][j]);
        box->setText(selectAnsQry.value(j).toString());

        j++;
    }
    //qDebug() << selectQsQry.at() << selectAnsQry.at();
}

int studentexam::on_correctExamBtn_clicked()
{
    //Salveaza raspunsurile de pe pagina unde erai cand ai apasat butonul Mark
    //Trebuie facut asta pentru ca neapasand next sau previous nu se adauga automat
    //cand faci modificari pe o pagina dupa dai Mark
    int i = selectAnsQry.at()-9, j = 1;
    foreach(QCheckBox *checkB, this->findChildren<QCheckBox*>())
    {
        answers[i][j] = checkB->isChecked();
        if (j == 4)
        {
            i++;
            j = 1;
            continue;
        }
        j++;
    }

    QSqlQuery TorF("select *from Adev_sau_Fals");
    TorF.exec();
    TorF.next();
    //qDebug() << selectQsQry.at() << selectAnsQry.at();

    int points = 0;
    bool rightAnswer;
    int rows = selectAnsQry.size();

    for (int i = 0; i < rows; ++i )
    {
        rightAnswer = true;
        for (int j = 1; j <= 4; ++j)
        {
            if (answers[i][j] != TorF.value(j).toBool())
                rightAnswer = false;
        }
        if (rightAnswer)
            points++;
        TorF.next();
    }

    qDebug() << points;
    delAnswers();

    showGrade(points);

    this->accept();
    return points;
}

QString studentexam::timeConvertMS(qint64 timeIn_ms)
{
    int minutes = timeIn_ms / 1000 / 60;
    int seconds = timeIn_ms / 1000 % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void studentexam::startTimer()
{
    //bool ok;
    //qint64 counter = ui->spinBox->text().toLongLong(&ok, 10) * 1000;
    qint64 counter = profDialog().GetTimer()*60000;
    eTimer = new QElapsedTimer();
    while (counter)
    {
        eTimer->start();
        while (eTimer->elapsed() < 1000)
        {
            QApplication::processEvents();
        }
        ui->timeLabel->setText(timeConvertMS(counter));
        qDebug("%d", int(counter));
        counter -= 1000;
        if (!counter)
        {
            on_correctExamBtn_clicked();
            this->accept();
            qApp->exit();
        }
    }
}

void studentexam::delAnswers()
{
    memset(answers, false, sizeof (answers));
    qDebug() << "raspunsurile au fost sterse";
}

void studentexam::showGrade(int points)
{
    if (points < 5)
    {
        QMessageBox::information(this, "Nota", "Esti un netrebnic");
        return;
    }
    else if (points < 10)
    {
        QMessageBox::information(this, "Nota", "Nu esti bun de nimic");
        return;
    }
    else if (points < 15)
    {
        QMessageBox::information(this, "Nota", "Esti varza");
        return;
    }
    else if (points < 20)
    {
        QMessageBox::information(this, "Nota", "Ti-as da 5 dar nu meriti");
        return;
    }
    else if (points < 25)
    {
        QMessageBox::information(this, "Nota", "Ai luat nota 6");
        return;
    }
    else if (points < 30)
    {
        QMessageBox::information(this, "Nota", "Ai luat nota 7");
        return;
    }
    else if (points < 35)
    {
        QMessageBox::information(this, "Nota", "Ai luat nota 8");
        return;
    }
    else if (points < 40)
    {
        QMessageBox::information(this, "Nota", "Ai luat nota 9");
        return;
    }
    else if (points == 40)
    {
        QMessageBox::information(this, "Nota", "Ai luat nota 10");
        return;
    }
    else
    {
        qDebug() << "Eroare in on_correct_button, nota nu este calculata bine";
    }
}
