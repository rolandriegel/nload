
#ifndef DEVREADERFACTORY_H
#define DEVREADERFACTORY_H

#include <string>
#include <map>

class DevReader;

class DevReaderFactory
{
    public:
        DevReaderFactory();
        ~DevReaderFactory();

        static int findAllDevices();
        static int getDeviceCount();
        
        static const std::map<std::string, DevReader*>& getAllDevReaders();

    private:
        static DevReader* createDevReader(const std::string& deviceName);
        
        static std::map<std::string, DevReader*> m_devReaders;
        static DevReaderFactory m_instance;
};

#endif

