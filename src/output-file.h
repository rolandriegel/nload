/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2018 by Roland Riegel <feedback@roland-riegel.de>
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

#ifndef OUTPUT_FILE_H
#define OUTPUT_FILE_H

#include "output.h"

#include <iosfwd>
#include <string>
#include <vector>

class AppControl;
class Device;

class OutputFile : public Output
{
    public:
        OutputFile(AppControl* appControl, const std::vector<Device*>& devices, const std::string& fileName);
        virtual ~OutputFile();

        virtual void process() {}
        virtual bool processSignal(int signal) { return false; }

        virtual void output();

    private:
        std::string formatTimeStamp(unsigned long seconds, unsigned long microseconds);

    private:
        AppControl* m_appControl;
        const std::vector<Device*>& m_devices;
        std::ostream* m_stream;
};

#endif

