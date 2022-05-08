#ifndef SERIALOPTIONS_H
#define SERIALOPTIONS_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>


namespace Ui {
class SerialOptions;
}

class SerialOptions : public QDialog
{
    Q_OBJECT

public:
    explicit SerialOptions(QWidget *parent = nullptr);
    ~SerialOptions();
    static QString getFirstPortName();

signals:
    void baudrateChanged(qint64 &new_rate);


private slots:
    void handle_accepted();

private:
    Ui::SerialOptions *ui;
    void initPortOptions();
    void initBaudRateOptions();

    qint64 default_baudrate = 9600;
    qint64 user_baudrate = -1;
    static constexpr std::initializer_list<qint64> all_rates = {QSerialPort::Baud1200,
                                                                QSerialPort::Baud2400,
                                                                QSerialPort::Baud4800,
                                                                QSerialPort::Baud9600,
                                                                QSerialPort::Baud19200,
                                                                QSerialPort::Baud38400,
                                                                QSerialPort::Baud57600,
                                                                QSerialPort::Baud115200};
};

#endif // SERIALOPTIONS_H
