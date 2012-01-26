/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include "devreader-linux-proc.h"
#include "stringutils.h"

#include <fstream>
#include <list>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

DevReaderLinuxProc::DevReaderLinuxProc(const string& deviceName)
    : DevReader(deviceName)
{
}

DevReaderLinuxProc::~DevReaderLinuxProc()
{
}

bool DevReaderLinuxProc::isAvailable()
{
    struct stat procStat;
    if(stat("/proc/net/dev", &procStat) < 0 || ! S_ISREG(procStat.st_mode))
        return false;

    return true;
}

list<string> DevReaderLinuxProc::findAllDevices()
{
    list<string> interfaceNames;
    
    ifstream fin("/proc/net/dev");
    if(!fin.is_open())
        return interfaceNames;
    
    // skip the two header lines
    string line;
    getline(fin, line);
    getline(fin, line);

    if(!fin.good())
        return interfaceNames;

    // read all remaining lines and extract the device name
    while(fin.good())
    {
        getline(fin, line);
        if(line.empty())
            continue;

        string::size_type posEnd = line.find(':');
        if(posEnd == string::npos)
            continue;

        interfaceNames.push_back(trim(line.substr(0, posEnd)));
    }
    
    return interfaceNames;
}

void DevReaderLinuxProc::readFromDevice(DataFrame& dataFrame)
{
    if(m_deviceName.empty())
        return;
    
    ifstream fin("/proc/net/dev");
    if(!fin.is_open())
        return;
    
    // skip the two header lines
    string line;
    getline(fin, line);
    getline(fin, line);

    if(!fin.good())
        return;

    // search for device
    while(fin.good())
    {
        getline(fin, line);
        if(line.empty())
            continue;

        string::size_type posEnd = line.find(':');
        if(posEnd == string::npos)
            continue;

        string interfaceName = trim(line.substr(0, posEnd));
        if(interfaceName.empty())
            continue;

        // check if it is the device we want
        if(m_deviceName != interfaceName)
            continue;

        // read device data
        unsigned long long bytesIn = 0;
        unsigned long long packetsIn = 0;
        unsigned long long errorsIn = 0;
        unsigned long long dropsIn = 0;
        unsigned long long bytesOut = 0;
        unsigned long long packetsOut = 0;
        unsigned long long errorsOut = 0;
        unsigned long long dropsOut = 0;
        unsigned long long dummy = 0;

        istringstream sin(trim(line.substr(posEnd + 1)));

        sin >> bytesIn
            >> packetsIn
            >> errorsIn
            >> dropsIn
            >> dummy
            >> dummy
            >> dummy
            >> dummy
            >> bytesOut
            >> packetsOut
            >> errorsOut
            >> dropsOut;

        if(sin.fail())
            break;
        
        dataFrame.setTotalDataIn(bytesIn);
        dataFrame.setTotalDataOut(bytesOut);

        dataFrame.setTotalPacketsIn(packetsIn);
        dataFrame.setTotalPacketsOut(packetsOut);

        dataFrame.setTotalErrorsIn(errorsIn);
        dataFrame.setTotalErrorsOut(errorsOut);
        
        dataFrame.setTotalDropsIn(dropsIn);
        dataFrame.setTotalDropsOut(dropsOut);
        
        dataFrame.setValid(true);
        
        break;
    }
}

