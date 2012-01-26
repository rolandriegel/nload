/***************************************************************************
                                  device.h
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

#ifndef DEVICE_H
#define DEVICE_H

#include "dataframe.h"
#include "graph.h"
#include "statistics.h"

#include <string>

class DevReader;
class Window;

class Device
{
    public:
        explicit Device(DevReader& devReader);
        ~Device();

        void update();
        void print(Window&);
        void setDeviceNumber(int deviceNumber);
        void setTotalNumberOfDevices(int totalNumberOfDevices);
        
    private:
        void fixOverflows(DataFrame& dataFrame, const DataFrame& dataFrameOld);
        unsigned long long fixOverflow(unsigned long long value, unsigned long long valueOld);

        void printTrafficValue(Window& window, int x, int y, const std::string& description, unsigned long long value);
        void printDataValue(Window& window, int x, int y, const std::string& description, unsigned long long value);
        void printStatisticsIn(Window& window, int x, int y);
        void printStatisticsOut(Window& window, int x, int y);

        int m_deviceNumber;
        int m_totalNumberOfDevices;
        
        Statistics m_deviceStatistics;
        Graph m_deviceGraphIn;
        Graph m_deviceGraphOut;

        DataFrame m_dataFrameOld;
        DevReader& m_devReader;
};

#endif

