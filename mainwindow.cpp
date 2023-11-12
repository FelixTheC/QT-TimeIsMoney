#include "ui_mainwindow.h"
#include <QBoxLayout>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QInputDialog>
#include <QLabel>
#include <QSqlDatabase>
#include <QTimer>

#include "mainwindow.hpp"
#include "updater.hpp"


MainWindow::MainWindow(QSharedPointer<QSqlDatabase> &&database, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);

    this->database = database;

    infoTimer = QSharedPointer<QTimer>(new QTimer(this));
    connect(infoTimer.data(), &QTimer::timeout, this, &MainWindow::taskinfo_changed);

    setLabelFontBold();

    initSerialOptions();
    initSerialReader();

    taskList = QSharedPointer<TaskList>(new TaskList(database));
    invoice = QSharedPointer<Invoice>(new Invoice(database, this));
    externalApi = QSharedPointer<ExternalApi>(new ExternalApi(database, this));
    externalApiManager = QSharedPointer<QNetworkAccessManager>(new QNetworkAccessManager());
    
    connect(ui->actionAdd_API, SIGNAL(triggered()), this, SLOT(actionAdd_API_triggered()));
    connect(externalApiManager.data(), &QNetworkAccessManager::finished, this, &MainWindow::handleExternalApiResponse);
}

MainWindow::~MainWindow()
{
    serialTimer->stop();
    infoTimer->stop();

    delete ui;
}

void
MainWindow::initSerialReader()
{
    serialReaderQt = QSharedPointer<SerialReader_QT>(new SerialReader_QT(usbPort));
    if (serialReaderQt->isOpen())
    {
        connect(serialReaderQt.data(),
                &SerialReader_QT::serialValueReceived,
                this,
                &MainWindow::runCmd);

        if (serialOptions)
        {
            connect(serialOptions.data(),
                    &SerialOptions::baudrateChanged,
                    serialReaderQt.data(),
                    &SerialReader_QT::changeBaudrate);

            connect(serialOptions.data(),
                    &SerialOptions::portChanged,
                    serialReaderQt.data(),
                    &SerialReader_QT::changePort);
        }
    }
}

void
MainWindow::initSerialOptions()
{
    usbPort = SerialOptions::getFirstPortName();
    serialOptions = QSharedPointer<SerialOptions>(new SerialOptions(this->database, this));
}

void
MainWindow::runCmd(const std::string &val)
{
    if (val.length() > 0)
    {
        if (val.find(start_UUID) != std::string::npos)
        {
            if (!(currentTask))
            {
                currentTask = QSharedPointer<Task>(new Task(database));
            }
            
            if (!is_task_window_open)
            {
                is_task_window_open = true;
                newTaskDialog();
            }
        }

        if (val.find(end_UUID) != std::string::npos)
        {
            --remaing_stop_calls;
            if (remaing_stop_calls <=0)
            {
                cancelTask();
                is_task_window_open = false;
                remaing_stop_calls = 3;
            }
        }
    }
}

void
MainWindow::startTask()
{
    if (this->currentTask && this->is_paused_)
    {
        this->currentTask->startTask();
        send_cmd(this->database, externalApiManager, SendTopic::Start);
        ui->progressBar->setVisible(true);
        
        // set one value back as it will be increased in taskinfo_changed
        --progress_value;

        infoTimer->start(60000);
        taskinfo_changed();
        
        this->is_paused_ = false;
        ui->startTaskBtn->setText("Pause");
    }
    else if (this->currentTask && this->currentTask->is_running())
    {
        this->currentTask->stopTask();
        this->infoTimer->stop();
        send_cmd(this->database, externalApiManager, SendTopic::Pause);
        
        this->is_paused_ = true;
        ui->progressBar->setFormat("Paused");
        ui->startTaskBtn->setText("Start");
    }
}

void
MainWindow::cancelTask()
{
    if (currentTask && !currentTask->getCreatedAt().isEmpty())
    {
        currentTask->stopTask();
        send_cmd(this->database, externalApiManager, SendTopic::Stop);
        
        auto task_name = currentTask->getTaskName();
        displayInformationMessage(task_name);

        taskinfo_changed();
        ui->startTaskBtn->setText("Start");
        this->is_paused_ = false;
    }
    else if (!(currentTask))
    {
        taskinfo_changed();
    }

    ui->progressBar->setFormat("0h %vmin");
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

void MainWindow::setLabelFontBold()
{
    auto bold = QFont();
    bold.setBold(true);

    this->ui->label_currentclient->setFont(bold);
    this->ui->label_currenttask->setFont(bold);
    this->ui->label_taskstarted->setFont(bold);
    this->ui->label_moneyearned->setFont(bold);
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
        if (is_paused_)
        {
            ui->progressBar->setFormat("0h %vmin");
        }
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
    auto task = QSharedPointer<Task>(new Task(database));

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
    auto *clientSelect = new QComboBox(widget);

    auto client_names = task->getUsedClientNames();
    clientSelect->addItem("");
    std::for_each(client_names.begin(),
                  client_names.end(),
                  [&clientSelect](auto name){clientSelect->addItem(name);});


    auto *taskLabel = new QLabel(widget);
    taskLabel->setText("Task:");
    auto *taskField = new QLineEdit(widget);
    taskField->setText(lastTask);
    auto *taskSelect = new QComboBox(widget);

    auto task_names = task->getUsedTaskNames();
    taskSelect->addItem("");
    std::for_each(task_names.begin(),
                  task_names.end(),
                  [&taskSelect](auto name){taskSelect->addItem(name);});

    auto *priceLabel = new QLabel(widget);
    priceLabel->setText("Price per Hour:");
    auto *priceField = new QLineEdit(widget);
    priceField->setText(lastPrice);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto *hLayoutClient = new QHBoxLayout();
    hLayoutClient->addWidget(clientLabel);
    hLayoutClient->addWidget(clientField);
    hLayoutClient->addWidget(clientSelect);

    auto *hLayoutTask = new QHBoxLayout();
    hLayoutTask->addWidget(taskLabel);
    hLayoutTask->addWidget(taskField);
    hLayoutTask->addWidget(taskSelect);

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
    connect(clientSelect, &QComboBox::currentTextChanged, clientField, &QLineEdit::setText);
    connect(clientSelect, &QComboBox::currentTextChanged, this, &MainWindow::clientname_changed);
    connect(clientField, &QLineEdit::textEdited, this, &MainWindow::clientname_changed);
    connect(taskField, &QLineEdit::textEdited, this, &MainWindow::taskname_changed);
    connect(taskSelect, &QComboBox::currentTextChanged, taskField, &QLineEdit::setText);
    connect(taskSelect, &QComboBox::currentTextChanged, this, &MainWindow::taskname_changed);
    connect(priceField, &QLineEdit::textEdited, this, &MainWindow::price_changed);
    connect(widget, &QDialog::accepted, this, &MainWindow::startTask);
    connect(widget, &QDialog::rejected, this, &MainWindow::cancelTask);

    if (is_paused_)
    {
        widget->setModal(true);
        widget->show();
    }
    else
    {
        startTask();
    }
}


void
MainWindow::on_actionOverview_triggered()
{
    taskList->setModal(true);
    taskList->show();
}


void
MainWindow::on_actionCreate_Invoice_triggered()
{
    invoice->show();
}


void
MainWindow::on_actionPort_triggered()
{
    if (SerialOptions::getFirstPortName().isEmpty())
    {
        QMessageBox::warning(this, "No Serialdevice", "No Serialdevice connected.");
    }
    else
    {
        serialOptions->show();
    }
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

void
MainWindow::informationMessageBoxClosed()
{
    msg_box_open = false;
}

void
MainWindow::displayInformationMessage(const QString &task_name)
{
    if (!msg_box_open)
    {
        msgBox = QSharedPointer<QMessageBox>(new QMessageBox(QMessageBox::Information,
                                 "Task stopped.",
                                 "Task [" + task_name + "]\nwas stopped",
                                 QMessageBox::Ok| QMessageBox::Cancel,
                                 this));
        connect(msgBox.data(), &QMessageBox::accepted, this, &MainWindow::informationMessageBoxClosed);
        msg_box_open = true;
        msgBox->show();
    }
}

void MainWindow::on_actionCheck_for_Updates_triggered()
{
   new Updater();
}

void MainWindow::actionAdd_API_triggered()
{
    externalApi->show();
}

void MainWindow::handleExternalApiResponse(QNetworkReply *networkReply)
{
    auto statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    
    if (statusCode.isValid())
    {
        auto status = statusCode.toInt();
        
        if (status != 200)
        {
            auto json_doc = QJsonDocument::fromJson(networkReply->readAll());
            
            msgBox = QSharedPointer<QMessageBox>(new QMessageBox(QMessageBox::Warning,
                                                                 "Invalid response",
                                                                 json_doc.toJson(QJsonDocument::Compact),
                                                                 QMessageBox::Ok| QMessageBox::Cancel,
                                                                 this));
            msgBox->show();
        }
    }
}

