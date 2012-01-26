/***************************************************************************
                               statistics.h
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

#ifndef STATISTICS_H
#define STATISTICS_H

#include "dataframe.h"

#include <string>
#include <vector>

class Statistics
{
    public:
        enum dataUnit
        {
            humanReadableBit = -2,
            humanReadableByte = -1,
            bit = 0,
            byte = 1,
            kiloBit = 2,
            kiloByte = 3,
            megaBit = 4,
            megaByte = 5,
            gigaBit = 6,
            gigaByte = 7
        };
        
        Statistics() {}
        ~Statistics() {}

        static float getUnitFactor(dataUnit unit, unsigned long long value);
        static std::string getUnitString(dataUnit unit, unsigned long long value);
        
        void insertDataFrame(const DataFrame& dataFrame);
        void reset();

        bool isValid() const { return !m_dataFrames.empty(); }

        unsigned long long getDataInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDataIn(); }
        unsigned long long getDataOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDataOut(); }
        unsigned long long getPacketsInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalPacketsIn(); }
        unsigned long long getPacketsOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalPacketsOut(); }
        unsigned long long getErrorsInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalErrorsIn(); }
        unsigned long long getErrorsOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalErrorsOut(); }
        unsigned long long getDropsInTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDropsIn(); }
        unsigned long long getDropsOutTotal() const { return m_dataFrames.empty() ? 0 : m_dataFrames.back().getTotalDropsOut(); }

        unsigned long long getDataInPerSecond() const { return m_second.getTotalDataIn(); }
        unsigned long long getDataOutPerSecond() const { return m_second.getTotalDataOut(); }
        unsigned long long getPacketsInPerSecond() const { return m_second.getTotalPacketsIn(); }
        unsigned long long getPacketsOutPerSecond() const { return m_second.getTotalPacketsOut(); }
        unsigned long long getErrorsInPerSecond() const { return m_second.getTotalErrorsIn(); }
        unsigned long long getErrorsOutPerSecond() const { return m_second.getTotalErrorsOut(); }
        unsigned long long getDropsInPerSecond() const { return m_second.getTotalDropsIn(); }
        unsigned long long getDropsOutPerSecond() const { return m_second.getTotalDropsOut(); }

        unsigned long long getDataInAverage() const { return m_average.getTotalDataIn(); }
        unsigned long long getDataOutAverage() const { return m_average.getTotalDataOut(); }
        unsigned long long getPacketsInAverage() const { return m_average.getTotalPacketsIn(); }
        unsigned long long getPacketsOutAverage() const { return m_average.getTotalPacketsOut(); }
        unsigned long long getErrorsInAverage() const { return m_average.getTotalErrorsIn(); }
        unsigned long long getErrorsOutAverage() const { return m_average.getTotalErrorsOut(); }
        unsigned long long getDropsInAverage() const { return m_average.getTotalDropsIn(); }
        unsigned long long getDropsOutAverage() const { return m_average.getTotalDropsOut(); }

        unsigned long long getDataInMin() const { return m_min.getTotalDataIn(); }
        unsigned long long getDataOutMin() const { return m_min.getTotalDataOut(); }
        unsigned long long getPacketsInMin() const { return m_min.getTotalPacketsIn(); }
        unsigned long long getPacketsOutMin() const { return m_min.getTotalPacketsOut(); }
        unsigned long long getErrorsInMin() const { return m_min.getTotalErrorsIn(); }
        unsigned long long getErrorsOutMin() const { return m_min.getTotalErrorsOut(); }
        unsigned long long getDropsInMin() const { return m_min.getTotalDropsIn(); }
        unsigned long long getDropsOutMin() const { return m_min.getTotalDropsOut(); }

        unsigned long long getDataInMax() const { return m_max.getTotalDataIn(); }
        unsigned long long getDataOutMax() const { return m_max.getTotalDataOut(); }
        unsigned long long getPacketsInMax() const { return m_max.getTotalPacketsIn(); }
        unsigned long long getPacketsOutMax() const { return m_max.getTotalPacketsOut(); }
        unsigned long long getErrorsInMax() const { return m_max.getTotalErrorsIn(); }
        unsigned long long getErrorsOutMax() const { return m_max.getTotalErrorsOut(); }
        unsigned long long getDropsInMax() const { return m_max.getTotalDropsIn(); }
        unsigned long long getDropsOutMax() const { return m_max.getTotalDropsOut(); }

    private:
        void calculateAverage(const DataFrame& dataFrameFrom, const DataFrame& dataFrameTo, DataFrame& result);
        void calculateMinMax(const DataFrame& dataFrame, DataFrame& min, DataFrame& max);
        unsigned int getAverageWindow();
        unsigned int getSecondWindow();

        DataFrame m_second;
        DataFrame m_average;
        DataFrame m_min;
        DataFrame m_max;

        std::vector<DataFrame> m_dataFrames;
};

#endif

