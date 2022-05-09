#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialoptions.h"
#include "task.h"
#include "taskdb.h"
#include "serialreader_qt.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QTextBrowser>


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
    void on_actionInfo_triggered();
    void on_startTaskBtn_clicked();
    void on_stopTaskBtn_clicked();

private:
    Ui::MainWindow *ui;
    void newTaskDialog();
    Task *currentTask = nullptr;
    QTimer *serialTimer;
    QTimer *infoTimer;
    SerialReader_QT *serialReaderQt;
    SerialOptions *serialOptions;
    QSqlDatabase *database;
    QString usbPort;
    std::string start_UUID = "d3b3ecc2-ced7-461a-ac96-04f6d99d9d34";
    std::string end_UUID = "2e11f26e-d155-42d7-be96-d8dec1e6c69e";
    qint8 remaing_stop_calls = 3;
    qint8 progress_value = 0;
    qint8 running_hours = 0;
    void initSerialReader();
    void setLabelFontBold();
};
#endif // MAINWINDOW_H
