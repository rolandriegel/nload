
#ifndef DEVREADER_LINUX_H
#define DEVREADER_LINUX_H

#include <string>
#include <list>

class DevReaderLinux
{
    public:
        static bool isAvailable();
        static std::list<std::string> findAllDevices();
};

#endif

