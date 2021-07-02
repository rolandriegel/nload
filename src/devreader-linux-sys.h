/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2012 by Roland Riegel <feedback@roland-riegel.de>
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
        static std::list<std::string> findAllDevices_ib();
        unsigned long long readULongSysEntry(const std::string& entry);
        void readFromDevice_nic(DataFrame& dataFrame);
        unsigned long long readULongSysEntry_ib(const std::string& entry);
        void readFromDevice_ib(DataFrame& dataFrame);
        static std::string ibname_of(const std::string&);
        static std::string portname_of(const std::string&);
};

#endif

