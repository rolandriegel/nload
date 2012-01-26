/***************************************************************************
                             devreader-linux-sys.h
                             ---------------------
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

#ifndef DEVREADER_LINUX_SYS_H
#define DEVREADER_LINUX_SYS_H

#include "devreader.h"

#include <string>
#include <list>

class DevReaderLinuxSys : public DevReader
{
    public:
        DevReaderLinuxSys(const std::string& deviceName);
        virtual ~DevReaderLinuxSys();

        static bool isAvailable();
        static std::list<std::string> findAllDevices();
        
    protected:
        void readFromDevice(DataFrame& dataFrame);

    private:
        unsigned long long readULongSysEntry(const std::string& entry);
};

#endif

