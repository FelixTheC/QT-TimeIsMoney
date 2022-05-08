#include "serialreader_qt.h"
#include "qdebug.h"

SerialReader_QT::SerialReader_QT()
{
    auto port_info = QSerialPortInfo(usb_port);

    serial_port = new QSerialPort("ttyACM0");

    connect(serial_port, &QSerialPort::readyRead, this, &SerialReader_QT::read_line);

    if (serial_port->open(QIODevice::ReadOnly))
    {
        serial_port->setBaudRate(QSerialPort::BaudRate::Baud9600);
        serial_port->setParity(QSerialPort::EvenParity);
        serial_port->setDataBits(QSerialPort::Data7);   // receiving ASCII data
    }
}

SerialReader_QT::~SerialReader_QT()
{
    serial_port->close();
}

void
SerialReader_QT::read_line()
{
    received_uuid.append(serial_port->read(8));
    std::string uuid = received_uuid.toStdString();
    if (uuid.size() == 36)
    {
        emit serialValueReceived(uuid);
    }
    if (uuid.size() == 37)
        received_uuid.clear();
}

void
SerialReader_QT::changeBaudrate(qint64 &val)
{
    if (serial_port != nullptr && serial_port->isOpen())
    {
        serial_port->setBaudRate(val);
    }
}
