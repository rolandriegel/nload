/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * HP-UX specific network code                                             *
 *                                                                         *
 * Copyright 1998 by Hewlett-Packard Company                               *
 *                                                                         *
 * Permission to use, copy, modify, and distribute this                    *
 * software and its documentation for any purpose and without              *
 * fee is hereby granted, provided that the above copyright                *
 * notice appear in all copies and that both that copyright                *
 * notice and this permission notice appear in supporting                  *
 * documentation, and that the name of Hewlett-Packard Company not         *
 * be used in advertising or publicity pertaining to distribution          *
 * of the software without specific, written prior permission.             *
 * Hewlett-Packard makes no representations about the                      *
 * suitability of this software for any purpose.  It is provided           *
 * "as is" without express or implied warranty.                            *
 *                                                                         *
 *                                                                         *
 * Modification History:                                                   *
 *                                                                         *
 * int_stats.c : Get network statistics from the dlpi device               *
 * On 10.20 only get a mib_ifEntry, on 11.0 this is followed               *
 * by an mib_Dot3StatsEntry. Accessing the latter structure                *
 * is left as an exercise for the reader, the struct is defined            *
 * in /usr/include/sys/mib.h along with mib_ifEntry.                       *
 *                                                                         *
 * Author: Jon Dewis, SPP 3/26/98                                          *
 * (With ackn to 'lanadmin' source code and streams class notes)           *
 *                                                                         *
 * 24-June-1998    Add mib3 stats (AJD) when compiling                     *
 *                 for 11.0 ensure HP_UX11 macro defined                   *
 *                 e.g. cc -o int_stats -DHP_UX11 int_stats.c              *
 * 16-Dec-1999     Change to obviate need for lanscan to get               *
 *                 nmid - thus assumes any old nmid will do                *
 *                 and just iterates thru the 1st 100 possibilities        *
 *                 (MAX_NMID). Works on 10.20 and 11.0                     *
 * Dec 2003        Modified by Roshan Sequeira roshan.sequeira@hp.com,     *
 *                 to get network statistics for the nload port to HP-UX.  *
 *                 Original code available at                              *
 *                 http://h21007.www2.hp.com/dspp/tech/tech_TechDocumentDetailPage_IDX/1,1701,2599,00.html
 *                                                                         *
 ***************************************************************************/

#include "devreader-hpux.h"
#include "stringutils.h"

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

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
#include <fcntl.h>
#include <sys/stropts.h>
#include <sys/dlpi.h>
#include <sys/dlpi_ext.h>
#include <sys/mib.h>

#include <string>
#include <list>

using namespace std;

#define AREA_SIZE 40000
#define LONG_AREA_SIZE  (AREA_SIZE / sizeof(u_long))

static u_long ctrl_area[LONG_AREA_SIZE];        /* for control messages */
static u_long data_area[LONG_AREA_SIZE];        /* for data messages */
static u_long ppa_area[LONG_AREA_SIZE];         /* for saving ppa area */
static struct strbuf ctrl_buf = {AREA_SIZE, 0, (char*) ctrl_area};
static struct strbuf data_buf = {AREA_SIZE, 0, (char*) data_area};

DevReaderHpux::DevReaderHpux(const string& deviceName)
    : DevReader(deviceName)
{
}

DevReaderHpux::~DevReaderHpux()
{
}

list<string> DevReaderHpux::findAllDevices()
{
    list<string> devicesFound;
    
    dl_hp_ppa_info_t* ppa_info_temp;
    dl_hp_ppa_req_t* ppa_req;
    dl_hp_ppa_ack_t* ppa_ack;
    dl_attach_req_t* attach_req;
    
    int fd = -1;
    if((fd = open("/dev/dlpi", O_RDWR)) < 0)
    {
        perror("open /dev/dlpi");
        return devicesFound;
    }
    
    do
    {
        ppa_req = (dl_hp_ppa_req_t*) ctrl_area;
        ppa_ack = (dl_hp_ppa_ack_t*) ctrl_area;
        
        ppa_req->dl_primitive = DL_HP_PPA_REQ;
        
        ctrl_buf.len = sizeof(dl_hp_ppa_req_t);
        if(putmsg(fd, &ctrl_buf, 0, 0) < 0)
        {
            perror("putmsg DL_HP_PPA_REQ");
            break;
        }
        
        int flags = 0;
        ctrl_area[0] = 0;
        
        if(getmsg(fd, &ctrl_buf, &data_buf, &flags) < 0)
        {
            perror("getmsg DL_HP_PPA_REQ");
            break;
        }
        
        if(ppa_ack->dl_length == 0)
        {
            fprintf(stderr, "error: no PPAs available\n");
            break;
        }
        
        // save all the PPA information
        memcpy((u_char*) ppa_area, (u_char*) ctrl_area + ppa_ack->dl_offset, ppa_ack->dl_length);
        int ppa_count = ppa_ack->dl_count;
        
        int count;
        for(count = 0, ppa_info_temp = (dl_hp_ppa_info_t*) ppa_area; count < ppa_count; count++, ppa_info_temp++)
            devicesFound.push_back(string("lan") + toString(ppa_info_temp->dl_ppa));

    } while(0);

    close(fd);

    return devicesFound;
}

void DevReaderHpux::readFromDevice(DataFrame& dataFrame)
{
    dl_get_statistics_req_t* get_statistics_req = (dl_get_statistics_req_t*) ctrl_area;
    dl_get_statistics_ack_t* get_statistics_ack = (dl_get_statistics_ack_t*) ctrl_area;

    dl_hp_ppa_info_t ppa_info;
    dl_hp_ppa_info_t* ppa_info_temp;
    dl_hp_ppa_req_t* ppa_req;
    dl_hp_ppa_ack_t* ppa_ack;
    dl_attach_req_t* attach_req;
    mib_ifEntry* mib_ptr;
    
    unsigned int ppa = 0;
    int flags = 0, ppa_count = 0, count = 0;
    
    // make sure interface name begins with lan
    if(m_deviceName.length() < 3 || m_deviceName.substr(0, 3) != "lan")
        return;
    
    // get the PPA from the interface name
    ppa = fromString<int>(m_deviceName.substr(3));
    if(ppa == 0 && m_deviceName != "lan0")
        return;
    
    int fd = -1;
    if((fd = open("/dev/dlpi", O_RDWR)) < 0)
    {
        perror("open /dev/dlpi");
        return;
    }
    
    do
    {
        ppa_req = (dl_hp_ppa_req_t*) ctrl_area;
        ppa_ack = (dl_hp_ppa_ack_t*) ctrl_area;
        
        ppa_req->dl_primitive = DL_HP_PPA_REQ;
        
        ctrl_buf.len = sizeof(dl_hp_ppa_req_t);
        if(putmsg(fd, &ctrl_buf, 0, 0) < 0)
        {
            perror("putmsg DL_HP_PPA_REQ");
            break;
        }
        
        flags = 0;
        ctrl_area[0] = 0;
        
        if(getmsg(fd, &ctrl_buf, &data_buf, &flags) < 0)
        {
            perror("getmsg DL_HP_PPA_REQ");
            break;
        }
        
        if(ppa_ack->dl_length == 0)
        {
            fprintf(stderr, "Error: No PPAs available\n");
            break;
        }
        
        // save all the PPA information
        memcpy((u_char*) ppa_area, (u_char*) ctrl_area + ppa_ack->dl_offset, ppa_ack->dl_length);
        ppa_count = ppa_ack->dl_count;
        
        bool found = false;
        for(count = 0, ppa_info_temp = (dl_hp_ppa_info_t*) ppa_area; count < ppa_count; count++, ppa_info_temp++)
        {
            if(ppa_info_temp->dl_ppa == ppa)
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            fprintf(stderr, "error: PPA %d not found\n", ppa);
            break;
        }
        
        attach_req = (dl_attach_req_t*) ctrl_area;
        attach_req->dl_primitive = DL_ATTACH_REQ;
        attach_req->dl_ppa = ppa;
        ctrl_buf.len = sizeof(dl_attach_req_t);
        if(putmsg(fd, &ctrl_buf, 0, 0) < 0)
        {
            perror("putmsg DL_ATTACH_REQ");
            break;
        }
        
        ctrl_area[0] = 0;
        if(getmsg(fd, &ctrl_buf, &data_buf, &flags) < 0)
        {
            perror("getmsg DL_ATTACH_REQ");
            break;
        }
        memcpy(&ppa_info, ppa_info_temp, sizeof(ppa_info));
        
        get_statistics_req->dl_primitive = DL_GET_STATISTICS_REQ;
        ctrl_buf.len = sizeof(dl_get_statistics_req_t);
        flags = 0;
        
        if(putmsg(fd, &ctrl_buf, NULL, 0) < 0)
        {
            perror("putmsg DL_GET_STATISTICS_REQ");
            break;
        }
        
        if(getmsg(fd, &ctrl_buf, NULL, &flags) < 0)
        {
            perror("getmsg DL_GET_STATISTICS_REQ");
            break;
        }
        if(get_statistics_ack->dl_primitive != DL_GET_STATISTICS_ACK)
            fprintf(stderr, "error: wrong primitive\n");

        mib_ptr = (mib_ifEntry*) ((u_char*) ctrl_area + get_statistics_ack->dl_stat_offset);

        dataFrame.setTotalDataIn(mib_ptr->ifInOctets);
        dataFrame.setTotalDataOut(mib_ptr->ifOutOctets);

        dataFrame.setTotalPacketsIn(mib_ptr->ifInUcastPkts + mib_ptr->ifInNUcastPkts);
        dataFrame.setTotalPacketsOut(mib_ptr->ifOutUcastPkts + mib_ptr->ifOutNUcastPkts);

        dataFrame.setTotalErrorsIn(mib_ptr->ifInErrors);
        dataFrame.setTotalErrorsOut(mib_ptr->ifOutErrors);

        dataFrame.setTotalDropsIn(mib_ptr->ifInDiscards);
        dataFrame.setTotalDropsOut(mib_ptr->ifOutDiscards);

        dataFrame.setValid(true);

    } while(0);

    close(fd);
}

