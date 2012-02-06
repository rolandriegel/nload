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
#include "window.h"

#include <iomanip>

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
    // if device does not exist
    if(!m_deviceStatistics.isValid())
    {
        // ... print warning message ...
        window.print() << "Device " << m_devReader.getDeviceName() << " (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "): does not exist" << endl;
        window.print() << string(window.getWidth(), '=') << endl;
        
        // ... and exit
        return;
    }
    
    // print header
    string ip4 = m_dataFrameOld.getIpV4();
    if(!ip4.empty())
        window.print() << "Device " << m_devReader.getDeviceName() << " [" << ip4 << "] (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "):" << endl;
    else
        window.print() << "Device " << m_devReader.getDeviceName() << " (" << (m_deviceNumber + 1) << "/" << m_totalNumberOfDevices << "):" << endl;
    window.print() << string(window.getWidth(), '=');
    
    // if graphs should be hidden ...
    if(SettingStore::get("MultipleDevices"))
    {
        window.print() << "Incoming:";
        window.print(window.getWidth() / 2) << "Outgoing:" << endl;
        
        int statusY = window.getY();
        
        printStatisticsIn(window, 0, statusY);
        printStatisticsOut(window, window.getWidth() / 2, statusY);
        
        window.print() << endl;
    }
    // ... or not
    else
    {
        // incoming traffic
        window.print() << "Incoming:" << endl;
        
        m_deviceGraphIn.setNumOfBars(window.getWidth() * 2 / 3);
        m_deviceGraphIn.setHeightOfBars((window.getHeight() - window.getY() - 1) / 2);
        m_deviceGraphIn.setMaxDeflection((unsigned long long) SettingStore::get("BarMaxIn") * 1024 / 8);
        m_deviceGraphIn.print(window, 0, window.getY());
        
        printStatisticsIn(window, window.getWidth() * 2 / 3 + 2, window.getY() - 5);
        
        // outgoing traffic
        window.print() << "Outgoing:" << endl;
        
        m_deviceGraphOut.setNumOfBars(window.getWidth() * 2 / 3);
        m_deviceGraphOut.setHeightOfBars(window.getHeight() - window.getY());
        m_deviceGraphOut.setMaxDeflection((unsigned long long) SettingStore::get("BarMaxOut") * 1024 / 8);
        m_deviceGraphOut.print(window, 0, window.getY());
        
        printStatisticsOut(window, window.getWidth() * 2 / 3 + 2, window.getY() - 4);
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

void Device::printTrafficValue(Window& window, int x, int y, const std::string& description, unsigned long long value)
{
    Statistics::dataUnit trafficFormat = (Statistics::dataUnit) ((int) SettingStore::get("TrafficFormat"));

    string unitString = Statistics::getUnitString(trafficFormat, value);
    float unitFactor = Statistics::getUnitFactor(trafficFormat, value);

    window.print(x, y) << fixed << setprecision(2) << description << ": " << ((float) value / unitFactor) << " " << unitString << "/s" << endl;
}

void Device::printDataValue(Window& window, int x, int y, const std::string& description, unsigned long long value)
{
    Statistics::dataUnit dataFormat = (Statistics::dataUnit) ((int) SettingStore::get("DataFormat"));

    string unitString = Statistics::getUnitString(dataFormat, value);
    float unitFactor = Statistics::getUnitFactor(dataFormat, value);

    window.print(x, y) << fixed << setprecision(2) << description << ": " << ((float) value / unitFactor) << " " << unitString << endl;
}

void Device::printStatisticsIn(Window& window, int x, int y)
{
    // print current traffic
    printTrafficValue(window, x, y, "Curr", m_deviceStatistics.getDataInPerSecond());
    
    // print average traffic
    printTrafficValue(window, x, y + 1, "Avg", m_deviceStatistics.getDataInAverage());
    
    // print min traffic since nload start
    printTrafficValue(window, x, y + 2, "Min", m_deviceStatistics.getDataInMin());
    
    // print max traffic since nload start
    printTrafficValue(window, x, y + 3, "Max", m_deviceStatistics.getDataInMax());
    
    // print total traffic since last system reboot
    printDataValue(window, x, y + 4, "Ttl", m_deviceStatistics.getDataInTotal());
}

void Device::printStatisticsOut(Window& window, int x, int y)
{
    // print current traffic
    printTrafficValue(window, x, y, "Curr", m_deviceStatistics.getDataOutPerSecond());
    
    // print average traffic
    printTrafficValue(window, x, y + 1, "Avg", m_deviceStatistics.getDataOutAverage());
    
    // print min traffic since nload start
    printTrafficValue(window, x, y + 2, "Min", m_deviceStatistics.getDataOutMin());
    
    // print max traffic since nload start
    printTrafficValue(window, x, y + 3, "Max", m_deviceStatistics.getDataOutMax());
    
    // print total traffic since last system reboot
    printDataValue(window, x, y + 4, "Ttl", m_deviceStatistics.getDataOutTotal());
}

