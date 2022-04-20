#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions

#include <unistd.h> // write(), read(), close()
#include <string>
#include <stdexcept>

#include <QMessageBox>


class SerialReader : public QWidget
{
    Q_OBJECT

public:
    SerialReader();
    SerialReader(const std::string &path);
    ~SerialReader();

    void setBaudRate(const unsigned int &rate) noexcept;
    void setFilePath(const std::string &path) noexcept;

Q_SIGNALS:
    void serialValueReceived(std::string &val);

public Q_SLOTS:
    void getSerialValue();

private:
    int fileDescriptor;
    unsigned int baudRate = 9600;
    std::string filePath = "/dev/ttyACM0";

    speed_t getSerialSpeed(const unsigned int &rate) noexcept(false);

    struct termios options{};
    std::string readSerial();
};

#endif // SERIALREADER_H
