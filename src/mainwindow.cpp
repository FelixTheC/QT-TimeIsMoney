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
    free(serialReader);
    free(database);
    if (currentTask != nullptr)
        free(currentTask);

    delete ui;
}

void
MainWindow::initSerialReader()
{
    serialReader = new SerialReader(usbPort.toStdString());
    serialTimer = new QTimer(this);
    connect(serialTimer, &QTimer::timeout, serialReader, &SerialReader::getSerialValue);
    connect(serialReader, &SerialReader::serialValueReceived, this, &MainWindow::runCmd);
    serialTimer->start(100);
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
            cancelTask();
        }
    }
}

void
MainWindow::startTask()
{
    this->currentTask->startTask();
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
    auto *dialog = new QDialog(this);
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto *label = new QLabel(dialog);
    label->setText("Client:");
    auto *edit = new QLineEdit(dialog);
    edit->setText(usbPort);

    auto *hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    hLayout->addWidget(edit);

    auto *boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, dialog);
    boxLayout->addLayout(hLayout);
    boxLayout->addWidget(buttonBox);


    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    connect(edit, &QLineEdit::textChanged, this, &MainWindow::usbPort_Changed);
    connect(dialog, &QDialog::accepted, this, &MainWindow::usbPort_ChangeDialogClosed);

    dialog->setModal(true);
    dialog->show();
}

void
MainWindow::usbPort_Changed(const QString &val)
{
    usbPort = val;
}

void MainWindow::usbPort_ChangeDialogClosed()
{
    serialTimer->stop();
    initSerialReader();
}


void MainWindow::on_pushButton_clicked()
{
    runCmd(start_UUID);
}


void MainWindow::on_pushButton_2_clicked()
{
    runCmd(end_UUID);
}

