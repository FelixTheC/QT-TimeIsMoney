#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/favicon.ico"));

    auto db = TaskDB();

    MainWindow w(db.getDBPtr());
    // w.setFixedSize(w.size());
    w.show();

    return a.exec();
}
