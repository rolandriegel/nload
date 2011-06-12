/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * Free/Net/OpenBSD specific network code taken out of gkrellm:            *
 * Copyright by Bill Wilson <bill@gkrellm.net>                             *
 * FreeBSD code by Hajimu Umemoto <ume@mahoroba.org>                       *
 * NetBSD code by Anthony Mallet <anthony.mallet@useless-ficus.net>        *
 * Hajimu Umemoto merged Free/Net/OpenBSD code                             *
 * adapted for nload by Roland Riegel <feedback@roland-riegel.de>          *
 ***************************************************************************/

#include "devreader-bsd.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <net/if_dl.h>
#include <net/route.h>

#include <string>
#include <list>

using namespace std;

DevReaderBsd::DevReaderBsd(const string& deviceName)
    : DevReader(deviceName)
{
}

DevReaderBsd::~DevReaderBsd()
{
}

list<string> DevReaderBsd::findAllDevices()
{
    list<string> interfaceNames;

    struct if_msghdr *ifm, *nextifm;
    struct sockaddr_dl *sdl;
    char *lim, *next;
    size_t needed;
    int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0};
    char *buf = 0;
    
    if(sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
        return interfaceNames;
            
    buf = (char *) malloc(needed);
    if(buf == NULL)
        return interfaceNames;
            
    if(sysctl(mib, 6, buf, &needed, NULL, 0) < 0)
        return interfaceNames;
            
    lim = buf + needed;
            
    next = buf;
    while(next < lim)
    {
        ifm = (struct if_msghdr *) next;
        if(ifm->ifm_type != RTM_IFINFO)
            break;
                    
        next += ifm->ifm_msglen;
                    
        while(next < lim)
        {
            nextifm = (struct if_msghdr *) next;
            if(nextifm->ifm_type != RTM_NEWADDR)
                break;
            next += nextifm->ifm_msglen;
        }
                    
        sdl = (struct sockaddr_dl *) (ifm + 1);
        if(sdl->sdl_family != AF_LINK)
            continue;
        
        interfaceNames.push_back(string(sdl->sdl_data));
    }

    free(buf);

    return interfaceNames;
}

void DevReaderBsd::readFromDevice(DataFrame& dataFrame)
{
    struct if_msghdr *ifm, *nextifm;
    struct sockaddr_dl *sdl;
    char *lim, *next;
    size_t needed;
    int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0};
    char *buf = 0;
    
    if(m_deviceName.empty())
        return;
	
    do
    {
        if(sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
            break;
		
        buf = (char *) malloc(needed);
        if(buf == NULL)
            break;
		
        if(sysctl(mib, 6, buf, &needed, NULL, 0) < 0)
            break;
		
        lim = buf + needed;
		
        next = buf;
        while(next < lim)
        {
            ifm = (struct if_msghdr *) next;
            if(ifm->ifm_type != RTM_IFINFO)
                break;
			
            next += ifm->ifm_msglen;
			
            while(next < lim)
            {
                nextifm = (struct if_msghdr *) next;
                if(nextifm->ifm_type != RTM_NEWADDR)
                    break;
                next += nextifm->ifm_msglen;
            }
			
            if(ifm->ifm_flags & IFF_UP)
            {
                sdl = (struct sockaddr_dl *) (ifm + 1);
                if(sdl->sdl_family != AF_LINK)
                    continue;
                    
                if(strncmp(m_deviceName.c_str(), sdl->sdl_data, sdl->sdl_nlen) == 0)
                {
                    dataFrame.setTotalDataIn(ifm->ifm_data.ifi_ibytes);
                    dataFrame.setTotalDataOut(ifm->ifm_data.ifi_obytes);

                    dataFrame.setTotalPacketsIn(ifm->ifm_data.ifi_ipackets);
                    dataFrame.setTotalPacketsOut(ifm->ifm_data.ifi_opackets);

                    dataFrame.setTotalErrorsIn(ifm->ifm_data.ifi_ierrors);
                    dataFrame.setTotalErrorsOut(ifm->ifm_data.ifi_oerrors);

                    dataFrame.setTotalDropsIn(0); // ifi_iqdrops ?
                    dataFrame.setTotalDropsOut(0);

                    dataFrame.setValid(true);

                    break;
                }
            }
        }
    } while(0);
    
    free(buf);
}

