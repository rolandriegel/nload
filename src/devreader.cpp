/***************************************************************************
                                devreader.cpp
                             -------------------
    begin                : Fri Nov 16 2007
    copyright            : (C) 2007 - 2012 by Roland Riegel
    email                : feedback@roland-riegel.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "devreader.h"

#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_SOLARIS
    #include <sys/sockio.h>
#endif

using namespace std;

DevReader::DevReader(const string& deviceName)
    : m_deviceName(deviceName)
{
}

DevReader::~DevReader()
{
}

const string& DevReader::getDeviceName() const
{
    return m_deviceName;
}

DataFrame DevReader::getNewDataFrame()
{
    DataFrame deviceData;

    struct timeval tv;
    gettimeofday(&tv, 0);
    
    readFromDevice(deviceData);
    
    deviceData.setDeviceName(m_deviceName);

    deviceData.setTimeStampSeconds((unsigned long) tv.tv_sec);
    deviceData.setTimeStampMicroseconds((unsigned long) tv.tv_usec);

    deviceData.setIpV4(getDeviceIp4Address());

    return deviceData;
}

string DevReader::getDeviceIp4Address()
{
    struct ifreq ifr;
    int sk;
    
    string deviceIp = "";
    
    if(m_deviceName.empty())
        return deviceIp;
    
    /* create a temporary socket: ioctl needs one */
    if((sk = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return deviceIp;
    
    /* copy the device name into the ifreq structure */
    strncpy(ifr.ifr_name, m_deviceName.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    
    /* make the request */
    if(!ioctl(sk, SIOCGIFADDR, &ifr))
    {
        struct sockaddr_in sin;
        memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
        
        /* only use the IP number if the address family is really IPv4 */
        if(sin.sin_family == AF_INET)
            deviceIp = inet_ntoa(sin.sin_addr);
    }
    
    /* close the temporary socket */
    close(sk);
    
    return deviceIp;
}

