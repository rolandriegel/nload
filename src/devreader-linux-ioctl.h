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

#ifndef DEVREADER_LINUX_IOCTL_H
#define DEVREADER_LINUX_IOCTL_H

#include "devreader.h"

#include <string>
#include <list>

class DevReaderLinuxIoctl : public DevReader
{
    public:
        DevReaderLinuxIoctl(const std::string& deviceName);
        virtual ~DevReaderLinuxIoctl();

        static bool isAvailable();
        
    protected:
        void readFromDevice(DataFrame& dataFrame);

    private:
        bool init();

        int m_fd;
        int m_n_stats;
        int m_datain_id;
        int m_dataout_id;
        int m_packetin_id;
        int m_packetout_id;
        int m_errorsin_id;
        int m_errorsout_id;
        int m_dropsin_id;
        int m_dropsout_id;
};

#endif


