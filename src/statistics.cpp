/***************************************************************************
                               statistics.cpp
                             -------------------
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

#include "setting.h"
#include "settingstore.h"
#include "statistics.h"

using namespace std;

void Statistics::insertDataFrame(const DataFrame& dataFrame)
{
    if(!dataFrame.isValid())
        return;

    m_dataFrames.push_back(dataFrame);

    unsigned int frameCount = m_dataFrames.size();
    if(frameCount < 2)
        return;

    unsigned int averageCount = getAverageWindow();
    unsigned int secondCount = getSecondWindow();

    if(frameCount > averageCount)
    {
        m_dataFrames.erase(m_dataFrames.begin(), m_dataFrames.begin() + (frameCount - averageCount));
        frameCount = averageCount;
    }

    vector<DataFrame>::const_reverse_iterator itFrameSecond = m_dataFrames.rbegin();
    vector<DataFrame>::const_reverse_iterator itFrameAverage = m_dataFrames.rbegin();

    if(frameCount > secondCount)
        itFrameSecond += secondCount;
    else
        itFrameSecond += frameCount - 1;

    if(frameCount > averageCount)
        itFrameAverage += averageCount;
    else
        itFrameAverage += frameCount - 1;

    calculateAverage(*itFrameSecond, m_dataFrames.back(), m_second);
    calculateAverage(*itFrameAverage, m_dataFrames.back(), m_average);

    if(frameCount > 2)
        calculateMinMax(m_second, m_min, m_max);
    else
        m_min = m_max = m_second;
}

void Statistics::reset()
{
    m_second = DataFrame();
    m_average = DataFrame();
    m_min = DataFrame();
    m_max = DataFrame();

    m_dataFrames.clear();
}

float Statistics::getUnitFactor(dataUnit unit, unsigned long long value)
{
    float factor = 1.0 / (unit % 2 == 0 ? 8 : 1);
    
    switch(unit)
    {
        case humanReadableBit:
        case humanReadableByte:
            for(int i = 0; i < 3; ++i)
            {
                if(value / factor < 1024)
                    return factor;

                factor *= 1024;
            }
            return factor;
        case bit:
        case byte:
            return factor;
        case kiloBit:
        case kiloByte:
            return factor * 1024;
        case megaBit:
        case megaByte:
            return factor * 1024 * 1024;
        case gigaBit:
        case gigaByte:
            return factor * 1024 * 1024 * 1024;
        default: // should never be executed
            return factor;
    }
}

string Statistics::getUnitString(dataUnit unit, unsigned long long value)
{
    const string description = (unit % 2 == 0 ? "Bit" : "Byte");
    const string units[] = { "", "k", "M", "G" };

    switch(unit)
    {
        case humanReadableBit:
        case humanReadableByte:
            value *= (unit % 2 == 0 ? 8 : 1);
            for(int i = 0; i < 3; ++i)
            {
                if(value < 1024)
                    return units[i] + description;

                value /= 1024;
            }
            return units[3] + description;
        case bit:
        case byte:
            return description;
        case kiloBit:
        case kiloByte:
            return 'k' + description;
        case megaBit:
        case megaByte:
            return 'M' + description;
        case gigaBit:
        case gigaByte:
            return 'G' + description;
        default: // should never be executed
            return description;
    }
}

void Statistics::calculateAverage(const DataFrame& dataFrameFrom, const DataFrame& dataFrameTo, DataFrame& result)
{
    float timeSpan = (dataFrameTo.getTimeStampSeconds() + dataFrameTo.getTimeStampMicroseconds() / 1000000.0) -
                     (dataFrameFrom.getTimeStampSeconds() + dataFrameFrom.getTimeStampMicroseconds() / 1000000.0);

    if(timeSpan <= 0)
        return;

    result.setTotalDataIn((unsigned long long)((dataFrameTo.getTotalDataIn() - dataFrameFrom.getTotalDataIn()) / timeSpan));
    result.setTotalDataOut((unsigned long long)((dataFrameTo.getTotalDataOut() - dataFrameFrom.getTotalDataOut()) / timeSpan));
    result.setTotalPacketsIn((unsigned long long)((dataFrameTo.getTotalPacketsIn() - dataFrameFrom.getTotalPacketsIn()) / timeSpan));
    result.setTotalPacketsOut((unsigned long long)((dataFrameTo.getTotalPacketsOut() - dataFrameFrom.getTotalPacketsOut()) / timeSpan));
    result.setTotalErrorsIn((unsigned long long)((dataFrameTo.getTotalErrorsIn() - dataFrameFrom.getTotalErrorsIn()) / timeSpan));
    result.setTotalErrorsOut((unsigned long long)((dataFrameTo.getTotalErrorsOut() - dataFrameFrom.getTotalErrorsOut()) / timeSpan));
    result.setTotalDropsIn((unsigned long long)((dataFrameTo.getTotalDropsIn() - dataFrameFrom.getTotalDropsIn()) / timeSpan));
    result.setTotalDropsOut((unsigned long long)((dataFrameTo.getTotalDropsOut() - dataFrameFrom.getTotalDropsOut()) / timeSpan));
}

void Statistics::calculateMinMax(const DataFrame& dataFrame, DataFrame& min, DataFrame& max)
{
    if(dataFrame.getTotalDataIn() < min.getTotalDataIn())
        min.setTotalDataIn(dataFrame.getTotalDataIn());
    if(dataFrame.getTotalDataIn() > max.getTotalDataIn())
        max.setTotalDataIn(dataFrame.getTotalDataIn());
    if(dataFrame.getTotalDataOut() < min.getTotalDataOut())
        min.setTotalDataOut(dataFrame.getTotalDataOut());
    if(dataFrame.getTotalDataOut() > max.getTotalDataOut())
        max.setTotalDataOut(dataFrame.getTotalDataOut());

    if(dataFrame.getTotalPacketsIn() < min.getTotalPacketsIn())
        min.setTotalPacketsIn(dataFrame.getTotalPacketsIn());
    if(dataFrame.getTotalPacketsIn() > max.getTotalPacketsIn())
        max.setTotalPacketsIn(dataFrame.getTotalPacketsIn());
    if(dataFrame.getTotalPacketsOut() < min.getTotalPacketsOut())
        min.setTotalPacketsOut(dataFrame.getTotalPacketsOut());
    if(dataFrame.getTotalPacketsOut() > max.getTotalPacketsOut())
        max.setTotalPacketsOut(dataFrame.getTotalPacketsOut());

    if(dataFrame.getTotalErrorsIn() < min.getTotalErrorsIn())
        min.setTotalErrorsIn(dataFrame.getTotalErrorsIn());
    if(dataFrame.getTotalErrorsIn() > max.getTotalErrorsIn())
        max.setTotalErrorsIn(dataFrame.getTotalErrorsIn());
    if(dataFrame.getTotalErrorsOut() < min.getTotalErrorsOut())
        min.setTotalErrorsOut(dataFrame.getTotalErrorsOut());
    if(dataFrame.getTotalErrorsOut() > max.getTotalErrorsOut())
        max.setTotalErrorsOut(dataFrame.getTotalErrorsOut());

    if(dataFrame.getTotalDropsIn() < min.getTotalDropsIn())
        min.setTotalDropsIn(dataFrame.getTotalDropsIn());
    if(dataFrame.getTotalDropsIn() > max.getTotalDropsIn())
        max.setTotalDropsIn(dataFrame.getTotalDropsIn());
    if(dataFrame.getTotalDropsOut() < min.getTotalDropsOut())
        min.setTotalDropsOut(dataFrame.getTotalDropsOut());
    if(dataFrame.getTotalDropsOut() > max.getTotalDropsOut())
        max.setTotalDropsOut(dataFrame.getTotalDropsOut());
}

unsigned int Statistics::getAverageWindow()
{
    unsigned int refreshInterval = SettingStore::get("RefreshInterval");
    unsigned int averageWindow = SettingStore::get("AverageWindow");

    return (unsigned int) (1000.0 / refreshInterval * averageWindow);
}

unsigned int Statistics::getSecondWindow()
{
    unsigned int refreshInterval = SettingStore::get("RefreshInterval");
    unsigned int secondWindow = (unsigned int) (1000.0 / refreshInterval);

    return secondWindow > 0 ? secondWindow : 1;
}

