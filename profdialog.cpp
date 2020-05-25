#include "profdialog.h"
#include "ui_profdialog.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QScrollBar>

profDialog::profDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::profDialog),
    qModel{new QSqlQueryModel(this)},
    ansModel{new QSqlQueryModel(this)},
    adev_fals{new QSqlQueryModel(this)}
{
    ui->setupUi(this);
    ui->deleteButton->setCheckable(true);
    ui->editButton->setCheckable(true);
    ui->answers_View->verticalHeader()->setVisible(false);
    ui->intrebari_View->verticalHeader()->setVisible(false);
    ui->boolTable->verticalHeader()->setVisible(false);
    ui->buttonBox->removeButton(ui->buttonBox->button(QDialogButtonBox::Cancel));
    foreach (QCheckBox *c, this->findChildren<QCheckBox*>())
    {
        c->setText("");
    }
    ui->spinBox->setMinimum(30);
    ui->spinBox->setMaximum(120);

    ui->intrebari_View->setModel(qModel);
    ui->answers_View->setModel(ansModel);
    ui->boolTable->setModel(adev_fals);

    refreshQModels();

    qModel->setHeaderData(0, Qt::Horizontal, tr("Id Intrebare"));

    ansModel->setHeaderData(0, Qt::Horizontal, tr("Id Raspunsuri"));
    ansModel->setHeaderData(1, Qt::Horizontal, tr("Raspuns a)"));
    ansModel->setHeaderData(2, Qt::Horizontal, tr("Raspuns b)"));
    ansModel->setHeaderData(3, Qt::Horizontal, tr("Raspuns c)"));
    ansModel->setHeaderData(4, Qt::Horizontal, tr("Raspuns d)"));

    adev_fals->setHeaderData(0, Qt::Horizontal, tr("Id A/F"));
    adev_fals->setHeaderData(1, Qt::Horizontal, tr("a)"));
    adev_fals->setHeaderData(2, Qt::Horizontal, tr("b)"));
    adev_fals->setHeaderData(3, Qt::Horizontal, tr("c)"));
    adev_fals->setHeaderData(4, Qt::Horizontal, tr("d)"));

    ui->intrebari_View->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->answers_View->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->answers_View->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    ui->intrebari_View->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    ui->boolTable->setColumnWidth(0, 50);
    ui->boolTable->setColumnWidth(1, 40);
    ui->boolTable->setColumnWidth(2, 40);
    ui->boolTable->setColumnWidth(3, 40);
    ui->boolTable->setColumnWidth(4, 40);
    ui->boolTable->setMaximumWidth(250);
    ui->boolTable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->answers_View->verticalScrollBar()->hide();
    ui->boolTable->verticalScrollBar()->hide();

    connect(ui->intrebari_View->verticalScrollBar(), &QScrollBar::valueChanged,
            ui->answers_View->verticalScrollBar(), &QScrollBar::setValue);
    connect(ui->intrebari_View->verticalScrollBar(), &QScrollBar::valueChanged,
            ui->boolTable->verticalScrollBar(), &QScrollBar::setValue);

}

int profDialog::GetTimer()
{
    return ui->spinBox->value();
}

profDialog::~profDialog()
{
    delete ui;
}

void profDialog::on_intrebari_View_clicked(const QModelIndex &index)
{
    if (ui->deleteButton->isChecked())
    {
        delQuestions << index.model()->index(index.row(), 1).data();
        //qDebug() << delQuestions[0].toString();
        for (int i = 0; i < 4; ++i)
        {
            delAnswers[i] << ui->answers_View->model()->index(index.row(), i+1).data();
            delOptions[i] << ui->boolTable->model()->index(index.row(), i+1).data().toBool();
        }
        /*for (int i = 0; i < 4; ++i)
        {
            qDebug() << delAnswers[i][0];
            qDebug() << delOptions[i][0];
        }*/

        QSqlQuery delqryQ;
        delqryQ.prepare("delete from intrebari where Id = :index");
        delqryQ.bindValue(":index", index.row()+1);
        QSqlQuery delqryAns;
        delqryAns.prepare("delete from raspunsuri where Id = :index");
        delqryAns.bindValue(":index", index.row()+1);
        QSqlQuery AF;
        AF.prepare("delete from Adev_sau_Fals where Id = :index");
        AF.bindValue(":index", index.row()+1);

        if (delqryQ.exec() && delqryAns.exec() && AF.exec())
        {
            int rows = qModel->rowCount();
            QSqlQuery updateQryQ;
            QSqlQuery updateQryA;
            for (int i = index.row()+1; i < rows; ++i)
            {
                updateQryQ.prepare("update intrebari set Id = :i where Id >= :i");
                updateQryQ.bindValue(":i", i);
                updateQryQ.exec();

                updateQryA.prepare("update raspunsuri set Id = :i where Id >= :i");
                updateQryA.bindValue(":i", i);
                updateQryA.exec();

                updateQryA.prepare("update Adev_sau_Fals set Id = :i where Id >= :i");
                updateQryA.bindValue(":i", i);
                updateQryA.exec();
            }
            refreshQModels();

            QSqlQuery maxIdQ("select max(Id) from intrebari");
            maxIdQ.next();
            int m1 = maxIdQ.value(0).toInt(), m2 = m1;
            maxIdQ.prepare("alter table intrebari auto_increment = :m1");
            maxIdQ.bindValue(":m1", m1);

            QSqlQuery maxIdA;
            maxIdA.prepare("alter table raspunsuri auto_increment = :m2");
            maxIdA.bindValue(":m2", m2);

            if (!maxIdQ.exec())
                qDebug() << m1 << " problema in intrebari";
            if (!maxIdA.exec())
                qDebug() << m2 << " problema in raspunsuri";
            maxIdA.prepare("alter table Adev_sau_Fals auto_increment = :m2");
            maxIdA.bindValue(":m2", m2);
            maxIdA.exec();

        }
    }
    if (ui->editButton->isChecked())
    {
        ui->lineEditQ->setText(qModel->record(index.row()).value(1).toString());
        editIndex = new QModelIndex(index);
        int i = 0;
        foreach (QLineEdit *le, this->findChildren<QLineEdit*>())
        {
            if ( i == 0 )
            {
                i++;
                continue;
            }
            if (le->objectName() == "qt_spinbox_lineedit")
                break;
            //qDebug() << le->objectName();
            le->setText(ansModel->record(index.row()).value(i).toString());
            i++;
        }
        i = 1;
        foreach (QCheckBox *c, this->findChildren<QCheckBox*>())
        {
            c->setChecked(adev_fals->record(index.row()).value(i).toBool());
            i++;
        }
    }
}

void profDialog::on_addButton_clicked()
{       
    if (ui->lineEditQ->text().isEmpty() || ui->lineEditA1->text().isEmpty() || ui->lineEditA2->text().isEmpty())
    {
        QMessageBox::warning(this, "Camp incomplet", "Trebuie sa completezi intrebarea si minim primele 2 raspunsuri");
        return;
    }
    QSqlQuery qry;
    qry.prepare("insert into intrebari (Intrebare) values (:question)");
    qry.bindValue(":question", ui->lineEditQ->text());
    qry.exec();

    qry.prepare("insert into raspunsuri (Raspuns_1, Raspuns_2, Raspuns_3, Raspuns_4) values(:ans1, :ans2, :ans3, :ans4)");
    qry.bindValue(":ans1", ui->lineEditA1->text());
    qry.bindValue(":ans2", ui->lineEditA2->text());
    qry.bindValue(":ans3", ui->lineEditA3->text());
    qry.bindValue(":ans4", ui->lineEditA4->text());
    qry.exec();
    foreach (QLineEdit *le, this->findChildren<QLineEdit*>())
    {
        le->clear();
    }
    qry.prepare("insert into Adev_sau_Fals (Raspuns_1, Raspuns_2, Raspuns_3, Raspuns_4) values(:v1, :v2, :v3, :v4)");
    qry.bindValue(":v1", ui->checkBox->isChecked());
    qry.bindValue(":v2", ui->checkBox_2->isChecked());
    qry.bindValue(":v3", ui->checkBox_3->isChecked());
    qry.bindValue(":v4", ui->checkBox_4->isChecked());
    qry.exec();
    foreach (QCheckBox *c, this->findChildren<QCheckBox*>())
    {
        c->setCheckState(Qt::Unchecked);
    }
    refreshQModels();
}

void profDialog::on_deleteAllbtn_clicked()
{
    qModel->setQuery("truncate table intrebari;");
    ansModel->setQuery("truncate table raspunsuri;");
    adev_fals->setQuery("truncate table intrebari;");
}

void profDialog::on_editButton_clicked()
{
    if (ui->editButton->isChecked())
    {
        ui->addButton->setDisabled(true);
        ui->editButton->setChecked(true);
        ui->editButton->setText("Commit changes");
    }
    else
    {
        ui->editButton->setChecked(false);
        ui->addButton->setDisabled(false);

        QSqlQuery editQry;
        editQry.prepare("update intrebari SET Intrebare = :text where Id = :editIndex");
        editQry.bindValue(":text", ui->lineEditQ->text());
        editQry.bindValue(":editIndex", editIndex->row() + 1);
        editQry.exec();
        qModel->setQuery("select *from intrebari");

        editQry.prepare("update raspunsuri set Raspuns_1 = :r1, Raspuns_2 = :r2, Raspuns_3 = :r3, Raspuns_4 = :r4 where Id = :editIndex");
        editQry.bindValue(":r1", ui->lineEditA1->text());
        editQry.bindValue(":r2", ui->lineEditA2->text());
        editQry.bindValue(":r3", ui->lineEditA3->text());
        editQry.bindValue(":r4", ui->lineEditA4->text());
        editQry.bindValue(":editIndex", editIndex->row() + 1);
        editQry.exec();
        ansModel->setQuery("select *from raspunsuri");

        foreach (QLineEdit *le, this->findChildren<QLineEdit*>())
        {
            if (le->objectName() == "qt_spinbox_lineedit")
                break;
            le->clear();           
        }

        editQry.prepare("update Adev_sau_Fals Set  Raspuns_1 = :r1, Raspuns_2 = :r2, Raspuns_3 = :r3, Raspuns_4 = :r4 where Id = :editIndex");
        editQry.bindValue(":r1", ui->checkBox->isChecked());
        editQry.bindValue(":r2", ui->checkBox_2->isChecked());
        editQry.bindValue(":r3", ui->checkBox_3->isChecked());
        editQry.bindValue(":r4", ui->checkBox_4->isChecked());
        editQry.bindValue(":editIndex", editIndex->row() + 1);
        editQry.exec();
        adev_fals->setQuery("select *from Adev_sau_Fals");
        foreach (QCheckBox *c, this->findChildren<QCheckBox*>())
        {
            c->setCheckState(Qt::Unchecked);
        }

        ui->editButton->setText("Edit");
    }
}

void profDialog::on_buttonBox_rejected()
{
    /*foreach (QSqlQueryModel *qm, this->findChildren<QSqlQueryModel*>())
    {
        qm->query().finish();
    }
    */

}


void profDialog::on_deleteButton_clicked()
{
    if (!ui->deleteButton->isChecked())
    {
        QMessageBox msgBox;
        msgBox.setText("Do you want to save the changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.exec();
        if (msgBox.result() == QMessageBox::Save)
        {                       
            msgBox.close();

            delQuestions.clear();
            delAnswers->clear();
            delOptions->clear();

            refreshQModels();
        }
        else
        {
            QSqlQuery q, a, o;
            q.prepare("insert into intrebari (Intrebare) values(?)");
            q.addBindValue(delQuestions);
            a.prepare("insert into raspunsuri (Raspuns_1, Raspuns_2, Raspuns_3, Raspuns_4) values(?, ?, ?, ?)");
            o.prepare("insert into adev_sau_fals (Raspuns_1, Raspuns_2, Raspuns_3, Raspuns_4) values(?, ?, ?, ?)");
            for (int i = 0; i < 4; ++i)
            {
                a.addBindValue(delAnswers[i]);
                o.addBindValue(delOptions[i]);
            }
            if (!q.execBatch() || !a.execBatch() || !o.execBatch())
            {
                qDebug() << q.lastError();
                qDebug() << a.lastError();
                qDebug() << o.lastError();
            }

            delQuestions.clear();
            delAnswers->clear();
            delOptions->clear();

            refreshQModels();
        }
    }
}

void profDialog::refreshQModels()
{
    qModel->setQuery("select *from intrebari");
    ansModel->setQuery("select *from raspunsuri");
    adev_fals->setQuery("select *from Adev_sau_Fals");
}


/*void profDialog::closeEvent(QCloseEvent *event)
{
    qDebug() << "S-a apelat functia";
    int rows = ui->intrebari_View->model()->rowCount();
    if (rows % 10)
    {
        QMessageBox::warning(this, "Numar de intrebari insuficient", "Numarul de intrebari trebuie sa fie un multiplu"
        "al lui 10; Exemplu: 20, 30, 40, 60");
        event->ignore();
    }
    else
    {
        qDebug() << "esti prost rau";
        event->accept();
    }
}
*/
