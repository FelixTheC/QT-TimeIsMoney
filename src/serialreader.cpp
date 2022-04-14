#include "serialreader.h"
#include "serialreader_utils.h"
#include <QDebug>
#include <QString>


SerialReader::SerialReader()
{
    options.c_cflag |= (CLOCAL | CREAD);
    fileDescriptor = open(filePath.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
}

SerialReader::~SerialReader()
{
    fileDescriptor = cserial::cclose(fileDescriptor);
}

void
SerialReader::setBaudRate(const unsigned int &rate) noexcept
{
    baudRate = rate;
    speed_t newRate;
    try
    {
        newRate = getSerialSpeed(rate);
    }
    catch (std::invalid_argument &err)
    {
        return;
    }

    cfsetispeed(&options, newRate);
    cfsetospeed(&options, newRate);
}

void
SerialReader::setFilePath(const std::string &path) noexcept
{
    filePath = path;
}

std::string
SerialReader::readSerial()
{
    tcgetattr(fileDescriptor, &options);

    /*
     * Set the new options for the port...
     */
    tcsetattr(fileDescriptor, TCSANOW, &options);
    setBaudRate(baudRate);

    if (fileDescriptor == -1)
    {
        //qWarning("open_port: Unable to open /dev/ttyACM0 - ");
        return "";
    }
    else
    {
        size_t result = 0;
        char buf[38];
        std::string text;

        // FNDELAY option causes the read function to return 0 if no characters are available on the port.
        fcntl(fileDescriptor, F_SETFL, FNDELAY);
        // normal (blocking) behavior
        // fcntl(fd, F_SETFL, 0);

        while (result < SIZE_MAX)
        {
            result = read(fileDescriptor, &buf, sizeof(buf) - 1);
            if (result == 37)  // 36 + 1 newline char @TODO must be a const variable
            {
                text = buf;
                break;
            }
            buf[result] = 0x00;
        }
        return text;
    }
}

void SerialReader::getSerialValue()
{
    auto result = readSerial();
    result.erase(remove_if(result.begin(), result.end(), isspace), result.end());
    emit serialValueReceived(result);
}

speed_t
SerialReader::getSerialSpeed(const unsigned int &rate)
{
    speed_t speed;
    switch (rate) {
    case 230400:
        speed = B230400;
        break;

    case 115200:
        speed = B115200;
        break;

    case 57600:
        speed = B57600;
        break;

    case 38400:
        speed = B38400;
        break;

    case 19200:
        speed = B19200;
        break;

    case 9600:
        speed = B9600;
        break;

    case 4800:
        speed = B4800;
        break;

    case 2400:
        speed = B2400;
        break;

    case 1800:
        speed = B1800;
        break;

    case 1200:
        speed = B1200;
        break;

    case 600:
        speed = B600;
        break;

    case 300:
        speed = B300;
        break;

    default:
        throw std::invalid_argument("Unsupported baudrate.");
    }
    return speed;
}
