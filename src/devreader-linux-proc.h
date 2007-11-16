
#ifndef DEVREADER_LINUX_PROC_H
#define DEVREADER_LINUX_PROC_H

#include "devreader.h"

#include <string>
#include <list>

class DevReaderLinuxProc : public DevReader
{
    public:
        DevReaderLinuxProc(const std::string& deviceName);
        virtual ~DevReaderLinuxProc();

        static bool isAvailable();
        static std::list<std::string> findAllDevices();
        
    protected:
        void readFromDevice(DataFrame& dataFrame);
};

#endif


