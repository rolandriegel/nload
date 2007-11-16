/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include "devreader-linux-sys.h"

#include <string>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

DevReaderLinuxSys::DevReaderLinuxSys(const string& deviceName)
    : DevReader(deviceName)
{
}

DevReaderLinuxSys::~DevReaderLinuxSys()
{
}

bool DevReaderLinuxSys::isAvailable()
{
    struct stat sysStat;
    if(stat("/sys/class/net", &sysStat) < 0 || ! S_ISDIR(sysStat.st_mode))
        return false;

    return true;
}

list<string> DevReaderLinuxSys::findAllDevices()
{
    list<string> interfaceNames;
    DIR* sysDir = opendir("/sys/class/net");
    struct dirent* sysDirEntry = 0;

    if(!sysDir)
        return interfaceNames;

    while((sysDirEntry = readdir(sysDir)))
    {
        string interfaceName(sysDirEntry->d_name);
        
        if(interfaceName[0] == '.')
            continue;
                
        interfaceNames.push_back(interfaceName);
    }

    closedir(sysDir);

    return interfaceNames;
}

void DevReaderLinuxSys::readFromDevice(DataFrame& dataFrame)
{
    string devPath = "/sys/class/net/";
    devPath += m_deviceName;
    
    struct stat sysStat;
    if(stat(devPath.c_str(), &sysStat) < 0 || ! S_ISDIR(sysStat.st_mode))
        return;

    dataFrame.setTotalDataIn(readULongSysEntry("statistics/rx_bytes"));
    dataFrame.setTotalDataOut(readULongSysEntry("statistics/tx_bytes"));

    dataFrame.setTotalPacketsIn(readULongSysEntry("statistics/rx_packets"));
    dataFrame.setTotalPacketsOut(readULongSysEntry("statistics/tx_packets"));

    dataFrame.setTotalErrorsIn(readULongSysEntry("statistics/rx_errors"));
    dataFrame.setTotalErrorsOut(readULongSysEntry("statistics/tx_errors"));
    
    dataFrame.setTotalDropsIn(readULongSysEntry("statistics/rx_dropped"));
    dataFrame.setTotalDropsOut(readULongSysEntry("statistics/tx_dropped"));
    
    dataFrame.setValid(true);
}

unsigned long DevReaderLinuxSys::readULongSysEntry(const string& entry)
{
    string sysEntryPath = "/sys/class/net/";
    sysEntryPath += m_deviceName;
    sysEntryPath += '/';
    sysEntryPath += entry;
    
    FILE* sysEntry;
    if(!(sysEntry = fopen(sysEntryPath.c_str(), "r")))
        return 0;
    
    char content[32];
    size_t contentLength = 0;
    contentLength = fread(content, sizeof(content[0]), sizeof(content) / sizeof(content[0]), sysEntry);
    fclose(sysEntry);

    if(contentLength > 0)
    {
        unsigned long num = 0;
     
        if(contentLength >= sizeof(content) / sizeof(content[0]))
            contentLength = sizeof(content) / sizeof(content[0]) - 1;
        
        content[contentLength] = 0;
        num = strtoul(content, 0, 10);

        return num;
    }
    else
    {
        return 0;
    }
}


