
#ifndef DEVREADER_H
#define DEVREADER_H

#include <string>

#include "dataframe.h"

class DevReader
{
    public:
        DevReader(const std::string& deviceName);
        virtual ~DevReader();

        const std::string& getDeviceName() const;
        DataFrame getNewDataFrame();

    protected:
        virtual void readFromDevice(DataFrame& dataFrame) = 0;
        std::string getDeviceIp4Address();
        
        std::string m_deviceName;
};

#endif

