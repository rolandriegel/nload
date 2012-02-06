/***************************************************************************
                             devreaderfactory.cpp
                             --------------------
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

#include "devreaderfactory.h"

#include "config.h"

#include "devreader.h"
#include "devreader-bsd.h"
#include "devreader-hpux.h"
#include "devreader-linux-proc.h"
#include "devreader-linux-sys.h"
#include "devreader-linux.h"
#include "devreader-solaris.h"

#include <string>
#include <list>
#include <map>

using namespace std;

map<string, DevReader*> DevReaderFactory::m_devReaders;
DevReaderFactory DevReaderFactory::m_instance;

DevReaderFactory::DevReaderFactory()
{
}

DevReaderFactory::~DevReaderFactory()
{
    for(map<string, DevReader*>::const_iterator it = m_devReaders.begin(); it != m_devReaders.end(); ++it)
        delete it->second;

    m_devReaders.clear();
}

int DevReaderFactory::findAllDevices()
{
#if defined HAVE_BSD
    list<string> interfaceNames = DevReaderBsd::findAllDevices();
#elif defined HAVE_HPUX
    list<string> interfaceNames = DevReaderHpux::findAllDevices();
#elif defined HAVE_LINUX
    list<string> interfaceNames = DevReaderLinux::findAllDevices();
#elif defined HAVE_SOLARIS
    list<string> interfaceNames = DevReaderSolaris::findAllDevices();
#endif

    map<string, DevReader*>::iterator devReaderIt = m_devReaders.begin();
    while(devReaderIt != m_devReaders.end())
    {
        list<string>::iterator interfaceIt = interfaceNames.begin();
        list<string>::iterator interfaceItEnd = interfaceNames.end();
        
        while(*interfaceIt != devReaderIt->first && interfaceIt != interfaceItEnd)
            ++interfaceIt;

        // delete all devices which disappeared
        if(interfaceIt == interfaceItEnd)
        {
            delete devReaderIt->second;
            m_devReaders.erase(devReaderIt++);
        }
        // delete all entries in the interface name list which we know of already
        else
        {
            interfaceNames.erase(interfaceIt);
            devReaderIt++;
        }
    }
    
    // the interface name list now contains only devices which just appeared in the system
    for(list<string>::const_iterator it = interfaceNames.begin(); it != interfaceNames.end(); ++it)
    {
        DevReader* newReader = createDevReader(*it);
        if(newReader)
            m_devReaders[*it] = newReader;
    }
    
    return m_devReaders.size();
}

int DevReaderFactory::getDeviceCount()
{
    return m_devReaders.size();
}

const map<string, DevReader*>& DevReaderFactory::getAllDevReaders()
{
    return m_devReaders;
}

DevReader* DevReaderFactory::createDevReader(const string& deviceName)
{
    DevReader* reader = 0;
    
#if defined HAVE_BSD
    reader = new DevReaderBsd(deviceName);
#elif defined HAVE_HPUX
    reader = new DevReaderHpux(deviceName);
#elif defined HAVE_LINUX
    if(DevReaderLinuxSys::isAvailable())
        reader = new DevReaderLinuxSys(deviceName);
    else if(DevReaderLinuxProc::isAvailable())
        reader = new DevReaderLinuxProc(deviceName);
    else
        reader = 0;
#elif defined HAVE_SOLARIS
    reader = new DevReaderSolaris(deviceName);
#endif

    return reader;
}

