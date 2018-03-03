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

#include "output-file.h"

#include "appcontrol.h"
#include "device.h"
#include "setting.h"
#include "settingfilter.h"
#include "settingstore.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <time.h>

using namespace std;

OutputFile::OutputFile(AppControl* appControl, const vector<Device*>& devices, const string& fileName)
    : Output(), m_appControl(appControl), m_devices(devices)
{
    if(fileName == "-")
    {
        m_stream = &cout;
    }
    else
    {
        ofstream* fileStream = new ofstream(fileName.c_str(), ios_base::out | ios_base::app);
        if(fileStream->is_open() == false)
        {
            delete fileStream;
            fileStream = 0;

            cout << "Failed to open file '" << fileName << "' for writing." << endl;
        }

        m_stream = fileStream;
    }

    if(m_stream)
    {
        (*m_stream) << "Date Time "
                    << "DeviceName "
                    << "DataInTotal "
                    << "DataOutTotal "
                    << "DataInPerSecond "
                    << "DataOutPerSecond "
                    << "DataInAverage "
                    << "DataOutAverge "
                    << "DataInMin "
                    << "DataOutMin "
                    << "DataInMax "
                    << "DataOutMax "
                    << "TimeSeconds "
                    << "TimeMicroSeconds "
                    << endl;
    }
}

OutputFile::~OutputFile()
{
    if(m_stream != &cout)
        delete m_stream;
    m_stream = 0;
}

void OutputFile::output()
{
    if(!m_stream)
        return;

    for(vector<Device*>::const_iterator itDevice = m_devices.begin(); itDevice != m_devices.end(); ++itDevice)
    {
        Device* device = *itDevice;

        if(device->exists())
        {
            const Statistics& statistics = device->getStatistics();
            unsigned long timeStampSeconds = statistics.getLatestTimeStampSeconds();
            unsigned long timeStampMicroseconds = statistics.getLatestTimeStampMicroseconds();

            (*m_stream) << formatTimeStamp(timeStampSeconds, timeStampMicroseconds) << ' '
                        << device->getName() << ' '
                        << statistics.getDataInTotal() << ' '
                        << statistics.getDataOutTotal() << ' '
                        << statistics.getDataInPerSecond() << ' '
                        << statistics.getDataOutPerSecond() << ' '
                        << statistics.getDataInAverage() << ' '
                        << statistics.getDataOutAverage() << ' '
                        << statistics.getDataInMin() << ' '
                        << statistics.getDataOutMin() << ' '
                        << statistics.getDataInMax() << ' '
                        << statistics.getDataOutMax() << ' '
                        << timeStampSeconds << ' '
                        << timeStampMicroseconds << ' '
                        << endl;
        }
    }
}

string OutputFile::formatTimeStamp(unsigned long seconds, unsigned long microseconds)
{
    time_t ts = (time_t) seconds;
    char buffer [32];

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&ts));

    return string(buffer);
}

