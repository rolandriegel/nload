
#ifndef DEVREADER_HPUX_H
#define DEVREADER_HPUX_H

#include "devreader.h"

#include <string>
#include <list>

class DevReaderHpux : public DevReader
{
    public:
        DevReaderHpux(const std::string& deviceName);
        virtual ~DevReaderHpux();

        static std::list<std::string> findAllDevices();

    protected:
        void readFromDevice(DataFrame& dataFrame);
};

#endif

