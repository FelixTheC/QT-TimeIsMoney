#include "serialreader_qt.h"
#include "qdebug.h"

SerialReader_QT::SerialReader_QT()
{
    initSerialReading();
}

SerialReader_QT::SerialReader_QT(const QString &port_name)
{
    usb_port = port_name;
    initSerialReading();
}

SerialReader_QT::~SerialReader_QT()
{
    serial_port->close();
}

void
SerialReader_QT::initSerialReading()
{
    serial_port = new QSerialPort(usb_port);
    // first close the port, makes it easier to re-open
    serial_port->close();

    connect(serial_port, &QSerialPort::readyRead, this, &SerialReader_QT::read_line);

    if (serial_port->open(QIODevice::ReadOnly))
    {
        serial_port->setBaudRate(QSerialPort::BaudRate::Baud9600);
        serial_port->setParity(QSerialPort::EvenParity);
        serial_port->setDataBits(QSerialPort::Data7);   // receiving ASCII data
        open = true;
    }
}

void
SerialReader_QT::read_line()
{
    QByteArray raw_bytes = serial_port->read(64);
    QByteArray bytes {};

    if (raw_bytes.size() == 38)
    {
        std::transform(raw_bytes.begin(),
                       raw_bytes.end() - 2,
                       std::back_inserter(bytes),
                       [](auto chr){return chr;}
                       );
        std::string uuid = bytes.toStdString();
        emit serialValueReceived(uuid);
    }
    else if (raw_bytes.size() == 1)
    {
        received_uuid.append(raw_bytes);
        std::string uuid = received_uuid.toStdString();
        if (uuid.size() == 36)
            emit serialValueReceived(uuid);

        if (uuid.size() >= 37)
            received_uuid.clear();
    }
}

void
SerialReader_QT::changeBaudrate(qint64 &val)
{
    if (serial_port != nullptr && serial_port->isOpen())
    {
        serial_port->setBaudRate(val);
    }
}

bool
SerialReader_QT::isOpen()
{
    return open;
}
