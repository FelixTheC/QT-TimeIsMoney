#ifndef SERIALREADER_QT_H
#define SERIALREADER_QT_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <qwidget.h>


class SerialReader_QT : public QWidget
{
    Q_OBJECT

public:
    SerialReader_QT();
    ~SerialReader_QT();

Q_SIGNALS:
    void serialValueReceived(std::string &val);

public Q_SLOTS:
    void read_line();
    void changeBaudrate(qint64 &val);

private:
    QString usb_port = "/dev/ttyACM0";
    QSerialPort *serial_port;
    QByteArray received_uuid = QByteArray();
};

#endif // SERIALREADER_QT_H
