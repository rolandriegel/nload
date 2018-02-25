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

#include "deviceview.h"

#include "device.h"
#include "graph.h"
#include "setting.h"
#include "settingstore.h"
#include "stringutils.h"
#include "window.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

DeviceView::DeviceView(Device* device)
    : m_deviceNumber(0), m_totalNumberOfDevices(0), m_device(device)
{
}

DeviceView::~DeviceView()
{
}

void DeviceView::update()
{
    if(m_device->exists())
    {
        const Statistics& statistics = m_device->getStatistics();

        m_deviceGraphIn.update(statistics.getDataInPerSecond());
        m_deviceGraphOut.update(statistics.getDataOutPerSecond());
    }
    else
    {
        m_deviceGraphIn.resetTrafficData();
        m_deviceGraphOut.resetTrafficData();
    }
}

// print the device's data
void DeviceView::print(Window& window)
{
    int width = window.getWidth();
    int height = window.getHeight();

    // print header
    if(height > 2)
    {
        string deviceName = m_device->getName();
        string ipv4 = m_device->getIpV4Address();

        if(m_device->exists())
        {
            if(!ipv4.empty())
                window.print() << "Device " << deviceName << " [" << ipv4 << "] (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "):" << endl;
            else
                window.print() << "Device " << deviceName << " (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "):" << endl;
            window.print() << string(width, '=');
        }
        else
        {
            // if device does not exist print warning message
            window.print() << "Device " << deviceName << " (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "): does not exist" << endl;
            window.print() << string(width, '=') << endl;

            // and exit
            return;
        }
    }
    if(width < 25 || height < 8)
    {
        window.print() << "Please enlarge console for viewing device information." << endl;
        return;
    }
    
    // format statistics
    vector<string> statLinesIn;
    vector<string> statLinesOut;

    generateStatisticsIn(statLinesIn);
    generateStatisticsOut(statLinesOut);

    size_t statLineInMaxLength = max_element(statLinesIn.begin(), statLinesIn.end(), sizeLess())->size();
    size_t statLineOutMaxLength = max_element(statLinesOut.begin(), statLinesOut.end(), sizeLess())->size();
    int statLineMaxLength = statLineInMaxLength > statLineOutMaxLength ? statLineInMaxLength : statLineOutMaxLength;

    // if graphs should be hidden ...
    if(SettingStore::get("MultipleDevices"))
    {
        window.print() << "Incoming:";
        window.print(width / 2) << "Outgoing:" << endl;
        
        int statusY = window.getY();
        
        printStatistics(window, statLinesIn, 0, statusY);
        printStatistics(window, statLinesOut, width / 2, statusY);
        
        window.print() << endl;
    }
    // ... or not
    else
    {
        // calculate layout
        int lines = height - window.getY();
        int linesForIn = (lines + 1) / 2;
        int linesForOut = lines - linesForIn;
        int dirInY = window.getY();
        int dirOutY = dirInY + linesForIn;

        int statisticsX = width - statLineMaxLength - 1;
        statisticsX -= statisticsX % 5;

        if(linesForOut <= 5)
        {
            linesForIn = lines;
            linesForOut = 0;
            dirOutY = height;
        }

        // calculate deflection of graphs
        unsigned long long maxDeflectionIn = (unsigned long long) SettingStore::get("BarMaxIn") * 1024 / 8;
        unsigned long long maxDeflectionOut = (unsigned long long) SettingStore::get("BarMaxOut") * 1024 / 8;

        if(maxDeflectionIn < 1)
            maxDeflectionIn = roundUpMaxDeflection(m_deviceGraphIn.calcMaxDeflection());
        if(maxDeflectionOut < 1)
            maxDeflectionOut = roundUpMaxDeflection(m_deviceGraphOut.calcMaxDeflection());

        // print incoming data
        if(linesForIn > 5)
        {
            window.print(0, dirInY) << "Incoming (100% @ " << formatTrafficValue(maxDeflectionIn, 0) << "):" << endl;

            if(statisticsX > 1)
            {
                m_deviceGraphIn.setNumOfBars(statisticsX - 1);
                m_deviceGraphIn.setHeightOfBars(linesForIn - 1);
                m_deviceGraphIn.setMaxDeflection(maxDeflectionIn);
                m_deviceGraphIn.print(window, 0, dirInY + 1);
            }

            if(width > statLineMaxLength)
                printStatistics(window, statLinesIn, statisticsX, dirInY + linesForIn - 5);
        }


        // print outgoing data
        if(linesForOut > 5)
        {
            window.print(0, dirOutY) << "Outgoing (100% @ " << formatTrafficValue(maxDeflectionOut, 0) << "):" << endl;

            if(statisticsX > 1)
            {
                m_deviceGraphOut.setNumOfBars(statisticsX - 1);
                m_deviceGraphOut.setHeightOfBars(linesForOut - 1);
                m_deviceGraphOut.setMaxDeflection(maxDeflectionOut);
                m_deviceGraphOut.print(window, 0, dirOutY + 1);
            }

            if(width > statLineMaxLength)
                printStatistics(window, statLinesOut, statisticsX, dirOutY + linesForOut - 5);
        }
    }
}

// set the number identifying the device (for display only)
void DeviceView::setDeviceNumber(int deviceNumber)
{
    m_deviceNumber = deviceNumber;
}

// set the total number of shown devices (for display only)
void DeviceView::setTotalNumberOfDevices(int totalNumberOfDevices)
{
    m_totalNumberOfDevices = totalNumberOfDevices;
}

unsigned long long DeviceView::roundUpMaxDeflection(unsigned long long value)
{
    unsigned long long rounded = 2 * 1024; // 2 kByte/s
    while (rounded < value)
    {
        if((rounded << 1) < rounded)
            return value;

        rounded <<= 1;
    }

    return rounded;
}

string DeviceView::formatTrafficValue(unsigned long value, int precision)
{
    Statistics::dataUnit trafficFormat = (Statistics::dataUnit) ((int) SettingStore::get("TrafficFormat"));

    string unitString = Statistics::getUnitString(trafficFormat, value);
    float unitFactor = Statistics::getUnitFactor(trafficFormat, value);

    ostringstream oss;
    oss << fixed << setprecision(precision) << ((float) value / unitFactor) << " " << unitString << "/s";

    return oss.str();
}

string DeviceView::formatDataValue(unsigned long long value, int precision)
{
    Statistics::dataUnit dataFormat = (Statistics::dataUnit) ((int) SettingStore::get("DataFormat"));

    string unitString = Statistics::getUnitString(dataFormat, value);
    float unitFactor = Statistics::getUnitFactor(dataFormat, value);

    ostringstream oss;
    oss << fixed << setprecision(precision) << ((float) value / unitFactor) << " " << unitString << endl;

    return oss.str();
}

void DeviceView::generateStatisticsIn(vector<string>& statisticLines)
{
    const Statistics& statistics = m_device->getStatistics();

    statisticLines.push_back("Cur: " + formatTrafficValue(statistics.getDataInPerSecond(), 2));
    statisticLines.push_back("Avg: " + formatTrafficValue(statistics.getDataInAverage(), 2));
    statisticLines.push_back("Min: " + formatTrafficValue(statistics.getDataInMin(), 2));
    statisticLines.push_back("Max: " + formatTrafficValue(statistics.getDataInMax(), 2));
    statisticLines.push_back("Ttl: " + formatDataValue(statistics.getDataInTotal(), 2));
}

void DeviceView::generateStatisticsOut(vector<string>& statisticLines)
{
    const Statistics& statistics = m_device->getStatistics();

    statisticLines.push_back("Cur: " + formatTrafficValue(statistics.getDataOutPerSecond(), 2));
    statisticLines.push_back("Avg: " + formatTrafficValue(statistics.getDataOutAverage(), 2));
    statisticLines.push_back("Min: " + formatTrafficValue(statistics.getDataOutMin(), 2));
    statisticLines.push_back("Max: " + formatTrafficValue(statistics.getDataOutMax(), 2));
    statisticLines.push_back("Ttl: " + formatDataValue(statistics.getDataOutTotal(), 2));
}

void DeviceView::printStatistics(Window& window, const vector<string>& statisticLines, int x, int y)
{
    for(vector<string>::const_iterator itLine = statisticLines.begin(); itLine != statisticLines.end(); ++itLine)
    {
        window.print(x, y++) << *itLine;
    }
}

