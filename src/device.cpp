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

#include "device.h"
#include "devreader.h"

using namespace std;

Device::Device(DevReader* devReader)
    : m_devReader(devReader)
{
    m_name = m_devReader->getDeviceName();
}

Device::~Device()
{
}

bool Device::exists() const
{
    return m_dataFrameOld.isValid();
}

const string& Device::getName() const
{
    return m_name;
}

const string& Device::getIpV4Address() const
{
    return m_ipv4;
}

const Statistics& Device::getStatistics() const
{
    return m_deviceStatistics;
}

// update the device's data
void Device::update()
{
    // read current traffic
    DataFrame dataFrame = m_devReader->getNewDataFrame();

    /* Depending on the CPU architecture and the OS interface
     * used for reading the device statistics, the counts can
     * overflow. We monitor the overflows and fix them.
     */
    fixOverflows(dataFrame, m_dataFrameOld);
    m_deviceStatistics.insertDataFrame(dataFrame);

    m_name = dataFrame.getDeviceName();
    m_ipv4 = dataFrame.getIpV4();

    m_dataFrameOld = dataFrame;
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

