#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QCloseEvent>
#include <QTextBrowser>
#include <QMessageBox>
#include <QSharedPointer>
#include <QSqlDatabase>

#include "../TimeIsMoneyInvoice_CMake/invoice.hpp"
#include "../TimeIsMoneyTask_CMake/task.hpp"
#include "../TimeIsMoneyTask_CMake/tasklist.hpp"
#include "../TimeIsMoneySerial_CMake/serialoptions.hpp"
#include "../TimeIsMoneySerial_CMake/serialreader_qt.hpp"


namespace Ui
{
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSharedPointer<QSqlDatabase> &&database, QWidget *parent = nullptr);
    ~MainWindow() override;


private slots:
    void runCmd(const std::string &val);
    void startTask();
    void cancelTask();
    void clientname_changed(const QString &val);
    void taskname_changed(const QString &val);
    void price_changed(const QString &val);
    void taskinfo_changed();
    void closeEvent(QCloseEvent *event) override;
    void usbPort_Changed(const QString &val);
    void usbPort_ChangeDialogClosed();

    void on_actionOverview_triggered();
    void on_actionCreate_Invoice_triggered();
    void on_actionPort_triggered();
    void on_actionInfo_triggered();
    void on_startTaskBtn_clicked();
    void on_stopTaskBtn_clicked();
    void informationMessageBoxClosed();

    void on_actionCheck_for_Updates_triggered();

private:
    Ui::MainWindow *ui;
    void newTaskDialog();
    QSharedPointer<Task> currentTask;
    QSharedPointer<TaskList> taskList;
    QSharedPointer<QTimer> serialTimer;
    QSharedPointer<QTimer> infoTimer;
    QSharedPointer<Invoice> invoice;
    QSharedPointer<SerialOptions> serialOptions;
    QSharedPointer<SerialReader_QT> serialReaderQt;
    QSharedPointer<QSqlDatabase> database;
    QString usbPort;
    std::string start_UUID = "d3b3ecc2-ced7-461a-ac96-04f6d99d9d34";
    std::string end_UUID = "2e11f26e-d155-42d7-be96-d8dec1e6c69e";
    qint8 remaing_stop_calls = 3;
    qint8 progress_value = 0;
    qint8 running_hours = 0;
    void initSerialReader();
    void initSerialOptions();
    void setLabelFontBold();
    bool msg_box_open = false;
    void displayInformationMessage(const QString &task_name);
    QMessageBox *msgBox{};
};
#endif // MAINWINDOW_HPP
