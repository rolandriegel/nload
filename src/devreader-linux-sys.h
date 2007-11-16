
#ifndef DEVREADER_LINUX_SYS_H
#define DEVREADER_LINUX_SYS_H

#include "devreader.h"

#include <string>
#include <list>

class DevReaderLinuxSys : public DevReader
{
    public:
        DevReaderLinuxSys(const std::string& deviceName);
        virtual ~DevReaderLinuxSys();

        static bool isAvailable();
        static std::list<std::string> findAllDevices();
        
    protected:
        void readFromDevice(DataFrame& dataFrame);

    private:
        unsigned long readULongSysEntry(const std::string& entry);
};

#endif

