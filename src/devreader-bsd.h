
#ifndef DEVREADER_BSD_H
#define DEVREADER_BSD_H

#include "devreader.h"

#include <string>
#include <list>

class DevReaderBsd : public DevReader
{
    public:
        DevReaderBsd(const std::string& deviceName);
        virtual ~DevReaderBsd();

        static std::list<std::string> findAllDevices();
    
    protected:
        void readFromDevice(DataFrame& dataFrame);
};

#endif

