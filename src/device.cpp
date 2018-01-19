/***************************************************************************
                                  dev.cpp
                             -------------------
    begin                : Wed Aug 1 2001
    copyright            : (C) 2001 - 2012 by Roland Riegel
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

#include "device.h"
#include "devreader.h"
#include "graph.h"
#include "setting.h"
#include "settingstore.h"
#include "stringutils.h"
#include "window.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

Device::Device(DevReader& devReader)
    : m_deviceNumber(0), m_totalNumberOfDevices(0), m_devReader(devReader)
{
}

Device::~Device()
{
}

// update the device's data
void Device::update()
{
    // read current traffic
    DataFrame dataFrame = m_devReader.getNewDataFrame();

    if(dataFrame.isValid())
    {
        /* Depending on the CPU architecture and the OS interface
         * used for reading the device statistics, the counts can
         * overflow. We monitor the overflows and fix them.
         */
        fixOverflows(dataFrame, m_dataFrameOld);

        m_deviceStatistics.insertDataFrame(dataFrame);

        m_deviceGraphIn.update(m_deviceStatistics.getDataInPerSecond());
        m_deviceGraphOut.update(m_deviceStatistics.getDataOutPerSecond());

        m_dataFrameOld = dataFrame;
    }
    else
    {
        m_deviceStatistics.reset();
        m_deviceGraphIn.resetTrafficData();
        m_deviceGraphOut.resetTrafficData();
    }
}

// print the device's data
void Device::print(Window& window)
{
    int width = window.getWidth();
    int height = window.getHeight();

    // print header
    if(height > 2)
    {
        if(m_deviceStatistics.isValid())
        {
            string ip4 = m_dataFrameOld.getIpV4();
            if(!ip4.empty())
                window.print() << "Device " << m_devReader.getDeviceName() << " [" << ip4 << "] (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "):" << endl;
            else
                window.print() << "Device " << m_devReader.getDeviceName() << " (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "):" << endl;
            window.print() << string(width, '=');
        }
        else
        {
            // if device does not exist print warning message
            window.print() << "Device " << m_devReader.getDeviceName() << " (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "): does not exist" << endl;
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
void Device::setDeviceNumber(int deviceNumber)
{
    m_deviceNumber = deviceNumber;
}

// set the total number of shown devices (for display only)
void Device::setTotalNumberOfDevices(int totalNumberOfDevices)
{
    m_totalNumberOfDevices = totalNumberOfDevices;
}

void Device::fixOverflows(DataFrame& dataFrame, const DataFrame& dataFrameOld)
{
    if(!dataFrame.isValid() || !dataFrameOld.isValid())
        return;

    dataFrame.setTotalDataIn(fixOverflow(dataFrame.getTotalDataIn(), dataFrameOld.getTotalDataIn()));
    dataFrame.setTotalDataOut(fixOverflow(dataFrame.getTotalDataOut(), dataFrameOld.getTotalDataOut()));
    dataFrame.setTotalPacketsIn(fixOverflow(dataFrame.getTotalPacketsIn(), dataFrameOld.getTotalPacketsIn()));
    dataFrame.setTotalPacketsOut(fixOverflow(dataFrame.getTotalPacketsOut(), dataFrameOld.getTotalPacketsOut()));
    dataFrame.setTotalErrorsIn(fixOverflow(dataFrame.getTotalErrorsIn(), dataFrameOld.getTotalErrorsIn()));
    dataFrame.setTotalErrorsOut(fixOverflow(dataFrame.getTotalErrorsOut(), dataFrameOld.getTotalErrorsOut()));
    dataFrame.setTotalDropsIn(fixOverflow(dataFrame.getTotalDropsIn(), dataFrameOld.getTotalDropsIn()));
    dataFrame.setTotalDropsOut(fixOverflow(dataFrame.getTotalDropsOut(), dataFrameOld.getTotalDropsOut()));
}

unsigned long long Device::fixOverflow(unsigned long long value, unsigned long long valueOld)
{
    if(value > 0xffffffffULL)
        return value;

    if(value < (valueOld & 0xffffffffULL))
    {
        // overflow happend (again)
        valueOld += 0x100000000ULL;
    }

    // no overflow happend, keep previous ones
    valueOld &= 0xffffffff00000000ULL;
    value |= valueOld;
    return value;
}

unsigned long long Device::roundUpMaxDeflection(unsigned long long value)
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

string Device::formatTrafficValue(unsigned long value, int precision)
{
    Statistics::dataUnit trafficFormat = (Statistics::dataUnit) ((int) SettingStore::get("TrafficFormat"));

    string unitString = Statistics::getUnitString(trafficFormat, value);
    float unitFactor = Statistics::getUnitFactor(trafficFormat, value);

    ostringstream oss;
    oss << fixed << setprecision(precision) << ((float) value / unitFactor) << " " << unitString << "/s";

    return oss.str();
}

string Device::formatDataValue(unsigned long long value, int precision)
{
    Statistics::dataUnit dataFormat = (Statistics::dataUnit) ((int) SettingStore::get("DataFormat"));

    string unitString = Statistics::getUnitString(dataFormat, value);
    float unitFactor = Statistics::getUnitFactor(dataFormat, value);

    ostringstream oss;
    oss << fixed << setprecision(precision) << ((float) value / unitFactor) << " " << unitString << endl;

    return oss.str();
}

void Device::generateStatisticsIn(vector<string>& statisticLines)
{
    statisticLines.push_back("Cur: " + formatTrafficValue(m_deviceStatistics.getDataInPerSecond(), 2));
    statisticLines.push_back("Avg: " + formatTrafficValue(m_deviceStatistics.getDataInAverage(), 2));
    statisticLines.push_back("Min: " + formatTrafficValue(m_deviceStatistics.getDataInMin(), 2));
    statisticLines.push_back("Max: " + formatTrafficValue(m_deviceStatistics.getDataInMax(), 2));
    statisticLines.push_back("Ttl: " + formatDataValue(m_deviceStatistics.getDataInTotal(), 2));
}

void Device::generateStatisticsOut(vector<string>& statisticLines)
{
    statisticLines.push_back("Cur: " + formatTrafficValue(m_deviceStatistics.getDataOutPerSecond(), 2));
    statisticLines.push_back("Avg: " + formatTrafficValue(m_deviceStatistics.getDataOutAverage(), 2));
    statisticLines.push_back("Min: " + formatTrafficValue(m_deviceStatistics.getDataOutMin(), 2));
    statisticLines.push_back("Max: " + formatTrafficValue(m_deviceStatistics.getDataOutMax(), 2));
    statisticLines.push_back("Ttl: " + formatDataValue(m_deviceStatistics.getDataOutTotal(), 2));
}

void Device::printStatistics(Window& window, const vector<string>& statisticLines, int x, int y)
{
    for(vector<string>::const_iterator itLine = statisticLines.begin(); itLine != statisticLines.end(); ++itLine)
    {
        window.print(x, y++) << *itLine;
    }
}

