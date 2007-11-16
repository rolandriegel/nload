/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include "devreader-linux.h"
#include "devreader-linux-proc.h"
#include "devreader-linux-sys.h"

#include <string>
#include <list>

using namespace std;

bool DevReaderLinux::isAvailable()
{
    return DevReaderLinuxSys::isAvailable() || DevReaderLinuxProc::isAvailable();
}

list<string> DevReaderLinux::findAllDevices()
{
    if(DevReaderLinuxSys::isAvailable())
        return DevReaderLinuxSys::findAllDevices();
    else if(DevReaderLinuxProc::isAvailable())
        return DevReaderLinuxProc::findAllDevices();
    else
        return list<string>();
}

