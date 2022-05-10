#include "serialoptions.h"
#include "ui_serialoptions.h"

SerialOptions::SerialOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialOptions)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SerialOptions::handle_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SerialOptions::close);
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &SerialOptions::handlePortChanged);

    initPortOptions();
    initBaudRateOptions();
}

SerialOptions::~SerialOptions()
{
    delete ui;
}


QString
SerialOptions::getFirstPortName()
{
    auto available_ports = QSerialPortInfo::availablePorts();
    if (available_ports.isEmpty())
        return "";
    else
        return available_ports.first().portName();
}

void
SerialOptions::initPortOptions()
{
    auto availablePorts = QSerialPortInfo::availablePorts();
    std::for_each(availablePorts.begin(),
                  availablePorts.end(),
                  [&](const QSerialPortInfo &portInfo){
                     ui->comboBox->addItem(portInfo.portName());
                  });
    if (!availablePorts.empty())
    {
        if (selected_port_name.isEmpty())
        {
            selected_port_name = availablePorts[0].portName();
        }
        else
        {
            auto text_index = ui->comboBox->findText(selected_port_name);
            ui->comboBox->setCurrentIndex(text_index);
        }
    }
}

void
SerialOptions::initBaudRateOptions()
{
    std::for_each(all_rates.begin(),
                  all_rates.end(),
                  [&](const qint64 &rate){
                      ui->comboBox_2->addItem(QString::number(rate));
                  });

    if (user_baudrate == -1)
    {
        auto current_index = ui->comboBox_2->findText(QString::number(default_baudrate));
        ui->comboBox_2->setCurrentIndex(current_index);
    }
    else
    {
        auto current_index = ui->comboBox_2->findText(QString::number(user_baudrate));
        ui->comboBox_2->setCurrentIndex(current_index);
    }

}

void
SerialOptions::handle_accepted()
{
    qint64 new_rate = ui->comboBox_2->itemText(ui->comboBox_2->currentIndex()).toInt();
    if (new_rate != default_baudrate && new_rate != user_baudrate)
    {
        user_baudrate = new_rate;
        emit baudrateChanged(new_rate);
    }
}

void
SerialOptions::handlePortChanged(const QString &val)
{
    selected_port_name = val;
    emit portChanged(selected_port_name);
}
