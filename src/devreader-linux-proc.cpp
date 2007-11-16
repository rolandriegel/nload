/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * Linux specific network code taken out of knetload:                      *
 * Copyright by Markus Gustavsson <mighty@fragzone.se>                     *
 * adapted for nload by Roland Riegel <feedback@roland-riegel.de>          *
 ***************************************************************************/

#include "devreader-linux-proc.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <list>

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
    
    FILE* fd = 0;
    char buf[512] = "";
    char dev[128] = "";
    char* tmp = 0;
    char* tmp2 = 0;

    if(!(fd = fopen("/proc/net/dev", "r")))
        return interfaceNames;
    
    // skip the two header lines
    fgets(buf, sizeof(buf), fd);
    fgets(buf, sizeof(buf), fd);
    
    while(fgets(buf, sizeof(buf), fd))
    {
        memset(dev, 0, sizeof(dev));
        
        tmp = buf;
        tmp2 = dev;
       
        // skip whitespace
        while(*tmp == ' ')
            tmp++;
        // extract device name
        while((*tmp2++ = *tmp++) != ':');
        *--tmp2 = '\0';
    
        interfaceNames.push_back(string(dev));
    }
    
    fclose(fd);

    return interfaceNames;
}

void DevReaderLinuxProc::readFromDevice(DataFrame& dataFrame)
{
    FILE* fd = 0;
    char buf[512] = "";
    char dev[128] = "";
    char* tmp = 0;
    char* tmp2 = 0;

    if(m_deviceName.empty())
        return;
    
    do
    {
        if(!(fd = fopen("/proc/net/dev", "r")))
            break;
        
        // skip the two header lines
        fgets(buf, sizeof(buf), fd);
        fgets(buf, sizeof(buf), fd);
        
        while(fgets(buf, sizeof(buf), fd))
        {
            memset(dev, 0, sizeof(dev));
            
            tmp = buf;
            tmp2 = dev;
           
            // skip whitespace
            while(*tmp == ' ')
                tmp++;
            // extract device name
            while((*tmp2++ = *tmp++) != ':');
            *--tmp2 = '\0';
            
            // check if it is the device we want
            if(strcmp(m_deviceName.c_str(), dev) == 0)
            {
                unsigned long bytesIn = 0;
                unsigned long packetsIn = 0;
                unsigned long errorsIn = 0;
                unsigned long dropsIn = 0;
                unsigned long bytesOut = 0;
                unsigned long packetsOut = 0;
                unsigned long errorsOut = 0;
                unsigned long dropsOut = 0;
                
                // read device data
                sscanf(tmp, "%lu %lu %lu %lu %*u %*u %*u %*u %lu %lu %lu %lu",
                   &bytesIn,
                   &packetsIn,
                   &errorsIn,
                   &dropsIn,
                   &bytesOut,
                   &packetsOut,
                   &errorsOut,
                   &dropsOut
                );

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
        
        fclose(fd);

    } while(0);
}

