#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialreader.h"
#include "task.h"
#include "taskdb.h"

#include <QMainWindow>
#include <QCloseEvent>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QSqlDatabase *database, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void runCmd(const std::string &val);
    void startTask();
    void cancelTask();
    void clientname_changed(const QString &val);
    void taskname_changed(const QString &val);
    void price_changed(const QString &val);
    void taskinfo_changed();
    void closeEvent(QCloseEvent *event);
    void usbPort_Changed(const QString &val);
    void usbPort_ChangeDialogClosed();

    void on_actionOverview_triggered();
    void on_actionCreate_Invoice_triggered();
    void on_actionPort_triggered();


private:
    Ui::MainWindow *ui;
    void newTaskDialog();
    Task *currentTask = nullptr;
    QTimer *serialTimer;
    QTimer *infoTimer;
    SerialReader *serialReader;
    QSqlDatabase *database;
    QString usbPort = "/dev/ttyACM0";

    void initSerialReader();
};
#endif // MAINWINDOW_H
