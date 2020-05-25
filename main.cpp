#include "exam.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Exam w;
    w.show();
    return a.exec();
}
