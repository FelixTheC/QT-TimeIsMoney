#include "invoice.h"
#include "mainwindow.h"
#include "tasklist.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

MainWindow::MainWindow(QSqlDatabase *database, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);

    this->database = database;
    initSerialReader();

    infoTimer = new QTimer(this);
    connect(infoTimer, &QTimer::timeout, this, &MainWindow::taskinfo_changed);
    infoTimer->start(60000);
}

MainWindow::~MainWindow()
{
    serialTimer->stop();
    infoTimer->stop();

    free(serialTimer);
    free(infoTimer);
    free(serialReaderQt);
    free(database);
    if (currentTask != nullptr)
        free(currentTask);

    delete ui;
}

void
MainWindow::initSerialReader()
{
    serialReaderQt = new SerialReader_QT(SerialOptions::getFirstPortName());
    connect(serialReaderQt, &SerialReader_QT::serialValueReceived, this, &MainWindow::runCmd);

    if (serialOptions != nullptr)
        connect(serialOptions, &SerialOptions::baudrateChanged, serialReaderQt, &SerialReader_QT::changeBaudrate);
}

void
MainWindow::runCmd(const std::string &val)
{
    if (val.length() > 0)
    {
        if (val.find(start_UUID) != std::string::npos)
        {
            if (currentTask == nullptr)
            {
                currentTask = new Task(database);
                newTaskDialog();
            }
        }

        if (val.find(end_UUID) != std::string::npos)
        {
            --remaing_stop_calls;
            if (remaing_stop_calls <=0)
            {
                cancelTask();
                remaing_stop_calls = 3;
            }
        }
    }
}

void
MainWindow::startTask()
{
    this->currentTask->startTask();
    ui->progressBar->setVisible(true);
    progress_value = -1;
    taskinfo_changed();
}

void
MainWindow::cancelTask()
{
    if (currentTask != nullptr && !currentTask->getCreatedAt().isEmpty())
    {
        currentTask->stopTask();
        free(this->currentTask);
        this->currentTask = nullptr;
        taskinfo_changed();
    }
    else if (currentTask != nullptr)
    {
        free(this->currentTask);
        this->currentTask = nullptr;
        taskinfo_changed();
    }

    ui->progressBar->setVisible(false);
    progress_value = 0;
    running_hours = 0;
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
    cancelTask();
    event->accept();
}

void
MainWindow::clientname_changed(const QString &val)
{
    currentTask->setClientName(val.toStdString());
}

void
MainWindow::taskname_changed(const QString &val)
{
    currentTask->setTaskName(val.toStdString());
}

void
MainWindow::price_changed(const QString &val)
{

    if (val.length() > 0)
    {
        bool *ok = new bool();
        float price = val.toFloat(ok);
        if (*ok)
            currentTask->setPricePerHour(price);
    }
}

void
MainWindow::taskinfo_changed()
{
    if (currentTask == nullptr)
    {
        this->ui->label_currentclient->setText("");
        this->ui->label_currenttask->setText("");
        this->ui->label_taskstarted->setText("");
        this->ui->label_moneyearned->setText("");
    }
    else
    {
        this->ui->label_currentclient->setText(currentTask->getClientName());
        this->ui->label_currenttask->setText(currentTask->getTaskName());
        this->ui->label_taskstarted->setText(currentTask->getCreatedAt());
        auto price = currentTask->priceUntilNow();
        this->ui->label_moneyearned->setText(QString::number(price));
    }

    /*
     * reset stop_calls if required
     * if a user hit's the 2 times button maybe by accident then
     * we will reset the value in our clock cycle
     * to avoid a real stop call after only one push
     */
    if (remaing_stop_calls != 3)
        remaing_stop_calls = 3;

    if (ui->progressBar->isVisible())
    {
        ++progress_value;
        if (progress_value > 60)
        {
            progress_value = 0;
            ++running_hours;
            ui->progressBar->setFormat(QString::number(running_hours) + "h %vmin");
        }

        ui->progressBar->setValue(progress_value);
    }

}

void
MainWindow::newTaskDialog()
{
    auto *widget = new QDialog(this);
    QString lastClient = "";
    QString lastTask = "";
    QString lastPrice = "";

    Task::getLastRecord(lastClient, lastTask, lastPrice, database);
    currentTask->setClientName(lastClient.toStdString());
    currentTask->setTaskName(lastTask.toStdString());
    currentTask->setPricePerHour(lastPrice.toFloat());

    auto *clientLabel = new QLabel(widget);
    clientLabel->setText("Client:");
    auto *clientField = new QLineEdit(widget);
    clientField->setText(lastClient);

    auto *taskLabel = new QLabel(widget);
    taskLabel->setText("Task:");
    auto *taskField = new QLineEdit(widget);
    taskField->setText(lastTask);

    auto *priceLabel = new QLabel(widget);
    priceLabel->setText("Price per Hour:");
    auto *priceField = new QLineEdit(widget);
    priceField->setText(lastPrice);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto *hLayoutClient = new QHBoxLayout();
    hLayoutClient->addWidget(clientLabel);
    hLayoutClient->addWidget(clientField);

    auto *hLayoutTask = new QHBoxLayout();
    hLayoutTask->addWidget(taskLabel);
    hLayoutTask->addWidget(taskField);

    auto *hLayoutPrice = new QHBoxLayout();
    hLayoutPrice->addWidget(priceLabel);
    hLayoutPrice->addWidget(priceField);

    auto *boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, widget);
    boxLayout->addItem(hLayoutClient);
    boxLayout->addItem(hLayoutTask);
    boxLayout->addItem(hLayoutPrice);
    boxLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, widget, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MainWindow::cancelTask);
    connect(buttonBox, &QDialogButtonBox::rejected, widget, &QDialog::reject);
    connect(clientField, &QLineEdit::textEdited, this, &MainWindow::clientname_changed);
    connect(taskField, &QLineEdit::textEdited, this, &MainWindow::taskname_changed);
    connect(priceField, &QLineEdit::textEdited, this, &MainWindow::price_changed);
    connect(widget, &QDialog::accepted, this, &MainWindow::startTask);
    connect(widget, &QDialog::rejected, this, &MainWindow::cancelTask);

    widget->setModal(true);
    widget->show();
}

void
MainWindow::on_actionOverview_triggered()
{
    TaskList *taskList = new TaskList(database);
    taskList->setModal(true);
    taskList->show();
}


void
MainWindow::on_actionCreate_Invoice_triggered()
{
    Invoice *invoiceView = new Invoice(database, this);
    invoiceView->show();
}


void
MainWindow::on_actionPort_triggered()
{
    serialOptions = new SerialOptions(this);
    serialOptions->show();
}

void
MainWindow::usbPort_Changed(const QString &val)
{
    usbPort = val;
}

void
MainWindow::usbPort_ChangeDialogClosed()
{
    serialTimer->stop();
    initSerialReader();
}

void
MainWindow::on_startTaskBtn_clicked()
{
    runCmd(start_UUID);
}

void
MainWindow::on_stopTaskBtn_clicked()
{
    cancelTask();
}


void
MainWindow::on_actionInfo_triggered()
{
    QFile file(":/info.txt");
    file.open(QFile::ReadOnly | QFile::Text);

    auto *dialog = new QDialog(this);
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    auto *textBrowser = new QTextBrowser(dialog);
    textBrowser->setText(file.readAll());

    auto *boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, dialog);
    boxLayout->addWidget(textBrowser);
    boxLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    dialog->setModal(true);
    dialog->show();
}
