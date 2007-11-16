/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * Solaris specific network code taken out of gkrellm                      *
 * Copyright by Bill Wilson <bill@gkrellm.net>                             *
 * Solaris code by Daisuke Yabuki <dxy@acm.org>                            *
 * adapted for nload by Roland Riegel <feedback@roland-riegel.de>          *
 ***************************************************************************/

#include "devreader-solaris.h"

#include <string.h>
#include <kstat.h>
#include <sys/socket.h>
#include <net/if.h>

#include <string>
#include <list>

using namespace std;

DevReaderSolaris::DevReaderSolaris(const string& deviceName)
    : DevReader(deviceName)
{
}

DevReaderSolaris::~DevReaderSolaris()
{
}

list<string> DevReaderSolaris::findAllDevices()
{
    list<string> interfaceNames;
    struct if_nameindex* interfaces = if_nameindex();

    if(!interfaces)
        return interfaceNames;

    int i = 0;
    while(interfaces[i].if_name)
    {
        interfaceNames.push_back(string(interfaces[i].if_name));
        ++i;
    }

    if_freenameindex(interfaces);

    return interfaceNames;
}

void DevReaderSolaris::readFromDevice(DataFrame& dataFrame)
{
    kstat_ctl_t *kc;
    kstat_t *ksp;
    kstat_named_t *knp;
    char* devName = 0;

    if(m_deviceName.empty())
        return;
    
    devName = (char*) malloc(m_deviceName.length() + 1);
    strncpy(devName, m_deviceName.c_str(), m_deviceName.length());
    devName[m_deviceName.length()] = '\0';
    
    kc = kstat_open();
    ksp = kstat_lookup(kc, NULL, -1, devName);
    if(ksp && kstat_read(kc, ksp, NULL) >= 0)
    {
        knp = (kstat_named_t *) kstat_data_lookup(ksp, "rbytes");
        dataFrame.setTotalDataIn(knp ? knp->value.ui32 : 0);

        knp = (kstat_named_t *) kstat_data_lookup(ksp, "obytes");
        dataFrame.setTotalDataOut(knp ? knp->value.ui32 : 0);

        knp = (kstat_named_t *) kstat_data_lookup(ksp, "ipackets");
        dataFrame.setTotalPacketsIn(knp ? knp->value.ui32 : 0);
        
        knp = (kstat_named_t *) kstat_data_lookup(ksp, "opackets");
        dataFrame.setTotalPacketsOut(knp ? knp->value.ui32 : 0);
        
        knp = (kstat_named_t *) kstat_data_lookup(ksp, "ierrors");
        dataFrame.setTotalErrorsIn(knp ? knp->value.ui32 : 0);
        
        knp = (kstat_named_t *) kstat_data_lookup(ksp, "oerrors");
        dataFrame.setTotalErrorsOut(knp ? knp->value.ui32 : 0);
        
        dataFrame.setValid(true);
    }

    kstat_close(kc);
    free(devName);
    devName = 0;
}

