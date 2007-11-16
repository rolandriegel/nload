
#ifndef DEVREADER_SOLARIS_H
#define DEVREADER_SOLARIS_H

#include "devreader.h"

#include <string>
#include <list>

class DevReaderSolaris : public DevReader
{
    public:
        DevReaderSolaris(const std::string& deviceName);
        virtual ~DevReaderSolaris();

        static std::list<std::string> findAllDevices();

    protected:
        void readFromDevice(DataFrame& dataFrame);
};

#endif

