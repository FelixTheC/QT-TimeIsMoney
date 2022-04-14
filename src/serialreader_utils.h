#ifndef SERIALREADER_UTILS_H
#define SERIALREADER_UTILS_H

#include <unistd.h> // write(), read(), close()
namespace cserial
{
    int cclose(int fileDescriptor)
    {
        return close(fileDescriptor);
    }
}

#endif // SERIALREADER_UTILS_H
