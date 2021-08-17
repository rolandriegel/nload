/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2018 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "devreader-linux-sys.h"

#include <fstream>
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
    struct stat sysStat;

    if(!sysDir)
        return interfaceNames;

    while((sysDirEntry = readdir(sysDir)))
    {
        string interfaceName(sysDirEntry->d_name);
        
        if(interfaceName[0] == '.')
            continue;

        if(stat(("/sys/class/net/" + interfaceName).c_str(), &sysStat) < 0 ||
           !S_ISDIR(sysStat.st_mode))
            continue;
                
        interfaceNames.push_back(interfaceName);
    }

    closedir(sysDir);

    list<string> ibs = findAllDevices_ib();
    interfaceNames.splice(interfaceNames.end(), ibs);

    return interfaceNames;
}

list<string> DevReaderLinuxSys::findAllDevices_ib()
{
    list<string> interfaceNames;
    DIR* sysDir = opendir("/sys/class/infiniband");
    struct dirent* sysDirEntry = 0;
    struct stat sysStat;

    if(!sysDir)
        return interfaceNames;

    while((sysDirEntry = readdir(sysDir)))
    {
        string interfaceName(sysDirEntry->d_name);

        if(interfaceName[0] == '.')
            continue;

        if(stat(("/sys/class/infiniband/" + interfaceName + "/counters").c_str(), &sysStat) == 0 &&
                S_ISDIR(sysStat.st_mode))
            interfaceNames.push_back("ib:" + interfaceName);

        DIR* portsDir = opendir(("/sys/class/infiniband/" + interfaceName + "/ports").c_str());
        struct dirent* portsDirEntry = 0;
        if (!portsDir)
            continue;
        while((portsDirEntry = readdir(portsDir)))
        {
            string portname(portsDirEntry->d_name);
            if(portname[0] == '.')
                continue;

            if(stat(("/sys/class/infiniband/" + interfaceName + "/ports/"+ portname + "/counters").c_str(), &sysStat) < 0 ||
                    !S_ISDIR(sysStat.st_mode))
                continue;
            interfaceNames.push_back("ib:" + interfaceName + ":" + portname);
        }

    }

    closedir(sysDir);

    return interfaceNames;
}

void DevReaderLinuxSys::readFromDevice(DataFrame& dataFrame)
{
    if (m_deviceName.substr(0,3) == "ib:") {
        readFromDevice_ib(dataFrame);
    } else {
        readFromDevice_nic(dataFrame);
    }
}

void DevReaderLinuxSys::readFromDevice_ib(DataFrame& dataFrame)
{
    string devPath = "/sys/class/infiniband/";
    devPath += ibname_of(m_deviceName);
    if (!portname_of(m_deviceName).empty()) {
        devPath += "/ports/";
        devPath += portname_of(m_deviceName);
    }

    struct stat sysStat;
    if(stat(devPath.c_str(), &sysStat) < 0 || ! S_ISDIR(sysStat.st_mode))
        return;

    // magic number *4 here
    dataFrame.setTotalDataIn(readULongSysEntry_ib("counters/port_rcv_data")*4);
    dataFrame.setTotalDataOut(readULongSysEntry_ib("counters/port_xmit_data")*4);

    dataFrame.setTotalPacketsIn(readULongSysEntry_ib("counters/port_rcv_packets"));
    dataFrame.setTotalPacketsOut(readULongSysEntry_ib("counters/port_xmit_packets"));

    dataFrame.setTotalErrorsIn(readULongSysEntry_ib("counters/port_rcv_errors"));
    //dataFrame.setTotalErrorsOut(readULongSysEntry_ib(""));

    //dataFrame.setTotalDropsIn(readULongSysEntry_ib(""));
    dataFrame.setTotalDropsOut(readULongSysEntry_ib("counters/port_xmit_discards"));

    dataFrame.setValid(true);
}

unsigned long long DevReaderLinuxSys::readULongSysEntry_ib(const string& entry)
{
    string sysEntryPath = "/sys/class/infiniband/";
    sysEntryPath += ibname_of(m_deviceName);
    if (!portname_of(m_deviceName).empty()) {
        sysEntryPath += "/ports/";
        sysEntryPath += portname_of(m_deviceName);
    }
    sysEntryPath += '/';
    sysEntryPath += entry;

    ifstream sysEntry(sysEntryPath.c_str());
    if(!sysEntry.is_open())
        return 0;

    unsigned long long num = 0;
    sysEntry >> num;
    if(sysEntry.fail())
        return 0;

    return num;
}

void DevReaderLinuxSys::readFromDevice_nic(DataFrame& dataFrame)
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

unsigned long long DevReaderLinuxSys::readULongSysEntry(const string& entry)
{
    string sysEntryPath = "/sys/class/net/";
    sysEntryPath += m_deviceName;
    sysEntryPath += '/';
    sysEntryPath += entry;
    
    ifstream sysEntry(sysEntryPath.c_str());
    if(!sysEntry.is_open())
        return 0;

    unsigned long long num = 0;
    sysEntry >> num;
    if(sysEntry.fail())
        return 0;

    return num;
}

// extract "mlx5_0" from "ib:mlx5_0:1"
std::string DevReaderLinuxSys::ibname_of(const std::string& devname)
{
    std::string r = devname;
    if (devname.substr(0,3) == "ib:")
            r.erase(0,3);
    size_t pos = r.rfind(':');
    if (pos != std::string::npos) {
        r.erase(pos);
    }
    return r;
}

// extract "1" from "ib:mlx5_0:1"
std::string DevReaderLinuxSys::portname_of(const std::string& devname)
{
    std::string r = devname;
    if (devname.substr(0,3) == "ib:")
            r.erase(0,3);
    size_t pos = r.rfind(':');
    if (pos != std::string::npos) {
        return r.substr(pos+1);
    } else
        return std::string();
}

